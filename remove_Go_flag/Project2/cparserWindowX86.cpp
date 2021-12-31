#include<Windows.h>
#include<stdio.h>
#include "cparserWindowX86.h"

void cparserWindowX86::init()
{
}

int cparserWindowX86::execute(char* lpBuffer, int iSize)
{
	m_lpPeBuffer= lpBuffer;
	 m_iPeSize= iSize;
	 return GetSection();
	
}

void cparserWindowX86::uninit()
{
}

bool cparserWindowX86::GetSection()
{

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)m_lpPeBuffer;


	m_pNtHeader = (PIMAGE_NT_HEADERS)(m_lpPeBuffer + pDosHeader->e_lfanew);
	m_dwImageBase = m_pNtHeader->OptionalHeader.ImageBase;
	printf("\r\n------------------begin GetRuntime_pclntab-----------------------\r\n");
	if (GetRuntime_pclntab())
	{
		printf("---------------end GetRuntime_pclntab--------------------------\r\n");
		printf("find apilist mem:0x%x,file:0x%x\r\n", m_dwImageBase+m_dwRuntime_pclntab_mem, m_dwRuntime_pclntab_file);
		return ParserGo();
	}
	else
	{
		printf("-----------------end GetRuntime_pclntab------------------------\r\n");
		printf("can'tfind apilist\r\n");
	}

	
	return false;
}
DWORD cparserWindowX86::RvaToRwa(PIMAGE_NT_HEADERS pNtHeader, DWORD Rva)
{
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)IMAGE_FIRST_SECTION(pNtHeader);
	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		//printf("%0x", pSectionHeader[i].VirtualAddress);
		DWORD SectionBeginRva = pSectionHeader[i].VirtualAddress;

		DWORD SectionEndRva = pSectionHeader[i].VirtualAddress + pSectionHeader[i].SizeOfRawData;
		//printf("%0x,%0x", SectionBeginRva, SectionEndRva);
		if (Rva >= SectionBeginRva && Rva <= SectionEndRva)
		{
			DWORD Temp = Rva - SectionBeginRva;
			DWORD Rwa = Temp + pSectionHeader[i].PointerToRawData;
			return Rwa;
		}
	}

}

bool cparserWindowX86::GetRuntime_pclntab()
{
	bool bResult = false;
	//搜索runtime_pclntab 0FFFFFFFBh
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)IMAGE_FIRST_SECTION(m_pNtHeader);
	DWORD dwTextAddr = pSectionHeader->VirtualAddress;
	for (int count = 0; count < m_pNtHeader->FileHeader.NumberOfSections; count++)
	{

		/*printf("\nVirtualSize\t\t%08X\n", pSectionHeader->Misc.VirtualSize);
		printf("VirtualAddress\t\t%08X\n", pSectionHeader->VirtualAddress);
		printf("SizeOfRawData\t\t%08X\n", pSectionHeader->SizeOfRawData);
		printf("PointerToRawData\t%08X\n", pSectionHeader->PointerToRawData);*/
		for (size_t i = 0; i < pSectionHeader->SizeOfRawData; i++)
		{
			char* p = m_lpPeBuffer + pSectionHeader->PointerToRawData+i;
			if (!memcmp(p, "\xFB\xff\xff\xff", 4)||
				!memcmp(p, "\xFA\xff\xff\xff", 4))
			{

				DWORD* pFlag =(DWORD *)( m_dwImageBase + pSectionHeader->VirtualAddress + i);
				printf("mem addr:0x%x\r\n", pFlag);
				pFlag = (DWORD*)( pSectionHeader->PointerToRawData + i);
				printf(" file addr:0x%x\r\n", pFlag);
				//开始判断是否符合要求
				 pFlag = (DWORD*)p;
				pFlag++;
				
				
				if (*pFlag ==0x4010000)
				{
					m_dwRuntime_pclntab_file =(DWORD) p;
					m_dwRuntime_pclntab_mem= (DWORD)(pSectionHeader->VirtualAddress + i);
					m_dwGoVersion = *(DWORD *)p;
					bResult = true;
				}
			}
			
		}
		pSectionHeader++;
	}
	return bResult;
}

