#include "sampgdk.h"
#include "plugincommon.h"
#include "raknet.h"
#include "BitStream.h"
#include "Hooks.h"

using sampgdk::logprintf;

void **ppPluginData = nullptr;
CCRakServer *pRakServer = nullptr;
bool bGotRakSvr = false;

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) {
	logprintf("OnPlayerConnect");
	return true;
}


PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return sampgdk::Supports() | SUPPORTS_PROCESS_TICK | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {

	ppPluginData = ppData;

	return sampgdk::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	sampgdk::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	sampgdk::ProcessTick();
}


AMX_NATIVE_INFO PluginNatives[] =
{
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
	return 1;// amx_Register(amx, PluginNatives, -1);
}