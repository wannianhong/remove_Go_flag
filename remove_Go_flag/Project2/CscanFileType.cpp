#define _CRT_SECURE_NO_WARNINGS

#include "CscanFileType.h"
#include "cparserWindowX86.h"
#include "cparserLinuxX86.h"
#include "cparserLinuxX64.h"

int CscanFileType::init()
{
	m_map_parserFile[FILE_TYPE_WINDOWS_X86] = new cparserWindowX86();
	m_map_parserFile[FILE_TYPE_LINUX_X86] = new cparserLinuxX86();
	m_map_parserFile[FILE_TYPE_LINUX_X64] = new cparserLinuxX64();
	return 0;
}

int CscanFileType::Execute(const char* lpFilePath)
{
	int type = 0;
	if (GetFileBuffer(lpFilePath))
	{
		type = GetFileType();
		m_parserFile =(cparserFile * )GetParserFile(type);
		if (!m_parserFile)
		{
			return -2;
		}
		m_parserFile->init();
	return m_parserFile->execute(m_lpFileBuffer, m_iFileSize);
	 
	}
	return -1;
}

int CscanFileType::uninit()
{
	m_parserFile->uninit();
	return 0;
}

bool CscanFileType::GetFileBuffer(const char* lpFilePath)
{
	FILE* fp = NULL;


	fp = fopen(lpFilePath, "rb");

	if (NULL == fp)
	{
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	m_iFileSize = ftell(fp);
	m_lpFileBuffer = (char*)malloc(m_iFileSize);
	if (!m_lpFileBuffer)
	{
		return 0;
	}
	fseek(fp, 0, SEEK_SET);
	fread(m_lpFileBuffer, m_iFileSize, 1, fp);

	fclose(fp);
	return 1;

}

int CscanFileType::GetFileType()
{
	//判断是否是windows
	IMAGE_DOS_HEADER* pDOS_Header = (IMAGE_DOS_HEADER*)m_lpFileBuffer;
	if (pDOS_Header->e_magic== IMAGE_DOS_SIGNATURE)
	{
		//判断是32位数还是64位
		IMAGE_NT_HEADERS* pNT_Header = (IMAGE_NT_HEADERS*)(m_lpFileBuffer +
			pDOS_Header->e_lfanew);
		int wOs = pNT_Header->OptionalHeader.Magic;
		if (wOs== 0x20B)
		{
			return FILE_TYPE_WINDOWS_X64;
		}
		else if (wOs == 0x10B)
		{
			return FILE_TYPE_WINDOWS_X86;
		}
		else
		{
			return FILE_TYPE_UNKNOW;
		}

	}
	//判断是否是linux
	// 判断elf文件类型
	if (m_lpFileBuffer[0] == 0x7F &&
		m_lpFileBuffer[1] == 'E' &&
		m_lpFileBuffer[2] == 'L' &&
		m_lpFileBuffer[3] == 'F')
	{
		//判断是32位数还是64位
		if (m_lpFileBuffer[4]==0x1)
		{
			return FILE_TYPE_LINUX_X86;
		}
		else
		{
			return FILE_TYPE_LINUX_X64;
		}
		
	}

	return false;
}
void CscanFileType::Save_File(const char* lpFilePath)
{
	FILE* fp = NULL;


	fp = fopen(lpFilePath, "wb");

	if (NULL == fp)
	{
		return ;
	}

	fwrite(m_lpFileBuffer, m_iFileSize, 1, fp);

	fclose(fp);
}
void * CscanFileType::GetParserFile(int type)
{
	map<int, void*>::iterator it;
	it=m_map_parserFile.find(type);
	if (it!= m_map_parserFile.end())
	{
		return it->second;
	}
	return 0;
}
