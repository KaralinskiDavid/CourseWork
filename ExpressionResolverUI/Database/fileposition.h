#pragma once

class FilePosition
{
public:
	unsigned int pageNumber;
	unsigned int outerOffset;
	unsigned int innerOffset;
	unsigned int lineSize;
	FilePosition(int pageNumber, int outerOffset, int innerOffset, int lineSize);
};

FilePosition::FilePosition(int pageNumber, int outerOffset, int innerOffset, int lineSize)
{
	this->pageNumber = pageNumber;
	this->outerOffset = outerOffset;
	this->innerOffset = innerOffset;
	this->lineSize = lineSize;
}