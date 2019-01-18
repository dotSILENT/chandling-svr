#include "Hooks.h"
#include "utils.h"
#include "sampgdk/sampgdk.h"
#include "subhook/subhook.h"
#include "raknet/raknet.h"
#include "PacketEnum.h"
#include "Actions.h"
#include "Natives.h"
#include <cstdint>

#ifndef _WIN32
typedef uint8_t BYTE;
typedef unsigned long DWORD;
#define __cdecl __attribute__((cdecl))
#endif

DWORD FUNC_GetPacketID = 0; //NULL;

/*
* amx_Register hook was taken from YSF
* https://github.com/IllidanS4/YSF/
*/
subhook_t amx_Register_hook;

/*
 * GetPacketID hook code together with FindPattern for windows & linux was taken from Whitetiger's ACV2
 * https://github.com/Whitetigerswt/SAMP_AC_v2
 *
*/
subhook::Hook shGetPacketID;
typedef BYTE(__cdecl *GetPacketID_t)(Packet* p);

BYTE __cdecl hookedGetPacketID(Packet* p)
{
	subhook::ScopedHookRemove remove(&shGetPacketID);

	BYTE ret = ((GetPacketID_t)(FUNC_GetPacketID))(p);
	if (!p || !p->data || !p->length)
		return ret;

	BYTE id = ret;
	
	//sampgdk::logprintf("GetPacketID %d length %d", id, p->length);

	if (id == (BYTE)ID_CHANDLING)
	{
		if (p->length > sizeof(BYTE) * 2)
		{
			RakNet::BitStream bs(&p->data[2], p->length - 2, false);
			Actions::Process((CHandlingAction)p->data[1], &bs, p->playerIndex);
		}
		return 0xFF;
	}

	// Subhook returns a null trampoline so I just call the func directly
	return ret;
}

/* Code taken from YSF: https://github.com/IllidanS4/YSF/ */
typedef int(AMXAPI *t_amx_Register)(AMX *amx, AMX_NATIVE_INFO *nativelist, int number);
int AMXAPI HOOK_amx_Register(AMX *amx, AMX_NATIVE_INFO *nativelist, int number)
{
	// amx_Register hook for redirect natives
	static bool bNativesHooked = false;

	if (!bNativesHooked)
	{
		for (int i = 0; nativelist[i].name; i++)
		{
			if (ApplyNativeRedirect(nativelist[i]))
			{
				bNativesHooked = true;
				break;
			}

			if (i == number - 1) break;
		}
	}

	return ((t_amx_Register)subhook_get_trampoline(amx_Register_hook))(amx, nativelist, number);
}


namespace Hooks
{ 
	/* This needs to be called from Load before InstallHooks is called */
	void PreHooking()
	{
		/*
			We do it this way, so GetPacketID's pattern isn't changed right after loading the plugin
			That would make it impossible for other plugins to use FindPattern again, because a JMP instruction is placed at the first 5 bytes
			This way we find the function right after the plugin is loaded (Load()) and hook it when the gamemode is being loaded (AmxLoad())
		*/
#ifdef _WIN32
		FUNC_GetPacketID = FindPattern("\x8B\x44\x24\x04\x85\xC0\x75\x03\x0C\xFF\xC3", "xxxxxxx???x");
#else
		FUNC_GetPacketID = FindPattern("\x55\xB8\x00\x00\x00\x00\x89\xE5\x8B\x55\x00\x85\xD2", "xx????xxxx?xx");
#endif
	}

	/* this needs to be called from AmxLoad when rakserver already exists */
	bool InstallHooks()
	{
		if (FUNC_GetPacketID == 0)
			return false;

		shGetPacketID.Install((void*)FUNC_GetPacketID, (void*)hookedGetPacketID);
		return true;
	}

	/* This needs to be called before netgame is initialized and before sampgdk */
	void InstallNativeRedirects(void* pAMXFunctions)
	{
		amx_Register_hook = subhook_new(reinterpret_cast<void*>(*(DWORD*)((DWORD)pAMXFunctions + (PLUGIN_AMX_EXPORT_Register * 4))), reinterpret_cast<void*>(HOOK_amx_Register), static_cast<subhook_flags_t>(NULL));
		subhook_install(amx_Register_hook);
	}

	void UninstallHooks()
	{
		subhook_remove(amx_Register_hook);
		shGetPacketID.Remove();
	}
}