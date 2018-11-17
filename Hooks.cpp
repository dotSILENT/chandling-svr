#include "Hooks.h"
#include "utils.h"
#include "raknet.h"
#include "sampgdk.h"
#include "subhook/subhook.h"

#ifdef _WIN32
#include <Windows.h>
#else
typedef unsigned long DWORD;
#define NULL 0;
#endif

DWORD FUNC_GetPacketID = NULL;

SubHook shGetPacketID;



typedef BYTE(*GetPacketID_t)(Packet* p);


static BYTE hookedGetPacketID(Packet* p)
{
	SubHook::ScopedRemove remove(&shGetPacketID);

	sampgdk::logprintf("getpacketid called");

	BYTE ret = ((GetPacketID_t)(FUNC_GetPacketID))(p);
	
	sampgdk::logprintf("GetPacketID %d", (int)ret);

	// For some reason subhook returns a null trampoline, so I call the func directly
	return ((GetPacketID_t)(FUNC_GetPacketID))(p);
}

bool InstallHooks()
{
#ifdef _WIN32
	FUNC_GetPacketID = FindPattern("\x8B\x44\x24\x04\x85\xC0\x75\x03\x0C\xFF\xC3", "xxxxxxx???x");
#else
	FUNC_GetPacketID = FindPattern("\x55\xB8\x00\x00\x00\x00\x89\xE5\x8B\x55\x00\x85\xD2", "xx????xxxx?xx");
#endif

	if (FUNC_GetPacketID == 0)
		return false;

	shGetPacketID.Install((void*)FUNC_GetPacketID, (void*)hookedGetPacketID);
	return true;
}