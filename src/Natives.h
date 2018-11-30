#pragma once
#include "sampgdk/sampgdk.h"
//#include "samp-plugin-sdk/amx/amx.h"


#define PAWN_NATIVE(funcname) \
	cell AMX_NATIVE_CALL funcname(AMX* amx, cell* params)

/* Taken from YSF: https://github.com/IllidanS4/YSF/ */
struct AMX_HOOK_INFO
{
	const char _FAR *name PACKED;
	AMX_NATIVE &originalFunc PACKED;
	AMX_NATIVE func PACKED;
};

#define AMX_DEFINE_HOOK(name) \
	{#name, Original::name, Hook::name }

extern AMX_HOOK_INFO hookedNatives[];


bool ApplyNativeRedirect(AMX_NATIVE_INFO &native);

namespace Natives
{
	extern AMX_NATIVE_INFO PluginNatives[];

	// native GetHandlingAttribType(CHandlingAttrib:attribute);
	PAWN_NATIVE(n_GetHandlingAttribType);

	// native IsPlayerUsingCHandling(playerd);
	PAWN_NATIVE(n_IsPlayerUsingCHandling);

	// native ResetModelHandling(modelid);
	PAWN_NATIVE(n_ResetModelHandling);
	// native ResetVehicleHandling(vehicleid);
	PAWN_NATIVE(n_ResetVehicleHandling);
	// native SetVehicleHandlingFloat(vehicleid, CHandlingAttrib:attribute, Float:value);
	PAWN_NATIVE(n_SetVehicleHandlingFloat);
	// native SetVehicleHandlingInt(vehicleid, CHandlingAttrib:attribute, value);
	PAWN_NATIVE(n_SetVehicleHandlingInt);
	
	// native SetModelHandlingFloat(modelid, CHandlingAttrib:attribute, Float:value);
	PAWN_NATIVE(n_SetModelHandlingFloat);
	// native SetModelHandlingInt(modelid, CHandlingAttrib:attribute, value);
	PAWN_NATIVE(n_SetModelHandlingInt);
	
	// native GetVehicleHandlingFloat(vehicleid, CHandlingAttrib:attribute, &Float:value);
	PAWN_NATIVE(n_GetVehicleHandlingFloat);
	// native GetVehicleHandlingInt(vehicleid, CHandlingAttrib:attribute, &value);
	PAWN_NATIVE(n_GetVehicleHandlingInt);
	
	// native GetModelHandlingFloat(modelid, CHandlingAttrib:attribute, &Float:value);
	PAWN_NATIVE(n_GetModelHandlingFloat);
	// native GetModelHandlingInt(modelid, CHandlingAttrib:attribute, &value);
	PAWN_NATIVE(n_GetModelHandlingInt);
}