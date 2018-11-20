#include "Natives.h"
#include "CPlayer.h"
#include "HandlingEnum.h"
#include "HandlingManager.h"
#include <cstring>

using sampgdk::logprintf;

/*
 * AMX Native hooking code was taken from YSF
 * https://github.com/IllidanS4/YSF/
 */
namespace Original
{
	AMX_NATIVE CreateVehicle;
	AMX_NATIVE AddStaticVehicle;
	AMX_NATIVE AddStaticVehicleEx;
}

namespace Hook
{
	/* These hooks are required so newly created vehicles have their handling data reset */
	PAWN_NATIVE(CreateVehicle)
	{
		//logprintf("[chandling] Hooked CreateVehicle");
		int vehicleid = hookedNatives[0].originalFunc(amx, params);

		HandlingMgr::OnCreateVehicle(vehicleid);
		return vehicleid;
	}

	PAWN_NATIVE(AddStaticVehicle)
	{
		//logprintf("[chandling] Hooked AddStaticVehicle");
		int vehicleid = hookedNatives[0].originalFunc(amx, params);

		HandlingMgr::OnCreateVehicle(vehicleid);
		return vehicleid;
	}

	PAWN_NATIVE(AddStaticVehicleEx)
	{
		//logprintf("[chandling] Hooked AddStaticVehicleEx");
		int vehicleid = hookedNatives[0].originalFunc(amx, params);

		HandlingMgr::OnCreateVehicle(vehicleid);
		return vehicleid;
	}
}

AMX_HOOK_INFO hookedNatives[] =
{
	AMX_DEFINE_HOOK(CreateVehicle),
	AMX_DEFINE_HOOK(AddStaticVehicle),
	AMX_DEFINE_HOOK(AddStaticVehicleEx)
};

#define NATIVE_HOOK_COUNT (3)

bool ApplyNativeRedirect(AMX_NATIVE_INFO &native)
{
	static int hookedCount = 0;

	for (int i = 0; i < NATIVE_HOOK_COUNT; i++)
	{
		if (!strcmp(native.name, hookedNatives[i].name))
		{
			hookedCount++;

			hookedNatives[i].originalFunc = native.func;
			native.func = hookedNatives[i].func;

			if (hookedCount == NATIVE_HOOK_COUNT)
				return true;
		}
	}
	return false;
}

namespace Natives
{
	AMX_NATIVE_INFO PluginNatives[] =
	{
		{ "GetHandlingAttribType", n_GetHandlingAttribType },
		{ "IsPlayerUsingCHandling", n_IsPlayerUsingCHandling },
		{ "ResetModelHandling", n_ResetModelHandling },
		{ "ResetVehicleHandling", n_ResetVehicleHandling },
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

		return (cell)HandlingMgr::SetVehicleHandling((int)params[1], (CHandlingAttrib)params[2], amx_ctof(params[3]));
	}
}
