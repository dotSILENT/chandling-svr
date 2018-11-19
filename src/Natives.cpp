#include "Natives.h"
#include "CPlayer.h"
#include "HandlingEnum.h"
#include "HandlingManager.h"

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


	PAWN_NATIVE(n_GetHandlingAttribType)
	{
		CHECK_PARAMS(1, "GetHandlingAttribType");
		return (cell)GetHandlingAttribType((CHandlingAttrib)((int)params[1]));
	}

	PAWN_NATIVE(n_IsPlayerUsingCHandling)
	{
		CHECK_PARAMS(1, "IsPlayerUsingCHandling");

		int playerid = (int)params[1];
		if (IS_VALID_PLAYERID(playerid) || IsPlayerConnected(playerid))
			return gPlayers[playerid].hasCHandling();
		
		return (cell)false;
	}


	PAWN_NATIVE(n_ResetModelHandling)
	{
		CHECK_PARAMS(1, "ResetModelHandling");

		return (cell)HandlingMgr::ResetModelHandling((int)params[1]);
	}

	PAWN_NATIVE(n_ResetVehicleHandling)
	{
		CHECK_PARAMS(1, "ResetVehicleHandling");

		return (cell)HandlingMgr::ResetVehicleHandling((int)params[1]);
	}

	PAWN_NATIVE(n_SetVehicleHandlingFloat)
	{
		CHECK_PARAMS(3, "SetVehicleHandlingFloat");

		HandlingMgr::OnCreateVehicle((cell)params[1]); // just for testing
		return (cell)HandlingMgr::SetVehicleHandling((int)params[1], (CHandlingAttrib)params[2], amx_ctof(params[3]));
	}
}
