#include<Windows.h>
#include "CscanFileType.h"

int main()
{
	CscanFileType scanFileType;
	scanFileType.init();
	if (scanFileType.Execute("D:\\Project\\go\\ddns\\ddns_ip.exe"))
	{
		scanFileType.Save_File("remove.exe");
	}
	
	return 0;
}