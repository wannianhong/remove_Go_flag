#pragma once
#include "cparserFile.h"

typedef ULONG64 	Elf64_Addr;

typedef ULONG32 	Elf64_Half;

typedef ULONG64 	Elf64_Off;

typedef LONG64 		Elf64_Sword;

typedef ULONG64 	Elf64_Word;

typedef ULONG64 	Elf64_Size;

typedef USHORT 		Elf64_Quarter;

typedef Elf64_Half 	Elf64_Hashelt;

typedef	ULONG64		Elf64_Xword;

/* ELF Header */
#define EI_NIDENT 16
typedef struct
{
	unsigned char    e_ident[EI_NIDENT];    /* Id bytes */
	Elf64_Quarter    e_type;            /* file type */
	Elf64_Quarter    e_machine;        /* machine type */
	Elf64_Half       e_version;        /* version number */
	Elf64_Addr       e_entry;         /* entry point */
	Elf64_Off        e_phoff;         /* Program hdr offset */
	Elf64_Off        e_shoff;         /* Section hdr offset */
	Elf64_Half       e_flags;         /* Processor flags */
	Elf64_Quarter    e_ehsize;        /* sizeof ehdr */
	Elf64_Quarter    e_phentsize;     /* Program header entry size */
	Elf64_Quarter    e_phnum;         /* Number of program headers */
	Elf64_Quarter    e_shentsize;     /* Section header entry size */
	Elf64_Quarter    e_shnum;         /* Number of section headers */
	Elf64_Quarter    e_shstrndx;      /* String table index */
} Elf64_Ehdr;

typedef struct
{
	Elf64_Half    p_type;        /* entry type */
	Elf64_Half    p_flags;    /* flags */
	Elf64_Off    p_offset;    /* offset */
	Elf64_Addr    p_vaddr;    /* virtual address */
	Elf64_Addr    p_paddr;    /* physical address */
	Elf64_Xword    p_filesz;    /* file size */
	Elf64_Xword    p_memsz;    /* memory size */
	Elf64_Xword    p_align;    /* memory & file alignment */
} Elf64_Phdr;

typedef struct
{
	Elf64_Half    sh_name;    /* section name */
	Elf64_Half    sh_type;    /* section type */
	Elf64_Xword   sh_flags;    /* section flags */
	Elf64_Addr    sh_addr;    /* virtual address */
	Elf64_Off     sh_offset;    /* file offset */
	Elf64_Xword   sh_size;    /* section size */
	Elf64_Half    sh_link;    /* link to another */
	Elf64_Half    sh_info;    /* misc info */
	Elf64_Xword   sh_addralign;    /* memory alignment */
	Elf64_Xword   sh_entsize;    /* table entry size */
} Elf64_Shdr;


struct LinuxX64_Section_ST
{
	char section_name[20];

};
class cparserLinuxX64 :public cparserFile
{
public:
	virtual void init();
	virtual int execute(char* lpBuffer, int iSize);
	virtual void uninit();
private:
	bool GetSection();

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
