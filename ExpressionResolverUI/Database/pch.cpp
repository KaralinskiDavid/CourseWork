// pch.cpp: файл исходного кода, соответствующий предварительно скомпилированному заголовочному файлу

#include "pch.h"
#include "node.h"
#include "HistoryLine.h"
#include "fileposition.h"
#include "user.h"

#define FRAME_SIZE 65536
#define LINES_ON_PAGE 30


using namespace std;

wstring _userPath = L"Users.txt";
wstring _historyPath;
FilePosition* lastPosition;
bool initialized = false;

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
	wstring delimiter = wstring(L" ");
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

Node<size_t, FilePosition>* UsernameIndex = NULL;


void loadUsers()
{
	User* user;
	//vector<PhoneBookLine*> phonebook;

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
						lastPosition = filePostition;
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

HistoryLine* getHistoryLine(wstring string)
{
	wstring delimiter = wstring(L" ");
	vector<wstring> substrings;
	HistoryLine* historyLine = NULL;
	try
	{
		substrings = splitline(string, delimiter);
		historyLine = new HistoryLine(
			substrings[0],
			substrings[1],
			substrings[2],
			substrings[3],
			substrings[4]);
	}
	catch (...)
	{

	}
	return historyLine;
}

Node<size_t, FilePosition>* HistoryIndex = NULL;

extern _declspec(dllexport) vector<HistoryLine*> loadHistory(wstring path, int page)
{
	HistoryLine* historyLine;
	vector<HistoryLine*> history;

	HANDLE hFile = CreateFile(_historyPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int fileSize = GetFileSize(hFile, NULL);
	HANDLE hMem = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);

	int fileMappingOffset = 0;
	int linesCount = 0;
	vector<wstring> records;
	hash<wstring> hash;
	try {
		while (fileMappingOffset < fileSize)
		{
			int frameMappingSize = fileMappingOffset + FRAME_SIZE > fileSize ? fileSize - fileMappingOffset : FRAME_SIZE;
			char* mappedTextPtr = (char*)MapViewOfFile(hMem, FILE_MAP_READ, 0, fileMappingOffset, frameMappingSize);
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
				if (currentPage == page)
					history.push_back(historyLine);
				if (!initialized)
				{
					FilePosition* filePostition = new FilePosition(currentPage, fileMappingOffset, offset, i);
					HistoryIndex->insert(&HistoryIndex, hash(historyLine->username), filePostition);
				}
				linesCount++;
				offset += i + 1;
			}
			fileMappingOffset += frameMappingSize;
			UnmapViewOfFile(mappedTextPtr);
		}
	}
	catch (...)
	{
	}
	CloseHandle(hMem);
	CloseHandle(hFile);

	initialized = true;
	return history;
}

vector<HistoryLine*> searchHistoryLineByFilePostition(vector<FilePosition*> positions, int page)
{
	vector<HistoryLine*> lines;
	int lowLimit = page * LINES_ON_PAGE;
	int highLimit = positions.size() > lowLimit + LINES_ON_PAGE ? lowLimit + LINES_ON_PAGE : positions.size();
	for (int i = lowLimit; i < highLimit; i++)
	{
		lines.push_back(getHistoryLineByPosition(positions[i]));
	}
	return lines;
}

void addNewUserToIndex(wstring login, wstring line)
{
	hash<wstring> hash;
	FilePosition* newPosition;
	if (lastPosition != NULL)
		newPosition = new FilePosition(lastPosition->pageNumber, lastPosition->outerOffset, lastPosition->innerOffset + lastPosition->lineSize, line.length());
	else
		newPosition = new FilePosition(0, 0, 0, line.length());
	UsernameIndex->insert(&UsernameIndex, hash(login), newPosition);
	lastPosition = newPosition;
}

extern _declspec(dllexport) bool registerUser(wstring login, wstring password)
{
	if (!initialized)
	{
		loadUsers();
		initialized = true;
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
			login.push_back(' ');
			wstring line = login + to_wstring(hash(password));
			line.push_back('\r');
			line.push_back('\n');
			fputws(line.c_str(), usersTable);
			login.pop_back();
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
	if (!initialized)
	{
		loadUsers();
		initialized = true;
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

extern _declspec(dllexport) void addHistoryLine(wstring line)
{
	hash<wstring> hash;
	string path(_historyPath.begin(), _historyPath.end());
	FILE* historyTable;
	fopen_s(&historyTable, path.c_str(), "a");
	if (historyTable != 0)
	{
		try {
			fputws(line.c_str(), historyTable);
			fclose(historyTable);
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