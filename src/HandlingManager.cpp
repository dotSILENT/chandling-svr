#include <unordered_map>
#include "sampgdk/sampgdk.h"
#include "chandlingsvr.h"
#include "PacketEnum.h"
#include "Actions.h"
#include "CPlayer.h"

#include "HandlingManager.h"


struct stHandlingMod
{
	union
	{
		float	fval;
		int		ival;
		uint8_t	bvar;
	};
};

struct stHandlingEntry
{
	struct tHandlingData	handlingData;
	std::unordered_map<CHandlingAttrib, struct stHandlingMod>	handlingModMap; // modifications are saved here so we only send things that have changed
};

struct stVehicleHandlingEntry : stHandlingEntry
{
	struct stHandlingEntry*	modelHandling;
	bool usesModelHandling = true; // set to false as soon as you change any handling attribute for this vehicle
};

struct stHandlingEntry			modelHandlings[MAX_VEHICLE_MODELS];
struct stVehicleHandlingEntry	vehicleHandlings[MAX_VEHICLES+1];


namespace HandlingMgr
{

	void CreateVehicle(int vehicleid)
	{
		if (!bInitialized)
			return;

		ResetVehicleHandling(vehicleid);
	}

	/* Resets the handling of specified vehicle to it's model handling (only if needed) */
	bool ResetVehicleHandling(int vehicleid)
	{
		if (!bInitialized)
			return false;

		int modelid = GetVehicleModel(vehicleid);
		if (!IS_VALID_VEHICLEID(vehicleid) || !IS_VALID_VEHICLE_MODEL(modelid) || vehicleHandlings[vehicleid].usesModelHandling)
			return false;

		vehicleHandlings[vehicleid].handlingModMap.clear();
		vehicleHandlings[vehicleid].modelHandling = &modelHandlings[VEHICLE_MODEL_INDEX(modelid)];
		vehicleHandlings[vehicleid].usesModelHandling = true;

		// transmit the reset action to all players who have this vehicle  streamed in (or maybe just broadcast?)
		RakNet::BitStream bs;
		bs.Write((uint8_t)ACTION_RESET_VEHICLE);
		bs.Write(vehicleid);

		for (int i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if (IsVehicleStreamedIn(vehicleid, i) && gPlayers[i].hasCHandling())
			{
				pRakServer->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, pRakServer->GetPlayerIDFromIndex(i), false);
			}
		}
		return true;
	}
}