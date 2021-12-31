#pragma once
#include "cparserFile.h"
#include<Windows.h>
#include<map>

using namespace std;

enum FILE_TYPE_Enum
{
	FILE_TYPE_UNKNOW,
	FILE_TYPE_WINDOWS_X86,
	FILE_TYPE_WINDOWS_X64,
	FILE_TYPE_LINUX_X86,
	FILE_TYPE_LINUX_X64,
};

class CscanFileType
{
public:
	int init();
	int Execute(const char* lpFilePath);
	void Save_File(const char* lpFilePath);
	int uninit();
private:
	map<int, void*> m_map_parserFile;
	char* m_lpFileBuffer;
	int m_iFileSize;
	cparserFile* m_parserFile;
private:
	bool GetFileBuffer(const char * lpFilePath);
	int GetFileType();
	void * GetParserFile(int type);
	
};