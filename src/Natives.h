#pragma once
#include "sampgdk/sampgdk.h"
#include "samp-plugin-sdk/amx/amx.h"


#define CHECK_PARAMS(m, n) \
	if (params[0] != (m * 4)) \
			{ \
		logprintf("[chandling] %s: Expecting %d parameter(s), but found %d", n, m, params[0] / 4); \
		return 0; \
		}

#define PAWN_NATIVE(funcname) \
	cell AMX_NATIVE_CALL funcname(AMX* amx, cell* params)

namespace Natives
{
	extern AMX_NATIVE_INFO PluginNatives[];

	// native GetHandlingAttribType(CHandlingAttrib:attribute);
	PAWN_NATIVE(n_GetHandlingAttribType);

	// native IsPlayerUsingCHandling(playerd);
	PAWN_NATIVE(n_IsPlayerUsingCHandling);

	// native ResetModelHandling(modelid);
	PAWN_NATIVE(n_ResetModelHandling);
	// native SetVehicleHandlingFloat(vehicleid, CHandlingAttrib:attribute, Float:value);
	PAWN_NATIVE(n_SetVehicleHandlingFloat);
}