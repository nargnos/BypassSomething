#include "ByPassXXXX.h"


ByPassXXXX::ByPassXXXX(void)
{
}

PUCHAR ByPassXXXX::GetStringPosition(PUCHAR start, PUCHAR end, PUCHAR str, int strLen)
{
	// 准备跳转表
	int* jmpTable = new int[256];
	for(int i = 0; i < 256; i++)
	{
		jmpTable[i] = strLen+1;
	}
	for(int k = 0; k < strLen; k++)
	{
		jmpTable[str[k]] = strLen- k;
	}
	// 大小写不想关的话就修改一下生成table方法
	bool isFound = false;
	PUCHAR pos = start;
	char tempChar = 0;
	while(pos< (end - strLen + 1))
	{
		int j = 0;
		try
		{
			for(; j < strLen; j++)
			{

				if(pos[j] != str[j])
				{
					tempChar = pos[strLen];
					pos += jmpTable[pos[strLen]];
					break;
				}

			}
		}
		catch(...)
		{
			// 高位加1
			pos = (PUCHAR)((DWORD)pos>>16);
			pos++;
			pos = (PUCHAR)((DWORD)pos<<16);
			//break;
		}
		if(j == strLen)
		{
			isFound = true;
			break;
		}	
	}

	delete jmpTable;
	if(isFound)
	{
		return pos;
	}
	return NULL;
}
bool ByPassXXXX::PassIt()
{
	DWORD nowBase = (DWORD)GetModuleHandleA("XXXXXXXX.dll");
	
	if(nowBase)
	{
		
		DWORD tempBase = nowBase;
		// 找位置 8A 45 B7 8B 4D F0 64 89 0D 00 00 00 00 59 5F 5E 5B 8B 4D E4 33 CD
		// 找到+1b就到修改位置
		UCHAR findChars[] = {"\x8A\x45\xB7\x8B\x4D\xF0\x64\x89\x0D\x00\x00\x00\x00\x59\x5F\x5E\x5B\x8B\x4D\xE4\x33\xCD"};
		
		// 判断返回值地点
		UCHAR memoryBin[] = {"\x56\x8D\x8B\x44\x71\x02\x00"};

		auto foundIt = (DWORD)GetStringPosition((PUCHAR)tempBase,(PUCHAR)(tempBase+0x100000),findChars,22);

		DWORD foundIt2 = (DWORD)GetStringPosition((PUCHAR)nowBase,(PUCHAR)(nowBase+0x100000),memoryBin,7);

		if(foundIt && foundIt2)
		{
			foundIt2+=0xc;// 偏移到位置

			DWORD position = foundIt + 0x1b;
			PCHAR code = new CHAR[32];

			DWORD oldProtectFlag = 0;
			DWORD arrayProtectFlag = 0;

			if(VirtualProtect((PCHAR)position,6,PAGE_EXECUTE_READWRITE,&oldProtectFlag) && 
				VirtualProtect(code,32,PAGE_EXECUTE_READWRITE,&arrayProtectFlag))
			{	
				// 计算jmp
				DWORD jmpAddress = ((DWORD)code) - 5 - position;
				// 写入
				PUCHAR tempPositionPoint = (PUCHAR)position;

				tempPositionPoint[0] = 0xe9;
				memcpy(tempPositionPoint+1,&jmpAddress,4);
				tempPositionPoint[5] = 0xcc;// 非必要
				// 恢复
				VirtualProtect((PCHAR)position,6,oldProtectFlag,&arrayProtectFlag);

				// 判断栈顶是否为判断返回值为1的那个,如果是就把返回值改为1
				UCHAR codeWrite[] ={"\x8B\xE5\x5D\x81\x3c\x24\xDE\xAD\xBE\xEF\x75\x03\x33\xc0\x40\xc2\x04\x00\xcc"};
				memcpy(codeWrite+6,&foundIt2,4);
				memcpy(code,codeWrite,19);
				
				return true;
			}	
		}
	}
	return false;
}

ByPassXXXX::~ByPassXXXX(void)
{
}