struct WindowX86_GO_ITEM
{
	DWORD dwApi;
	DWORD dwValue;
};
bool cparserWindowX86::ParserGo()
{
	//函数符号
	DWORD dwItem=*( (DWORD*)m_dwRuntime_pclntab_file + 2);
	WindowX86_GO_ITEM* dwApiBase = NULL;
	DWORD dwApiST = 0;
	DWORD* pFileIndex = NULL;
	DWORD dwFileNum = 0;
	//判断版本
	if (m_dwGoVersion == 0xFFFFFFFA)
	{
		dwApiBase = (WindowX86_GO_ITEM*)(m_dwRuntime_pclntab_mem + *((DWORD*)m_dwRuntime_pclntab_file + 8));
		m_dwFuncTab = (DWORD)dwApiBase;
		dwApiBase = (WindowX86_GO_ITEM*)(m_lpPeBuffer+ RvaToRwa(m_pNtHeader, m_dwFuncTab));
		m_dwNumFiles = *((DWORD*)m_dwRuntime_pclntab_file + 3);
		m_dwFuncNameTab = m_dwRuntime_pclntab_mem + *((DWORD*)m_dwRuntime_pclntab_file + 4);
		m_dwFileTab = m_dwRuntime_pclntab_mem + *((DWORD*)m_dwRuntime_pclntab_file + 6);
	}
	else if (m_dwGoVersion == 0xFFFFFFFB)
	{
		dwApiBase = (WindowX86_GO_ITEM*)((DWORD*)m_dwRuntime_pclntab_file + 3);
		m_dwFuncNameTab = m_dwRuntime_pclntab_mem;
		m_dwFuncTab = m_dwRuntime_pclntab_mem;
	}
	
	for (size_t i = 0; i < dwItem; i++)
	{
		//现在是内存地址
		DWORD dwApiST = m_dwFuncTab + dwApiBase->dwValue;
		
		//转化成文件地址
		DWORD dwRwaAddr=RvaToRwa(m_pNtHeader, dwApiST);
		WindowX86_GO_ITEM* wApiStr  =(WindowX86_GO_ITEM *)( m_lpPeBuffer + dwRwaAddr);
		dwApiST = m_dwFuncNameTab + wApiStr->dwValue;
		dwRwaAddr = RvaToRwa(m_pNtHeader, dwApiST);
		char* pStr = m_lpPeBuffer + dwRwaAddr;
		int ilen = strlen(pStr);
		printf("name:%s,%x\r\n", pStr, dwApiST+m_dwImageBase);
		memset(pStr, 0, ilen);
		//printf("hello");
		dwApiBase++;
	}
	//文件地址符号
	if (m_dwGoVersion == 0xFFFFFFFA)
	{
		DWORD dwRwaAddr = RvaToRwa(m_pNtHeader, m_dwFileTab);
		pFileIndex = (DWORD*)(m_lpPeBuffer + dwRwaAddr);

		//pFileIndex = (DWORD*)(m_dwFileTab);
		dwFileNum = m_dwNumFiles;
		char* pStr = (char*)pFileIndex;
		for (size_t i = 0; i < dwFileNum; i++)
		{



			int ilen = strlen(pStr);
			printf("%s\r\n", pStr);
			//memset(pStr, 0, ilen);
			pStr += ilen + 1;
		}
	}
	else if (m_dwGoVersion == 0xFFFFFFFB)
	{

		dwApiST = m_dwRuntime_pclntab_mem + dwApiBase->dwValue;
		//转化成文件地址
		DWORD dwRwaAddr = RvaToRwa(m_pNtHeader, dwApiST);
		pFileIndex = (DWORD*)(m_lpPeBuffer + dwRwaAddr);
		dwFileNum = *pFileIndex;
		pFileIndex++;
		for (size_t i = 0; i < dwFileNum - 1; i++)
		{
			dwApiST = m_dwRuntime_pclntab_mem + *pFileIndex;
			dwRwaAddr = RvaToRwa(m_pNtHeader, dwApiST);
			char* pStr = m_lpPeBuffer + dwRwaAddr;
			int ilen = strlen(pStr);
			//printf("%s\r\n", pStr);
			memset(pStr, 0, ilen);
			pFileIndex++;
		}
	}
	return true;
}
