#pragma once
#include "HandlingEnum.h"
#include "HandlingStruct.h"

#ifndef MAX_VEHICLES
#define MAX_VEHICLES	2000
#endif

#ifndef MAX_VEHICLE_MODELS
#define MAX_VEHICLE_MODELS	212
#endif

// keep in mind  vehicle ids in samp start from 1
#define IS_VALID_VEHICLEID(id) \
	(id >= 1 && id <= MAX_VEHICLES)

#define IS_VALID_VEHICLE_MODEL(modelid) \
	(modelid > 400 && modelid < MAX_VEHICLE_MODELS+400)

#define VEHICLE_MODEL_INDEX(modelid) \
	(modelid - 400)

namespace HandlingMgr
{

	void	OnCreateVehicle(int vehicleid); // needs to be called after pawn's CreateVehicle is called in order to reset the data
	void	OnPlayerConnect(int playerid); // call this from OnPlayerConnect (or rather from ACTION_INIT handler) so model handling modifications are sent to the player
	void	OnVehicleStreamIn(int vehicleid, int forplayerid); // call from OnVehicleStreamIn so handling modifications for this individual vehicle are sent to the player

	bool	ResetModelHandling(int modelid); // resets model handling to it's default one, NOTE: this resets any handling modifications for every vehicle of that model
	bool	ResetVehicleHandling(int vehicleid, bool sendToPlayers=true); // resets vehicle handling to it's model handling (and clears the modifications)
	bool	SetVehicleHandling(int vehicleid, CHandlingAttrib attrib, const float value);
}