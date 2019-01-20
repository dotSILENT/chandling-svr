#include "raknet/raknet.h"

#define CHANDLING_VERSION "v1.0-dev"

/*
 * The compaatibility version number decides if the client supports our CHandling version or not
 * Clients with smaller compat version won't be able to use CHandling
 * 
 * Increase this number only if doing things that break the compatibility with older client
*/
#define CHANDLING_COMPAT_VERSION 0x1001D

#define MAX_VEHICLE_MODELS (212)
#ifndef MAX_VEHICLES
#define MAX_VEHICLES (2000)
#endif

// keep in mind  vehicle ids in samp start from 1
#define IS_VALID_VEHICLEID(id) \
	(id >= 1 && id <= MAX_VEHICLES)

#define IS_VALID_VEHICLE_MODEL(modelid) \
	(modelid >= 400 && modelid < MAX_VEHICLE_MODELS+400)

#define VEHICLE_MODEL_INDEX(modelid) \
	(modelid - 400)

extern CCRakServer* pRakServer;
extern bool bInitialized;