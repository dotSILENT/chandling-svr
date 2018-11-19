#include "Natives.h"
#include "CPlayer.h"
#include "HandlingEnum.h"

using sampgdk::logprintf;

namespace Natives
{
	AMX_NATIVE_INFO PluginNatives[] =
	{
		{ "GetHandlingAttribType", n_GetHandlingAttribType },
		{ "IsPlayerUsingCHandling", n_IsPlayerUsingCHandling },
		{ "SetVehicleHandlingFloat", n_SetVehicleHandlingFloat },
		{ 0, 0 }
	};


	cell AMX_NATIVE_CALL n_GetHandlingAttribType(AMX* amx, cell* params)
	{
		CHECK_PARAMS(1, "GetHandlingAttribType");
		return GetHandlingAttribType((CHandlingAttrib)((int)params[1]));
	}

	cell AMX_NATIVE_CALL n_IsPlayerUsingCHandling(AMX* amx, cell* params)
	{
		CHECK_PARAMS(1, "IsPlayerUsingCHandling");

		int playerid = (int)params[1];
		if (IS_VALID_PLAYERID(playerid) || IsPlayerConnected(playerid))
			return gPlayers[playerid].hasCHandling();
		
		return false;
	}


	cell AMX_NATIVE_CALL n_SetVehicleHandlingFloat(AMX* amx, cell* params)
	{
		CHECK_PARAMS(3, "SetVehicleHandlingFloat");
		logprintf("SetVehicleHandling()");


		return 1;
	}
}
