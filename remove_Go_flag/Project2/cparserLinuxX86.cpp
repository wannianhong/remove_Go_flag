#include<Windows.h>
#include<stdio.h>
#include "cparserLinuxX86.h"

void cparserLinuxX86::init()
{
}

int cparserLinuxX86::execute(char* lpBuffer, int iSize)
{
	m_lpPeBuffer = lpBuffer;
	m_iPeSize = iSize;
	return GetSection();

}

void cparserLinuxX86::uninit()
{
}

bool cparserLinuxX86::GetSection()
{
	Elf32_Shdr* pShdr = NULL;
	Elf32_Phdr* pPhdr = NULL;
	bool bFind = false;
	 Elf32_Ehdr* pElf_header = ( Elf32_Ehdr*)m_lpPeBuffer;
	pShdr =(Elf32_Shdr *)( m_lpPeBuffer + pElf_header->e_shoff);
	pPhdr = (Elf32_Phdr*)(m_lpPeBuffer + pElf_header->e_phoff);

	char* shstrtab = m_lpPeBuffer+pShdr[pElf_header->e_shstrndx].sh_offset;
	// 遍历
	for (int i = 0; i < pElf_header->e_shnum; i++)
	{
		char* temp = shstrtab;
		temp = temp + pShdr[i].sh_name;
		if (!_strnicmp(temp,".gopclntab",10))
		{
			m_dwRuntime_pclntab_file =(DWORD)( m_lpPeBuffer+ pShdr[i].sh_offset);
			m_dwGoVersion = *((DWORD*)m_dwRuntime_pclntab_file);
			bFind = true;
		}
		/*printf("节的名称: %s\n", temp);
		printf("节首的偏移: %x\n", pShdr[i].sh_offset);
		printf("节的大小: %x\n", pShdr[i].sh_size);
		printf("节尾的地址: %x\n", pShdr[i].sh_offset + pShdr[i].sh_size);
		printf("\n");*/
	}
	
	printf("\r\n------------------begin GetRuntime_pclntab-----------------------\r\n");
	if (bFind)
	{
	printf("find apilist file:0x%x\r\n", m_dwRuntime_pclntab_file);
		return ParserGo();
	}
	else
	{
		
		printf("can'tfind apilist\r\n");
	}
printf("-----------------end GetRuntime_pclntab------------------------\r\n");
	//加载
	//char* phstrtab = m_lpPeBuffer + pPhdr[pElf_header->e_shentsize].p_offset;

	//for (int i = 0; i < pElf_header->e_phnum; i++)
	//{
	//	if (pPhdr[i].p_type==1)//pt_load
	//	{
	//		printf("imagebase=%x\r\n", pPhdr[i].p_vaddr);
	//	}
	//	/*printf("%d: \n", i);
	//	printf(" 该段首相对偏移: %x \n", pPhdr[i].p_offset);
	//	printf(" 该段的大小: %x \n", pPhdr[i].p_memsz);
	//	printf(" 该段尾相对偏移: %x \n", pPhdr[i].p_memsz + pPhdr[i].p_offset);
	//	
	//	printf("\n");*/
	//}

	//m_pNtHeader = (PIMAGE_NT_HEADERS)(m_lpPeBuffer + pDosHeader->e_lfanew);
	//m_dwImageBase = m_pNtHeader->OptionalHeader.ImageBase;
	//printf("\r\n------------------begin GetRuntime_pclntab-----------------------\r\n");
	//if (GetRuntime_pclntab())
	//{
	//	printf("---------------end GetRuntime_pclntab--------------------------\r\n");
	//	printf("find apilist mem:0x%x,file:0x%x\r\n", m_dwImageBase + m_dwRuntime_pclntab_mem, m_dwRuntime_pclntab_file);
	//	return ParserGo();
	//}
	//else
	//{
	//	printf("-----------------end GetRuntime_pclntab------------------------\r\n");
	//	printf("can'tfind apilist\r\n");
	//}


	return false;
}



struct LinuxX86_GO_ITEM
{
	DWORD dwApi;
	DWORD dwValue;
};
bool cparserLinuxX86::ParserGo()
{
	//函数符号
	DWORD dwItem = *((DWORD*)m_dwRuntime_pclntab_file + 2);
	LinuxX86_GO_ITEM* dwApiBase = NULL;
	DWORD dwApiST = 0;
	DWORD* pFileIndex = NULL;
	DWORD dwFileNum = 0;
	//判断版本
	if (m_dwGoVersion==0xFFFFFFFA)
	{
		dwApiBase = (LinuxX86_GO_ITEM*)(m_dwRuntime_pclntab_file+*((DWORD*)m_dwRuntime_pclntab_file + 8));
		m_dwFuncTab =(DWORD)dwApiBase;
		m_dwNumFiles =  *((DWORD*)m_dwRuntime_pclntab_file + 3);
		m_dwFuncNameTab= m_dwRuntime_pclntab_file + *((DWORD*)m_dwRuntime_pclntab_file+4);
		m_dwFileTab= m_dwRuntime_pclntab_file + *((DWORD*)m_dwRuntime_pclntab_file + 6);
	}
	else if (m_dwGoVersion == 0xFFFFFFFB)
	{
		dwApiBase = (LinuxX86_GO_ITEM*)((DWORD*)m_dwRuntime_pclntab_file + 3);
		m_dwFuncNameTab = m_dwRuntime_pclntab_file;
		m_dwFuncTab = m_dwRuntime_pclntab_file;
	}

	
	for (size_t i = 0; i < dwItem; i++)
	{
		//转化成文件地址
		 dwApiST = m_dwFuncTab + dwApiBase->dwValue;

		LinuxX86_GO_ITEM* wApiStr = (LinuxX86_GO_ITEM*)(dwApiST);
		
		char* pStr =(char*)(m_dwFuncNameTab + wApiStr->dwValue);
		int ilen = strlen(pStr);
		//printf("name:%s\r\n", pStr);
		memset(pStr, 0, ilen);
		
		dwApiBase++;
	}
	//文件地址符号
	if (m_dwGoVersion == 0xFFFFFFFA)
	{
		pFileIndex = (DWORD*)(m_dwFileTab);
		dwFileNum = m_dwNumFiles;
		char* pStr = (char*)pFileIndex;
		for (size_t i = 0; i < dwFileNum; i++)
		{
			

			
			int ilen = strlen(pStr);
			//printf("%s\r\n", pStr);
			memset(pStr, 0, ilen);
			pStr += ilen+1;
		}
	}
	else if (m_dwGoVersion == 0xFFFFFFFB)
	{
		dwApiST = m_dwRuntime_pclntab_file + dwApiBase->dwValue;
		pFileIndex = (DWORD*)(dwApiST);
		dwFileNum = *pFileIndex;
		pFileIndex++;
		for (size_t i = 0; i < dwFileNum - 1; i++)
		{
			dwApiST = m_dwRuntime_pclntab_file + *pFileIndex;

			char* pStr = (char*)dwApiST;
			int ilen = strlen(pStr);
			//printf("%s\r\n", pStr);
			memset(pStr, 0, ilen);
			pFileIndex++;
		}
	}
	 
	//转化成文件地址

	
	
	return true;
}
