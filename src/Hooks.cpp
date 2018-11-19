#include "Hooks.h"
#include "utils.h"
#include "sampgdk/sampgdk.h"
#include "subhook/subhook.h"
#include "raknet/raknet.h"
#include "PacketEnum.h"
#include "Actions.h"
#include <cstdint>


DWORD FUNC_GetPacketID = NULL;

SubHook shGetPacketID;

/*
 * GetPacketID hook code together with FindPattern for windows & linux was taken from Whitetiger's ACV2
 * https://github.com/Whitetigerswt/SAMP_AC_v2
 *
*/

typedef BYTE(__cdecl *GetPacketID_t)(Packet* p);

BYTE __cdecl hookedGetPacketID(Packet* p)
{
	SubHook::ScopedRemove remove(&shGetPacketID);

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