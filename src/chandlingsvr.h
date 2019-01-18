#include "raknet/raknet.h"

#define CHANDLING_VERSION "v1.0-dev"

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