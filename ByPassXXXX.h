#pragma once
#include <Windows.h>
#include <WinBase.h>
class ByPassXXXX
{
public:
	ByPassXXXX(void);
	~ByPassXXXX(void);
	bool PassIt();
private:
	PUCHAR ByPassXXXX::GetStringPosition(PUCHAR start, PUCHAR end, PUCHAR str, int strLen);
};

