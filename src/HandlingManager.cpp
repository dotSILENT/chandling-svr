#include <unordered_map>
#include "sampgdk/sampgdk.h"
#include "chandlingsvr.h"
#include "PacketEnum.h"
#include "Actions.h"
#include "CPlayer.h"

#include "HandlingManager.h"


struct stHandlingMod
{
	CHandlingAttribType type;
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

	/*
	 *  INTERNAL FUNCTIONS
	*/

	void __WriteHandlingEntryToBitStream(RakNet::BitStream* bs, const struct stHandlingEntry entry)
	{
		bs->Write((uint8_t)entry.handlingModMap.size());

		for (auto const &i : entry.handlingModMap)
		{
			bs->Write((uint8_t)i.first);
			switch (i.second.type)
			{
			case TYPE_BYTE:
				bs->Write(i.second.bvar);
				break;
			case TYPE_INT:
				bs->Write(i.second.ival);
				break;
			case TYPE_FLAG:
				bs->Write(i.second.ival);
				break;
			case TYPE_FLOAT:
				bs->Write(i.second.fval);
				break;
			}
		}
	}

	void __AddHandlingMod(struct stHandlingEntry *handling, CHandlingAttrib attribute, const struct stHandlingMod mod)
	{
		if (handling->handlingModMap.count(attribute))
			handling->handlingModMap.at(attribute) = mod;
		else handling->handlingModMap.insert(std::make_pair(attribute, mod));
	}

	void __AddVehicleHandlingMod(struct stVehicleHandlingEntry *handling, CHandlingAttrib attribute, const struct stHandlingMod mod)
	{
		__AddHandlingMod(handling, attribute, mod);
		handling->usesModelHandling = false;
	}

	/* -------------------------------------------------------------------------------------------------------------------- */

	void OnCreateVehicle(int vehicleid)
	{
		if (!bInitialized)
			return;

		ResetVehicleHandling(vehicleid);
	}

	void OnPlayerConnect(int playerid)
	{
		if (!IS_VALID_PLAYERID(playerid) || !gPlayers[playerid].hasCHandling())
			return;
		
		for (int model = 0; model < MAX_VEHICLE_MODELS; model++)
		{
			if (!modelHandlings[model].handlingModMap.empty())
			{
				struct CHandlingActionPacket p(ACTION_SET_MODEL_HANDLING);

				p.data.Write((uint16_t)model + 400);

				__WriteHandlingEntryToBitStream(&p.data, modelHandlings[model]);

				pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, pRakServer->GetPlayerIDFromIndex(playerid), false);
			}
		}
	}

	void OnVehicleStreamIn(int vehicleid, int forplayerid)
	{
		if (!IsValidVehicle(vehicleid) || vehicleHandlings[vehicleid].handlingModMap.empty() || !IS_VALID_PLAYERID(forplayerid) || !gPlayers[forplayerid].hasCHandling())
			return;

		struct CHandlingActionPacket p(ACTION_SET_VEHICLE_HANDLING);
		p.data.Write((uint16_t)vehicleid);

		__WriteHandlingEntryToBitStream(&p.data, vehicleHandlings[vehicleid]);

		pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, pRakServer->GetPlayerIDFromIndex(forplayerid), false);
	}

	/* 
	 * Resets handling of specified vehicle model to it's original default one
	 * NOTE: This also resets handlings (and handling mods) of all vehicles of this model
	*/
	bool ResetModelHandling(int modelid)
	{
		int model_index = VEHICLE_MODEL_INDEX(modelid);
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid) || modelHandlings[model_index].handlingModMap.empty())
			return false;
		
		modelHandlings[model_index].handlingModMap.clear();

		// TODO: Add default handling values

		for (int i = 1, j = GetVehiclePoolSize(); i < j && i <= MAX_VEHICLES; i++)
		{
			if (!vehicleHandlings[i].usesModelHandling && vehicleHandlings[i].modelHandling == &modelHandlings[model_index] && !vehicleHandlings[i].handlingModMap.empty())
			{
				ResetVehicleHandling(i, false); // we don't need to send this packet for every player, because client code handles it on his own when receiving ACTION_RESET_MODEL
			}
		}

		struct CHandlingActionPacket p(ACTION_RESET_MODEL);
		p.data.Write((uint16_t)modelid);
		// this needs to be announced to every player
		pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true);
		return true;
	}

	/* 
	 * Resets the handling of specified vehicle to it's model handling (only if needed)
	 * sendToPlayers is true by default, use it only when resetting model handling
	*/
	bool ResetVehicleHandling(int vehicleid, bool sendToPlayers)
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
		// use sendToPlayers = false when resetting model handling, client code can handle it on his own
		if (sendToPlayers)
		{
			struct CHandlingActionPacket p(ACTION_RESET_VEHICLE);
			p.data.Write(vehicleid);

			for (int i = 0, j = GetPlayerPoolSize(); i <= j && i < MAX_PLAYERS; i++)
			{
				if (IsVehicleStreamedIn(vehicleid, i) && gPlayers[i].hasCHandling())
				{
					pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, pRakServer->GetPlayerIDFromIndex(i), false);
				}
			}
		}
		return true;
	}



	bool SetVehicleHandling(int vehicleid, CHandlingAttrib attrib, const float value)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid))
			return false;

		struct stHandlingMod mod;
		mod.fval = value;

		__AddVehicleHandlingMod(&vehicleHandlings[vehicleid], attrib, mod);

		struct CHandlingActionPacket p(ACTION_SET_VEHICLE_HANDLING);
		p.data.Write((uint16_t)vehicleid);
		
		__WriteHandlingEntryToBitStream(&p.data, vehicleHandlings[vehicleid]);

		/* this is here just for now */
		for (int i = 0, j = GetPlayerPoolSize(); i <= j && i < MAX_PLAYERS; i++)
		{
			if (IsVehicleStreamedIn(vehicleid, i))
			{
				pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, pRakServer->GetPlayerIDFromIndex(i), false);
				sampgdk::logprintf("[chandling] SetVehicleHandlingFloat(%d, %d, %f) sent to player %d", vehicleid, attrib, value, i);
			}
		}
		return true;
	}
}