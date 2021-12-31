#pragma once
#include "cparserFile.h"

struct WindowX86_Section_ST
{
	char section_name[20];

};
class cparserWindowX86:public cparserFile
{
public:
	virtual void init() ;
	virtual int execute(char* lpBuffer, int iSize);
	virtual void uninit();
private:
	bool GetSection();
	//虚拟地址转化为文件地址
	DWORD RvaToRwa(PIMAGE_NT_HEADERS pNtHeader, DWORD Rva);
	//解析go符号
	bool GetRuntime_pclntab();
	bool ParserGo();
private:
	char* m_lpPeBuffer;
	int m_iPeSize;
	DWORD m_dwGoVersion;
	DWORD m_dwFuncNameTab;
	DWORD m_dwFuncTab;
	DWORD m_dwNumFiles;
	DWORD m_dwFileTab;
	DWORD m_dwImageBase;
	DWORD m_dwRuntime_pclntab_file;
	DWORD m_dwRuntime_pclntab_mem;
	PIMAGE_NT_HEADERS m_pNtHeader;
};