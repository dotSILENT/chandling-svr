/*
* GetPacketID hook code together with FindPattern for windows & linux was taken from Whitetiger's ACV2
* https://github.com/Whitetigerswt/SAMP_AC_v2
*
*/
#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#else
typedef char BYTE;
#define NULL 0
#endif

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
{
	for (; *mask; ++mask, ++data, ++pattern)
	{
		if (*mask == 'x' && *data != *pattern)
			return false;
	}
	return (*mask) == NULL;
}

unsigned long FindPattern(char *pattern, char *mask)
{
	unsigned long i;
	unsigned long size;
	unsigned long address;
#ifdef _WIN32
	MODULEINFO info = { 0 };

	address = (unsigned long)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (unsigned long)info.SizeOfImage;
#else
	address = 0x804b480; // around the elf base
	size = 0x8128B80 - address;
#endif
	for (i = 0; i < size; ++i)
	{
		if (memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
			return (unsigned long)(address + i);
	}
	return 0;
}