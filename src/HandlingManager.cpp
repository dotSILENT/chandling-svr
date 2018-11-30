#include "sampgdk/sampgdk.h"
#include "chandlingsvr.h"
#include "PacketEnum.h"
#include "Actions.h"
#include "CPlayer.h"
#include "HandlingDefault.h"

#include "HandlingManager.h"

#include <unordered_map>
#include <unordered_set>

#define CHECK_TYPE(attribute,type) \
	if(GetHandlingAttribType(attrib) != type) \
		{\
		sampgdk::logprintf("[chandling] Invalid type (%d) specified for attribute %d", type, attrib);\
		return false;\
		}

namespace HandlingMgr
{
	struct stHandlingMod
	{
		CHandlingAttribType type;
		union
		{
			float				fval;
			unsigned int		uival;
			uint8_t				bval;
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
		bool usesModelHandling = false; // set to true under OnCreateVehicle, set to false as soon as you change any handling attribute for this vehicle
	};

	struct stHandlingEntry			modelHandlings[MAX_VEHICLE_MODELS];
	struct stVehicleHandlingEntry	vehicleHandlings[MAX_VEHICLES + 1];

	std::unordered_set<uint16_t>	usOutgoingVehicleMods;
	std::unordered_set<uint16_t>	usOutgoingModelMods;

	/*
	 *  INTERNAL FUNCTIONS
	*/
	void __WriteHandlingEntryToBitStream(RakNet::BitStream* bs, const struct stHandlingEntry entry)
	{
		bs->Write((uint8_t)entry.handlingModMap.size());

		for (auto const &i : entry.handlingModMap)
		{
			bs->Write((uint8_t)i.first); // attribute
			bs->Write((uint8_t)i.second.type);
			switch (i.second.type)
			{
			case TYPE_BYTE:
				bs->Write(i.second.bval);
				break;
			case TYPE_UINT:
			case TYPE_FLAG:
				bs->Write(i.second.uival);
				break;
			case TYPE_FLOAT:
				bs->Write(i.second.fval);
				break;
			}
		}
	}
	// dis funcion no use no no
	void __addMod(struct stHandlingEntry *handling, CHandlingAttrib attribute, const struct stHandlingMod mod)
	{
		if (handling->handlingModMap.count(attribute))
			handling->handlingModMap.at(attribute) = mod;
		else handling->handlingModMap.emplace(attribute, mod);

		void* offs = GetHandlingAttribPtr(&handling->handlingData, attribute);
		/* write the value to the handling data so we can Get it later on */
		switch (mod.type)
		{
		case TYPE_FLOAT:
			*(float*)offs = mod.fval;
		case TYPE_UINT:
		case TYPE_FLAG:
			*(unsigned int*)offs = mod.uival;
			break;
		case TYPE_BYTE:
			*(uint8_t*)offs = mod.bval;
			break;
		}
	}
	
