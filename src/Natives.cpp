#include "Natives.h"
#include "CPlayer.h"
#include "HandlingEnum.h"
#include "HandlingManager.h"
#include <cstring>
#include "sampgdk/sampgdk.h"

#define CHECK_PARAMS(m, n) \
	if (params[0] != (m * 4)) \
				{ \
		logprintf("[chandling] %s: Expecting %d parameter(s), but found %d", n, m, params[0] / 4); \
		return 0; \
			}

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
		{ "SetVehicleHandlingInt", n_SetVehicleHandlingInt },
		{ "SetModelHandlingFloat", n_SetModelHandlingFloat },
		{ "SetModelHandlingInt", n_SetModelHandlingInt },

		{ "GetVehicleHandlingFloat", n_GetVehicleHandlingFloat },
		{ "GetVehicleHandlingInt", n_GetVehicleHandlingInt },
		{ "GetModelHandlingFloat", n_GetModelHandlingFloat },
		{ "GetModelHandlingInt", n_GetModelHandlingInt },

		{ "GetDefaultHandlingFloat", n_GetDefaultHandlingFloat },
		{ "GetDefaultHandlingInt", n_GetDefaultHandlingInt },
		{ 0, 0 }
	};

	PAWN_NATIVE(n_GetHandlingAttribType)
	{
		CHECK_PARAMS(1, "GetHandlingAttribType")
		return (cell)GetHandlingAttribType((CHandlingAttrib)((int)params[1]));
	}

	PAWN_NATIVE(n_IsPlayerUsingCHandling)
	{
		CHECK_PARAMS(1, "IsPlayerUsingCHandling")

		int playerid = (int)params[1];
		if (IS_VALID_PLAYERID(playerid) || IsPlayerConnected(playerid))
			return gPlayers[playerid].hasCHandling();
		
		return (cell)false;
	}


	PAWN_NATIVE(n_ResetModelHandling)
	{
		CHECK_PARAMS(1, "ResetModelHandling")

		return (cell)HandlingMgr::ResetModelHandling((int)params[1]);
	}

	PAWN_NATIVE(n_ResetVehicleHandling)
	{
		CHECK_PARAMS(1, "ResetVehicleHandling")

		return (cell)HandlingMgr::ResetVehicleHandling((int)params[1]);
	}

	PAWN_NATIVE(n_SetVehicleHandlingFloat)
	{
		CHECK_PARAMS(3, "SetVehicleHandlingFloat")

		return (cell)HandlingMgr::SetVehicleHandling((uint16_t)params[1], (CHandlingAttrib)params[2], amx_ctof(params[3]));
	}

	PAWN_NATIVE(n_SetVehicleHandlingInt)
	{
		CHECK_PARAMS(3, "SetVehicleHandlingInt")

		CHandlingAttrib attrib = (CHandlingAttrib)params[2];

		if (GetHandlingAttribType(attrib) == TYPE_BYTE)
			return (cell)HandlingMgr::SetVehicleHandling((uint16_t)params[1], attrib, (uint8_t)params[3]);

		return (cell)HandlingMgr::SetVehicleHandling((uint16_t)params[1], attrib, (unsigned int)params[3]);
	}

	PAWN_NATIVE(n_SetModelHandlingFloat)
	{
		CHECK_PARAMS(3, "SetModelHandlingFloat")

		return (cell)HandlingMgr::SetModelHandling((uint16_t)params[1], (CHandlingAttrib)params[2], amx_ctof(params[3]));
	}

	PAWN_NATIVE(n_SetModelHandlingInt)
	{
		CHECK_PARAMS(3, "SetModelHandlingInt")

		CHandlingAttrib attrib = (CHandlingAttrib)params[2];

		if (GetHandlingAttribType(attrib) == TYPE_BYTE)
			return (cell)HandlingMgr::SetModelHandling((uint16_t)params[1], attrib, (uint8_t)params[3]);

		return (cell)HandlingMgr::SetModelHandling((uint16_t)params[1], attrib, (unsigned int)params[3]);
	}

	PAWN_NATIVE(n_GetVehicleHandlingFloat)
	{
		CHECK_PARAMS(3, "GetVehicleHandlingFloat")

		float val = 0.0;
		bool ret = HandlingMgr::GetVehicleHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);

		cell* ref = NULL;
		amx_GetAddr(amx, params[3], &ref);
		if (!ref)
			return (cell)false;
		*ref = amx_ftoc(val);
		return (cell)ret;
	}

	PAWN_NATIVE(n_GetVehicleHandlingInt)
	{
		CHECK_PARAMS(3, "GetVehicleHandlingInt")
		
		bool ret = false;
		cell* ref = NULL;
		amx_GetAddr(amx, params[3], &ref);
		if (!ref)
			return (cell)false;

		if (GetHandlingAttribType((CHandlingAttrib)params[2]) == TYPE_BYTE)
		{
			uint8_t val = 0;
			ret = HandlingMgr::GetVehicleHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);
			*ref = (cell)val;
		}
		else
		{
			unsigned int val = 0;
			ret = HandlingMgr::GetVehicleHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);

			*ref = (cell)val;
		}
		return (cell)ret;
	}

	
	PAWN_NATIVE(n_GetModelHandlingFloat)
	{
		CHECK_PARAMS(3, "GetModelHandlingFloat")

		float val = 0.0;
		bool ret = HandlingMgr::GetModelHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);

		cell* ref = NULL;
		amx_GetAddr(amx, params[3], &ref);
		if (!ref)
			return (cell)false;
		*ref = amx_ftoc(val);
		return (cell)ret;
	}

	PAWN_NATIVE(n_GetModelHandlingInt)
	{
		CHECK_PARAMS(3, "GetModelHandlingInt")

		bool ret = false;
		cell* ref = NULL;
		amx_GetAddr(amx, params[3], &ref);
		if (!ref)
			return (cell)false;

		if (GetHandlingAttribType((CHandlingAttrib)params[2]) == TYPE_BYTE)
		{
			uint8_t val = 0;
			ret = HandlingMgr::GetModelHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);
			*ref = (cell)val;
		}
		else
		{
			unsigned int val = 0;
			ret = HandlingMgr::GetModelHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);

			*ref = (cell)val;
		}
		return (cell)ret;
	}

	PAWN_NATIVE(n_GetDefaultHandlingFloat)
	{
		CHECK_PARAMS(3, "GetDefaultHandlingFloat")

		cell* ref = NULL;
		amx_GetAddr(amx, params[3], &ref);
		if (!ref)
			return (cell)false;

		float val = 0.0;
		bool ret = HandlingMgr::GetDefaultHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);

		*ref = amx_ftoc(val);
		return (cell)ret;
	}

	PAWN_NATIVE(n_GetDefaultHandlingInt)
	{
		CHECK_PARAMS(3, "GetDefaultHandlingInt")

		bool ret = false;
		cell* ref = NULL;
		amx_GetAddr(amx, params[3], &ref);
		if (!ref)
			return (cell)false;

		if (GetHandlingAttribType((CHandlingAttrib)params[2]) == TYPE_BYTE)
		{
			uint8_t val = 0;
			ret = HandlingMgr::GetModelHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);
			*ref = (cell)val;
		}
		else
		{
			unsigned int val = 0;
			ret = HandlingMgr::GetModelHandling((uint16_t)params[1], (CHandlingAttrib)params[2], val);
			*ref = (cell)val;
		}
		return (cell)ret;
	}
}
