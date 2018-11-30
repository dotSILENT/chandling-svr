#include "chandlingsvr.h"

#include "sampgdk/sampgdk.h"
#include "samp-plugin-sdk/plugincommon.h"
#include "samp-plugin-sdk/amx/amx.h"
#include "Hooks.h"
#include "CPlayer.h"
#include "Natives.h"
#include "HandlingManager.h"
#include "HandlingDefault.h"
#include "HandlingEnum.h"

extern void *pAMXFunctions;
void **ppPluginData = nullptr;

using sampgdk::logprintf;

CCRakServer *pRakServer = nullptr;

bool bInitialized = false;
bool bGotRakSvr = false;

PLUGIN_EXPORT bool PLUGIN_CALL OnIncomingConnection(int playerid, const char* ip_addr, int port)
{
	if (IS_VALID_PLAYERID(playerid))
		gPlayers[playerid].Reset();
	return false;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) {
	//logprintf("[chandling] OnPlayerConnect");
	if (bInitialized)
		HandlingMgr::OnPlayerConnect(playerid);
	return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnVehicleStreamIn(int vehicleid, int forplayerid)
{
	//logprintf("[chandling] OnVehicleStreamIn(%d,%d)", vehicleid, forplayerid);

	if (bInitialized)
	{
		// Send handling modifications for this vehicle
		HandlingMgr::OnVehicleStreamIn(vehicleid, forplayerid);
	}
	return true;
}



PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return sampgdk::Supports() | SUPPORTS_PROCESS_TICK | SUPPORTS_AMX_NATIVES;
}


PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {

	ppPluginData = ppData;
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	bool ret = sampgdk::Load(ppData);
	Hooks::InstallNativeRedirects(pAMXFunctions);
	return ret;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	Hooks::UninstallHooks();
	sampgdk::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	if (bInitialized)
		HandlingMgr::ProcessTick();
	sampgdk::ProcessTick();
}


PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	if (!bInitialized)
	{
		// Get pRakServer, taken from SKY
		int(*pfn_GetRakServer)(void) = (int(*)(void))ppPluginData[PLUGIN_DATA_RAKSERVER];
		pRakServer = (CCRakServer*)pfn_GetRakServer();

		if (!pRakServer)
			logprintf("[chandling] [ERROR] pRakServer is null, plugin couldn't be initialized");
		else
		{
			bInitialized = Hooks::InstallHooks();

			if (!bInitialized)
				logprintf("[chandling] ERROR: Plugin couldn't be initialized");
			else
			{
				HandlingDefault::Initialize();
				HandlingMgr::InitializeModelHandlings();
			}
		}
	}

	return amx_Register(amx, Natives::PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
	return AMX_ERR_NONE;
}