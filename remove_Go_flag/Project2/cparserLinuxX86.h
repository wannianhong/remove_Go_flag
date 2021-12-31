#pragma once
#include "cparserFile.h"

typedef unsigned int            uint32_t;
typedef unsigned short int      uint16_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef long long int           int64_t;
typedef unsigned long long int  uint64_t;

#define Elf32_Addr        uint32_t
#define Elf32_Off       uint32_t
#define Elf32_Section    uint16_t
#define Elf32_Versym     uint16_t
#define Elf_Byte        unsigned char
#define Elf32_Half       uint16_t
#define Elf32_Sword      int32_t
#define Elf32_Word       uint32_t
#define Elf32_Sxword     int64_t
#define Elf32_Xword      uint64_t

/* ELF Header */
#define EI_NIDENT 16
typedef struct elfhdr
{
	unsigned char    e_ident[EI_NIDENT]; /* ELF Identification */
	Elf32_Half    e_type;        /* object file type */
	Elf32_Half    e_machine;    /* machine */
	Elf32_Word    e_version;    /* object file version */
	Elf32_Addr    e_entry;      /* virtual entry point */
	Elf32_Off     e_phoff;      /* program header table offset */
	Elf32_Off     e_shoff;      /* section header table offset */
	Elf32_Word    e_flags;      /* processor-specific flags */
	Elf32_Half    e_ehsize;     /* ELF header size */
	Elf32_Half    e_phentsize;    /* program header entry size */
	Elf32_Half    e_phnum;      /* number of program header entries */
	Elf32_Half    e_shentsize;    /* section header entry size */
	Elf32_Half    e_shnum;      /* number of section header entries */
	Elf32_Half    e_shstrndx;    /* section header table's "section
					   header string table" entry offset */
} Elf32_Ehdr;
typedef struct elf32_phdr
{
	Elf32_Word    p_type;  //段的类型，LOAD，DYNAMIC等
	Elf32_Off    p_offset;  //段在文件中的偏移量
	Elf32_Addr    p_vaddr;  //段的物理地址
	Elf32_Addr    p_paddr;  //段的虚拟地址
	Elf32_Word    p_filesz;  //段在文件中的大小
	Elf32_Word    p_memsz;  //段在内存中的大小
	Elf32_Word    p_flags;  //读写执行标记
	Elf32_Word    p_align;  //段的对齐
} Elf32_Phdr;
typedef struct elf32_shdr
{
	Elf32_Word    sh_name;  //节的名字，在符号表中的下标
	Elf32_Word    sh_type;  //节的类型，描述符号，代码，数据，重定位等
	Elf32_Word    sh_flags;  //读写执行标记
	Elf32_Addr    sh_addr;  //节在执行时的虚拟地址
	Elf32_Off    sh_offset;  //节在文件中的偏移量
	Elf32_Word    sh_size;  //节的大小
	Elf32_Word    sh_link;  //其它节的索引
	Elf32_Word    sh_info;  //节的其它信息
	Elf32_Word    sh_addralign;  //节对齐
	Elf32_Word    sh_entsize;  //节拥有固定大小项的大小
} Elf32_Shdr;

struct LinuxX86_Section_ST
{
	char section_name[20];

};
class cparserLinuxX86 :public cparserFile
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