	// dis use
	bool __AddModelHandlingMod(uint16_t modelid, CHandlingAttrib attribute, const struct stHandlingMod mod)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;
		__addMod(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)], attribute, mod);
		
		usOutgoingModelMods.emplace(modelid);
		return true;
	}

	bool __AddVehicleHandlingMod(uint16_t vehicleid, CHandlingAttrib attribute, const struct stHandlingMod mod)
	{
		if (!IS_VALID_VEHICLEID(vehicleid))
			return false;

		// copy the handling of the model & apply the changed value
		if (vehicleHandlings[vehicleid].usesModelHandling)
		{
			vehicleHandlings[vehicleid].usesModelHandling = false;
			memcpy(&vehicleHandlings[vehicleid].handlingData, &vehicleHandlings[vehicleid].modelHandling->handlingData, sizeof(struct tHandlingData));
		}
		__addMod(&vehicleHandlings[vehicleid], attribute, mod);

		usOutgoingVehicleMods.emplace(vehicleid);
		return true;
	}

	/* -------------------------------------------------------------------------------------------------------------------- */


	/* We use ProcessTick to broadcast queued modifications all at once instead of spamming with packets */
	void ProcessTick()
	{
		while (!usOutgoingVehicleMods.empty())
		{
			const auto &it = usOutgoingVehicleMods.begin();
			uint16_t vehicleid = *it;
			usOutgoingVehicleMods.erase(it);
			
			if (!IsValidVehicle(vehicleid) || vehicleHandlings[vehicleid].usesModelHandling)
			{
				usOutgoingVehicleMods.clear();
				continue;
			}
			struct CHandlingActionPacket p(ACTION_SET_VEHICLE_HANDLING);
			p.data.Write(vehicleid);
			__WriteHandlingEntryToBitStream(&p.data, vehicleHandlings[vehicleid]);

			pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true);
		}

		while (!usOutgoingModelMods.empty())
		{
			const auto &it = usOutgoingModelMods.begin();
			uint16_t modelid = *it;
			usOutgoingModelMods.erase(it);

			if (!IS_VALID_VEHICLE_MODEL(modelid) || modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingModMap.empty())
			{
				usOutgoingModelMods.clear();
				continue;
			}

			struct CHandlingActionPacket p(ACTION_SET_MODEL_HANDLING);
			p.data.Write(modelid);
			__WriteHandlingEntryToBitStream(&p.data, modelHandlings[VEHICLE_MODEL_INDEX(modelid)]);

			pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true);
		}
	}

	// call right after HandlingDefault::Initialize()
	void InitializeModelHandlings()
	{
		for (int i = 0; i < MAX_VEHICLE_MODELS; i++)
		{
			HandlingDefault::copyDefaultModelHandling(i + 400, &modelHandlings[i].handlingData);
		}
	}

	void OnCreateVehicle(int vehicleid)
	{
		if (!bInitialized)
			return;

		ResetVehicleHandling(vehicleid);
	}

	void OnPlayerConnect(int playerid)
	{
		sampgdk::logprintf("connect %d has %d", playerid, gPlayers[playerid].hasCHandling());
		if (!IS_VALID_PLAYERID(playerid) || !gPlayers[playerid].hasCHandling())
			return;
		
		for (int model = 0; model < MAX_VEHICLE_MODELS; model++)
		{
			if (!modelHandlings[model].handlingModMap.empty())
			{
				struct CHandlingActionPacket p(ACTION_SET_MODEL_HANDLING);

				p.data.Write((uint16_t) (model + 400));

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
	 * NOTE: This doesn't reset vehicles that have modified the model handling, these use their own (older) copy
	*/
	bool ResetModelHandling(int modelid)
	{
		int model_index = VEHICLE_MODEL_INDEX(modelid);
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid))
			return false;
		
		modelHandlings[model_index].handlingModMap.clear();

		HandlingDefault::copyDefaultModelHandling(modelid, &modelHandlings[model_index].handlingData);

		struct CHandlingActionPacket p(ACTION_RESET_MODEL);
		p.data.Write((uint16_t)modelid);
		// this needs to be announced to every player
		pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true);
		return true;
	}

	/* 
	 * Resets the handling of specified vehicle to it's model handling (only if needed)
	 * sendToPlayers is true by default, set it to false only when resetting model handling
	*/
	bool ResetVehicleHandling(int vehicleid, bool sendToPlayers)
	{
		if (!bInitialized)
			return false;

		int modelid = GetVehicleModel(vehicleid);
		if (!IS_VALID_VEHICLEID(vehicleid) || !IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		vehicleHandlings[vehicleid].handlingModMap.clear();
		vehicleHandlings[vehicleid].modelHandling = &modelHandlings[VEHICLE_MODEL_INDEX(modelid)];
		vehicleHandlings[vehicleid].usesModelHandling = true;

		// use sendToPlayers = false when resetting model handling, client code can handle it on his own
		if (sendToPlayers)
		{
			struct CHandlingActionPacket p(ACTION_RESET_VEHICLE);
			p.data.Write(vehicleid);

			pRakServer->Send(&p.data, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true);
		}
		return true;
	}


	/* SET HANDLING FUNCTIONS */

	bool SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, float value)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid) || !CanSetHandlingAttrib(attrib) || !IsValidHandlingValue(attrib, value))
			return false;
		//CHECK_TYPE(attrib, TYPE_FLOAT) // IsValidHandlingValue takes care of this

		struct stHandlingMod mod;
		mod.fval = value;
		mod.type = TYPE_FLOAT;

		return __AddVehicleHandlingMod(vehicleid, attrib, mod);
	}

	bool SetVehicleHandling(uint16_t  vehicleid, CHandlingAttrib attrib, unsigned int value)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid) || !CanSetHandlingAttrib(attrib)) // no validation checking for unsigned integers
			return false;	
		CHECK_TYPE(attrib, TYPE_UINT)

		struct stHandlingMod mod;
		mod.uival = value;
		mod.type = TYPE_UINT;
		return __AddVehicleHandlingMod(vehicleid, attrib, mod);
	}

	bool SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, uint8_t value)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid) || !CanSetHandlingAttrib(attrib) || !IsValidHandlingValue(attrib, value))
			return false;
		//CHECK_TYPE(attrib, TYPE_BYTE) // IsValidHandlingValue takes care of this

		struct stHandlingMod mod;
		mod.bval = value;
		mod.type = TYPE_BYTE;
		return __AddVehicleHandlingMod(vehicleid, attrib, mod);
	}

	bool SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, float value)
	{
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid) || !CanSetHandlingAttrib(attrib) || !IsValidHandlingValue(attrib, value))
			return false;

		struct stHandlingMod mod;
		mod.fval = value;
		mod.type = TYPE_FLOAT;
		return __AddModelHandlingMod(modelid, attrib, mod);
	}

	bool SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int value)
	{
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid) || !CanSetHandlingAttrib(attrib))
			return false;
		CHandlingAttribType type = GetHandlingAttribType(attrib);
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		struct stHandlingMod mod;
		mod.uival = value;
		mod.type = TYPE_UINT;
		return __AddModelHandlingMod(modelid, attrib, mod);
	}

	bool SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t value)
	{
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid) || !CanSetHandlingAttrib(attrib) || !IsValidHandlingValue(attrib, value))
			return false;

		struct stHandlingMod mod;
		mod.bval = value;
		mod.type = TYPE_BYTE;
		return __AddModelHandlingMod(modelid, attrib, mod);
	}

	/* GET */

	bool GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, float &ret)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid))
			return false;
		CHECK_TYPE(attrib, TYPE_FLOAT)

		ret = *(float*)GetHandlingAttribPtr(vehicleHandlings[vehicleid].usesModelHandling ? &vehicleHandlings[vehicleid].modelHandling->handlingData : &vehicleHandlings[vehicleid].handlingData, attrib);
		return true;
	}

	bool GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, unsigned int &ret)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid))
			return false;
		CHandlingAttribType type = GetHandlingAttribType(attrib);
		
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		ret = *(unsigned int*)GetHandlingAttribPtr(vehicleHandlings[vehicleid].usesModelHandling ? &vehicleHandlings[vehicleid].modelHandling->handlingData : &vehicleHandlings[vehicleid].handlingData, attrib);
		return true;
	}

	bool GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, uint8_t &ret)
	{
		if (!bInitialized || !IsValidVehicle(vehicleid))
			return false;
		CHECK_TYPE(attrib, TYPE_BYTE)

		ret = *(uint8_t*)GetHandlingAttribPtr(vehicleHandlings[vehicleid].usesModelHandling ? &vehicleHandlings[vehicleid].modelHandling->handlingData : &vehicleHandlings[vehicleid].handlingData, attrib);
		return true;
	}

	bool GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, float &ret)
	{
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid))
			return false;
		CHECK_TYPE(attrib, TYPE_FLOAT)

		ret = *(float*)GetHandlingAttribPtr(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingData, attrib);
		return true;
	}

	bool GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int &ret)
	{
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		CHandlingAttribType type = GetHandlingAttribType(attrib);
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		ret = *(unsigned int*)GetHandlingAttribPtr(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingData, attrib);
		return true;
	}

	bool GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t &ret)
	{
		if (!bInitialized || !IS_VALID_VEHICLE_MODEL(modelid))
			return false;
		CHECK_TYPE(attrib, TYPE_BYTE)

		ret = *(uint8_t*)GetHandlingAttribPtr(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingData, attrib);
		return true;
	}
}