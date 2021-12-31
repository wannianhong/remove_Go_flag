#pragma once
class cparserFile
{
public:
	virtual void init() = 0;
	virtual int execute(char* lpBuffer, int iSize) = 0;
	virtual void uninit() = 0;
};