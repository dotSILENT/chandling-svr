#include "sampgdk\sampgdk.h"
#include "samp-plugin-sdk\plugincommon.h"
#include "samp-plugin-sdk\amx\amx.h"
#include "raknet\raknet.h"
#include "Hooks.h"
#include "CPlayer.h"


extern void *pAMXFunctions;
#define CHECK_PARAMS(m, n) \
	if (params[0] != (m * 4)) \
		{ \
		logprintf("[chandling] %s: Expecting %d parameter(s), but found %d", n, m, params[0] / 4); \
		return 0; \
	}

using sampgdk::logprintf;

void **ppPluginData = nullptr;
CCRakServer *pRakServer = nullptr;
bool bGotRakSvr = false;

PLUGIN_EXPORT bool PLUGIN_CALL OnIncomingConnection(int playerid, const char* ip_addr, int port)
{
	if (IS_VALID_PLAYERID(playerid))
		gPlayers[playerid].Reset();
	return false;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) {
	logprintf("[chandling] OnPlayerConnect");
	return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnVehicleStreamIn(int vehicleid, int forplayerid)
{
	logprintf("[chandling] OnVehicleStreamIn(%d,%d)", vehicleid, forplayerid);

	if (IS_VALID_PLAYERID(forplayerid))
	{
		// Send handling modifications for this vehicle

	}
	return true;
}



PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return sampgdk::Supports() | SUPPORTS_PROCESS_TICK | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {

	ppPluginData = ppData;
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	return sampgdk::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	sampgdk::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	sampgdk::ProcessTick();
}


// forward SetVehicleHandlingFloat(vehicleid, E_HANDLING:type, Float:value);
cell AMX_NATIVE_CALL n_SetVehicleHandlingFloat(AMX* amx, cell* params)
{
	CHECK_PARAMS(3, "SetVehicleHandlingFloat");
	logprintf("SetVehicleHandling()");


	return 1;
}

AMX_NATIVE_INFO PluginNatives[] =
{
	{ "SetVehicleHandlingFloat", n_SetVehicleHandlingFloat },
	{ 0, 0 }
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	if (!bGotRakSvr)
	{
		// Get pRakServer, taken from SKY
		int(*pfn_GetRakServer)(void) = (int(*)(void))ppPluginData[PLUGIN_DATA_RAKSERVER];
		pRakServer = (CCRakServer*)pfn_GetRakServer();

		if (!pRakServer)
			logprintf("[chandling] pRakServer is null");
		else
			logprintf("[chandling] pRakServer 0x%x", (int)pRakServer);
		bGotRakSvr = true;

		InstallHooks();
	}
	return amx_Register(amx, PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
	return AMX_ERR_NONE;
}