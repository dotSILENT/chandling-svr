#include "sampgdk\sampgdk.h"
#include "samp-plugin-sdk\amx\amx.h"


#define CHECK_PARAMS(m, n) \
	if (params[0] != (m * 4)) \
			{ \
		logprintf("[chandling] %s: Expecting %d parameter(s), but found %d", n, m, params[0] / 4); \
		return 0; \
		}

namespace Natives
{
	extern AMX_NATIVE_INFO PluginNatives[];

	// native GetHandlingAttribType(CHandlingAttrib:attribute);
	cell AMX_NATIVE_CALL n_GetHandlingAttribType(AMX* amx, cell* params);

	// native IsPlayerUsingCHandling(playerd);
	cell AMX_NATIVE_CALL n_IsPlayerUsingCHandling(AMX* amx, cell* params);
	// native SetVehicleHandlingFloat(vehicleid, CHandlingAttrib:attribute, Float:value);
	cell AMX_NATIVE_CALL n_SetVehicleHandlingFloat(AMX* amx, cell* params);
}