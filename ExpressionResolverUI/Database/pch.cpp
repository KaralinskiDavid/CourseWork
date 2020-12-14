#include "pch.h"
#include "node.h"
#include "HistoryLine.h"
#include "fileposition.h"
#include "user.h"

#define FRAME_SIZE 65536
#define LINES_ON_PAGE 30


using namespace std;

wstring _userPath = L"Users.txt";
wstring _historyPath = L"History.txt";
FilePosition* lastUserPosition;
FilePosition* lastHistoryPosition;
bool usersInitialized = false;
bool historyInitialized = false;

vector<wstring> splitline(wstring line, wstring delimiter)
{
	vector<wstring> substrings;
	size_t prev = 0;
	size_t next;
	size_t delta = delimiter.length();

	while ((next = line.find(delimiter, prev)) != string::npos)
	{
		substrings.push_back(line.substr(prev, next - prev));
		prev = next + delta;
	}
	substrings.push_back(line.substr(prev));

	return substrings;
}

User* getUsersTableLine(wstring string)
{
	wstring delimiter = wstring(L"*");
	vector<wstring> substrings;
	User* user = NULL;
	try
	{
		substrings = splitline(string, delimiter);
		user = new User(
			substrings[0],
			_wtoi64(substrings[1].c_str())
		);
	}
	catch (...)
	{

	}
	return user;
}

HistoryLine* getHistoryLine(wstring string)
{
	wstring delimiter = wstring(L"~");
	vector<wstring> substrings;
	HistoryLine* historyLine = NULL;
	try
	{
		wstring white = L"~";
		substrings = splitline(string, delimiter);
		historyLine = new HistoryLine(
			substrings[0],
			substrings[1],
			substrings[2],
			substrings[3],
			substrings[4],
			substrings[5]);
	}
	catch (...)
	{

	}
	return historyLine;
}

User* getLineByPosition(FilePosition* position)
{
	User* user = NULL;
	HANDLE hFile = CreateFile(_userPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_READ , NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int fileSize = GetFileSize(hFile, NULL);
	HANDLE hMem = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
	if (hMem != 0)
	{
		try {
			int outerOffset = position->outerOffset;
			int innerOffset = position->innerOffset;
			int lineSize = position->lineSize;
			int frameMappingSize = outerOffset + FRAME_SIZE > fileSize ? fileSize - outerOffset : FRAME_SIZE;
			char* mappedTextPtr = (char*)MapViewOfFile(hMem, FILE_MAP_READ, 0, outerOffset, frameMappingSize);
			if (mappedTextPtr != 0)
			{
				wstring ws(&mappedTextPtr[innerOffset], &mappedTextPtr[innerOffset + lineSize]);
				user = getUsersTableLine(ws);
				UnmapViewOfFile(mappedTextPtr);
			}
		}
		catch (...)
		{
		}

		CloseHandle(hMem);
	}
	CloseHandle(hFile);
	return user;
}

HistoryLine* getHistoryLineByPosition(FilePosition* position)
{
	HistoryLine* historyLine = NULL;
	HANDLE hFile = CreateFile(_historyPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int fileSize = GetFileSize(hFile, NULL);
	HANDLE hMem = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
	if (hMem != 0)
	{
		try {
			int outerOffset = position->outerOffset;
			int innerOffset = position->innerOffset;
			int lineSize = position->lineSize;
			int frameMappingSize = outerOffset + FRAME_SIZE > fileSize ? fileSize - outerOffset : FRAME_SIZE;
			char* mappedTextPtr = (char*)MapViewOfFile(hMem, FILE_MAP_READ, 0, outerOffset, frameMappingSize);
			if (mappedTextPtr != 0)
			{
				wstring ws(&mappedTextPtr[innerOffset], &mappedTextPtr[innerOffset + lineSize]);
				historyLine = getHistoryLine(ws);
				UnmapViewOfFile(mappedTextPtr);
			}
		}
		catch (...)
		{
		}

		CloseHandle(hMem);
	}
	CloseHandle(hFile);
	return historyLine;
}

vector<HistoryLine*> searchHistoryByFilePostition(vector<FilePosition*> positions, int page)
{
	vector<HistoryLine*> lines;
	int lowLimit = page * LINES_ON_PAGE;
	int highLimit = positions.size() >= lowLimit + LINES_ON_PAGE ? positions.size() - lowLimit - 1 - LINES_ON_PAGE : 0;
	for (int i = positions.size()-lowLimit-1; i>highLimit; i--)
	{
		lines.push_back(getHistoryLineByPosition(positions[i]));
	}
	return lines;
}

Node<size_t, FilePosition>* UsernameIndex = NULL;


void loadUsers()
{
	User* user;

	HANDLE hFile = CreateFile(_userPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int fileSize = GetFileSize(hFile, NULL);
	HANDLE hMem = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
	if (hMem != 0)
	{
		int fileMappingOffset = 0;
		int linesCount = 0;
		vector<wstring> records;
		hash<wstring> hash;
		try {
			while (fileMappingOffset < fileSize)
			{
				int frameMappingSize = fileMappingOffset + FRAME_SIZE > fileSize ? fileSize - fileMappingOffset : FRAME_SIZE;
				char* mappedTextPtr = (char*)MapViewOfFile(hMem, FILE_MAP_READ, 0, fileMappingOffset, frameMappingSize);
				if (mappedTextPtr != 0)
				{
					int offset = 0;
					while (offset < frameMappingSize)
					{
						int i = 0;
						while (offset + i < frameMappingSize && mappedTextPtr[offset + i] != '\n')
						{
							i++;
						}
						wstring ws(&mappedTextPtr[offset], &mappedTextPtr[offset + i]);
						user = getUsersTableLine(ws);
						int currentPage = linesCount / LINES_ON_PAGE;
						FilePosition* filePostition = new FilePosition(currentPage, fileMappingOffset, offset, i);
						lastUserPosition = filePostition;
						UsernameIndex->insert(&UsernameIndex, hash(user->login), filePostition);
						linesCount++;
						offset += i + 1;
					}
					fileMappingOffset += frameMappingSize;
					UnmapViewOfFile(mappedTextPtr);
				}
			}
		}
		catch (...)
		{
		}
		CloseHandle(hMem);
	}
	CloseHandle(hFile);
	return;
}


Node<size_t, FilePosition>* HistoryIndex = NULL;

void loadHistory()
{
	HistoryLine* historyLine;

	HANDLE hFile = CreateFile(_historyPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int fileSize = GetFileSize(hFile, NULL);
	HANDLE hMem = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);

	if (hMem != 0)
	{
		int fileMappingOffset = 0;
		int linesCount = 0;
		vector<wstring> records;
		hash<wstring> hash;
		try {
			while (fileMappingOffset < fileSize)
			{
				int frameMappingSize = fileMappingOffset + FRAME_SIZE > fileSize ? fileSize - fileMappingOffset : FRAME_SIZE;
				char* mappedTextPtr = (char*)MapViewOfFile(hMem, FILE_MAP_READ, 0, fileMappingOffset, frameMappingSize);
				if (mappedTextPtr != 0)
				{
					int offset = 0;
					while (offset < frameMappingSize)
					{
						int i = 0;
						while (offset + i < frameMappingSize && mappedTextPtr[offset + i] != '\n')
						{
							i++;
						}
						wstring ws(&mappedTextPtr[offset], &mappedTextPtr[offset + i]);
						historyLine = getHistoryLine(ws);
						int currentPage = linesCount / LINES_ON_PAGE;
						FilePosition* filePostition = new FilePosition(currentPage, fileMappingOffset, offset, i);
						lastHistoryPosition = filePostition;
						HistoryIndex->insert(&HistoryIndex, hash(historyLine->username), filePostition);
						linesCount++;
						offset += i + 1;
					}
					fileMappingOffset += frameMappingSize;
					UnmapViewOfFile(mappedTextPtr);
				}
			}
		}
		catch (...)
		{
		}
		CloseHandle(hMem);
	}
	CloseHandle(hFile);
	return;
}

extern _declspec(dllexport) vector<HistoryLine*> getUserHistory(wstring username, int page)
{
	if (!historyInitialized)
	{
		loadHistory();
		historyInitialized = true;
	}
	hash<wstring> hash;
	return searchHistoryByFilePostition(HistoryIndex->getNodesByKey(HistoryIndex, hash(username)), page);
}

void addNewUserToIndex(wstring login, wstring line)
{
	hash<wstring> hash;
	FilePosition* newPosition;
	if (lastUserPosition != NULL)
		newPosition = new FilePosition(lastUserPosition->pageNumber, lastUserPosition->outerOffset, lastUserPosition->innerOffset + lastUserPosition->lineSize, line.length());
	else
		newPosition = new FilePosition(0, 0, 0, line.length());
	UsernameIndex->insert(&UsernameIndex, hash(login), newPosition);
	lastUserPosition = newPosition;
}

void addNewHistoryLineToIndex(wstring login, wstring line)
{
	hash<wstring> hash;
	FilePosition* newPosition;
	if (lastHistoryPosition != NULL)
		newPosition = new FilePosition(lastHistoryPosition->pageNumber, lastHistoryPosition->outerOffset, lastHistoryPosition->innerOffset + lastHistoryPosition->lineSize+1, line.length());
	else
		newPosition = new FilePosition(0, 0, 0, line.length());
	HistoryIndex->insert(&HistoryIndex, hash(login), newPosition);
	lastHistoryPosition = newPosition;
}

extern _declspec(dllexport) bool registerUser(wstring login, wstring password)
{
	if (!usersInitialized)
	{
		loadUsers();
		usersInitialized = true;
	}
	hash<wstring> hash;
	vector<FilePosition*> uv = UsernameIndex->getNodesByKey(UsernameIndex, hash(login));
	if (uv.size() != 0)
		return false;
	string path(_userPath.begin(), _userPath.end());
	FILE* usersTable;
	fopen_s(&usersTable, path.c_str(), "a");
	if (usersTable != 0)
	{
		try {
			login.push_back('*');
			wstring line = login + to_wstring(hash(password));
			line.push_back('\r');
			line.push_back('\n');
			fputws(line.c_str(), usersTable);
			login.pop_back();
			//nfvu
			//fveiv
			addNewUserToIndex(login, line);
			fclose(usersTable);
			return true;
		}
		catch (...)
		{
			return false;
		}
		fclose(usersTable);
	}
	return false;
}

extern _declspec(dllexport) bool authorize(wstring login, wstring password)
{
	if (!usersInitialized)
	{
		loadUsers();
		usersInitialized = true;
	}
	hash<wstring> hash;
	vector<FilePosition*> uv = UsernameIndex->getNodesByKey(UsernameIndex, hash(login));
	if (uv.size() == 0)
		return false;
	User* user = getLineByPosition(uv.back());
	if (hash(password) == user->passwordHash)
		return true;
	return false;
} 

wstring historyLineToWs(HistoryLine* historyLine)
{
	wstring line = historyLine->username;
	line.push_back('~');
	line += historyLine->date;
	line.push_back('~');
	line += historyLine->expression;
	line.push_back('~');
	line += historyLine->operation;
	line.push_back('~');
	line += historyLine->notation;
	line.push_back('~');
	line += historyLine->result;
	line.push_back('\n');
	return line;
}

extern _declspec(dllexport) void addHistoryLine(HistoryLine* historyLine)
{
	if (!historyInitialized)
	{
		loadHistory();
		historyInitialized = true;
	}
	wstring line = historyLineToWs(historyLine);
	string path(_historyPath.begin(), _historyPath.end());
	FILE* historyTable;
	fopen_s(&historyTable, path.c_str(), "a");
	if (historyTable != 0)
	{
		try {
			fputws(line.c_str(), historyTable);
			fclose(historyTable);
			addNewHistoryLineToIndex(historyLine->username, line);
			return;
		}
		catch (...)
		{
			return;
		}
		fclose(historyTable);
	}
	return;
}