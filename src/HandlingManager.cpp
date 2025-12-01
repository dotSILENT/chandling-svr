#include "chandlingsvr.h"
#include "PacketEnum.h"
#include "Actions.h"
#include "CPlayer.h"
#include "HandlingDefault.h"

#include "HandlingManager.h"

#include <unordered_map>
#include <unordered_set>
#include <cstring>

#define CHECK_TYPE(attribute, type)                                                                                  \
	if (GetHandlingAttributeType(attrib) != type)                                                                       \
	{                                                                                                                \
		auto core_ = CHandlingCompo::getCore();                                                                      \
		if (!core_)                                                                                                  \
		{                                                                                                            \
			return false;                                                                                            \
		}                                                                                                            \
		core_->logLn(LogLevel::Error, "[CHandling] Invalid type (%.*d) specified for attribute %.*d", type, attrib); \
		return false;                                                                                                \
	}

namespace HandlingMgr
{
	struct stHandlingMod
	{
		CHandlingAttribType type;
		union
		{
			float fval;
			unsigned int uival;
			uint8_t bval;
		};
	};

	struct stHandlingEntry
	{
		struct tHandlingData handlingData;
		std::unordered_map<CHandlingAttrib, struct stHandlingMod, std::hash<uint8_t>> handlingModMap; // modifications are saved here so we only send things that have changed
	};

	struct stVehicleHandlingEntry : stHandlingEntry
	{
		struct stHandlingEntry *modelHandling;
		bool usesModelHandling = false; // set to true under OnCreateVehicle, set to false as soon as you change any handling attribute for this vehicle
	};

	struct stHandlingEntry modelHandlings[MAX_VEHICLE_MODELS];
	struct stVehicleHandlingEntry vehicleHandlings[MAX_VEHICLES + 1];

	std::unordered_set<uint16_t> usOutgoingVehicleMods;
	std::unordered_set<uint16_t> usOutgoingModelMods;

	/*
	 *  INTERNAL FUNCTIONS
	 */
	void __WriteHandlingEntryToBitStream(NetworkBitStream *bs, const struct stHandlingEntry entry)
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
		else
			handling->handlingModMap.emplace(attribute, mod);

		void *offs = GetHandlingAttribPtr(&handling->handlingData, attribute);
		/* write the value to the handling data so we can Get it later on */
		switch (mod.type)
		{
		case TYPE_FLOAT:
			*(float *)offs = mod.fval;
		case TYPE_UINT:
		case TYPE_FLAG:
			*(unsigned int *)offs = mod.uival;
			break;
		case TYPE_BYTE:
			*(uint8_t *)offs = mod.bval;
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
		if (!CHandlingCompo::IsValidVehicle(vehicleid))
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

			if (!CHandlingCompo::IsValidVehicle(vehicleid) || vehicleHandlings[vehicleid].usesModelHandling)
			{
				usOutgoingVehicleMods.clear();
				continue;
			}
			struct CHandlingActionPacket p(ACTION_SET_VEHICLE_HANDLING);
			p.data.Write(vehicleid);
			__WriteHandlingEntryToBitStream(&p.data, vehicleHandlings[vehicleid]);

			for (IPlayer *player : CHandlingCompo::getCore()->getPlayers().players())
			{
				player->sendPacket(Span<uint8_t>(p.data.GetData(), p.data.GetNumberOfBitsUsed()), 0, true);
			}
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

			for(IPlayer *player : CHandlingCompo::getCore()->getPlayers().players())
			{
				player->sendPacket(Span<uint8_t>(p.data.GetData(), p.data.GetNumberOfBitsUsed()), 0, true);
			}
		}
	}

	// call right after HandlingDefault::Initialize()
	void InitializeModelHandlings()
	{
		for (uint16_t i = 0; i < MAX_VEHICLE_MODELS; i++)
		{
			HandlingDefault::copyDefaultModelHandling(i + 400, &modelHandlings[i].handlingData);
		}
	}

	void OnCreateVehicle(int vehicleid)
	{
		ResetVehicleHandling(*CHandlingCompo::GetVehicleByID(vehicleid));
	}

	void OnPlayerConnect(IPlayer &player)
	{
		int playerid = player.getID();
		if (!gPlayers[playerid].hasCHandling())
			return;

		for (int model = 0; model < MAX_VEHICLE_MODELS; model++)
		{
			if (!modelHandlings[model].handlingModMap.empty())
			{
				struct CHandlingActionPacket p(ACTION_SET_MODEL_HANDLING);

				p.data.Write((uint16_t)(model + 400));

				__WriteHandlingEntryToBitStream(&p.data, modelHandlings[model]);

				player.sendPacket(Span<uint8_t>(p.data.GetData(), p.data.GetNumberOfBitsUsed()), 0, false);
			}
		}
	}

	void OnVehicleStreamIn(IVehicle &vehicle, IPlayer &player)
	{
		int vehicleid = vehicle.getID();
		int forplayerid = player.getID();
		if (vehicleHandlings[vehicleid].handlingModMap.empty() || !gPlayers[forplayerid].hasCHandling())
			return;

		struct CHandlingActionPacket p(ACTION_SET_VEHICLE_HANDLING);
		p.data.Write((uint16_t)vehicleid);

		__WriteHandlingEntryToBitStream(&p.data, vehicleHandlings[vehicleid]);

		player.sendPacket(Span<uint8_t>(p.data.GetData(), p.data.GetNumberOfBitsUsed()), 0, false);
	}

	/*
	 * Resets handling of specified vehicle model to it's original default one
	 * NOTE: This doesn't reset vehicles that have modified the model handling, these use their own (older) copy
	 */
	bool ResetModelHandling(int modelid)
	{
		int model_index = VEHICLE_MODEL_INDEX(modelid);
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		modelHandlings[model_index].handlingModMap.clear();

		HandlingDefault::copyDefaultModelHandling((uint16_t)modelid, &modelHandlings[model_index].handlingData);

		struct CHandlingActionPacket p(ACTION_RESET_MODEL);
		p.data.Write((uint16_t)modelid);
		// this needs to be announced to every player
		for (IPlayer *player : CHandlingCompo::getCore()->getPlayers().players())
		{
			player->sendPacket(Span<uint8_t>(p.data.GetData(), p.data.GetNumberOfBitsUsed()), 0, true);
		}
		return true;
	}

	/*
	 * Resets the handling of specified vehicle to it's model handling (only if needed)
	 * sendToPlayers is true by default, set it to false only when resetting model handling
	 */
	void ResetVehicleHandling(IVehicle &vehicle, bool sendToPlayers)
	{
		int vehicleid = vehicle.getID();
		int modelid = vehicle.getModel();

		vehicleHandlings[vehicleid].handlingModMap.clear();
		vehicleHandlings[vehicleid].modelHandling = &modelHandlings[VEHICLE_MODEL_INDEX(modelid)];
		vehicleHandlings[vehicleid].usesModelHandling = true;

		// use sendToPlayers = false when resetting model handling, client code can handle it on his own
		if (sendToPlayers)
		{
			struct CHandlingActionPacket p(ACTION_RESET_VEHICLE);
			p.data.Write(vehicleid);

			for(IPlayer *player : CHandlingCompo::getCore()->getPlayers().players())
			{
				player->sendPacket(Span<uint8_t>(p.data.GetData(), p.data.GetNumberOfBitsUsed()), 0, true);
			}
		}
	}

	/* SET HANDLING FUNCTIONS */

	bool SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, float value)
	{
		if (!CHandlingCompo::IsValidVehicle(vehicleid) || !CanSetHandlingAttrib(attrib))
			return false;
		CHECK_TYPE(attrib, TYPE_FLOAT)

		if (!IsValidHandlingValue(attrib, value))
			return false;

		struct stHandlingMod mod;
		mod.fval = value;
		mod.type = TYPE_FLOAT;

		return __AddVehicleHandlingMod(vehicleid, attrib, mod);
	}

	bool SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, unsigned int value)
	{
		if (!CHandlingCompo::IsValidVehicle(vehicleid) || !CanSetHandlingAttrib(attrib)) // no validation checking for unsigned integers
			return false;

		CHandlingAttribType type = GetHandlingAttributeType(attrib);
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		struct stHandlingMod mod;
		mod.uival = value;
		mod.type = TYPE_UINT;
		return __AddVehicleHandlingMod(vehicleid, attrib, mod);
	}

	bool SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, uint8_t value)
	{
		if (!CHandlingCompo::IsValidVehicle(vehicleid) || !CanSetHandlingAttrib(attrib))
			return false;
		CHECK_TYPE(attrib, TYPE_BYTE)

		if (!IsValidHandlingValue(attrib, value))
			return false;

		struct stHandlingMod mod;
		mod.bval = value;
		mod.type = TYPE_BYTE;
		return __AddVehicleHandlingMod(vehicleid, attrib, mod);
	}

	bool SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, float value)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid) || !CanSetHandlingAttrib(attrib))
			return false;

		CHECK_TYPE(attrib, TYPE_FLOAT)

		if (!IsValidHandlingValue(attrib, value))
			return false;

		struct stHandlingMod mod;
		mod.fval = value;
		mod.type = TYPE_FLOAT;
		return __AddModelHandlingMod(modelid, attrib, mod);
	}

	bool SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int value)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid) || !CanSetHandlingAttrib(attrib))
			return false;
		CHandlingAttribType type = GetHandlingAttributeType(attrib);
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		struct stHandlingMod mod;
		mod.uival = value;
		mod.type = TYPE_UINT;
		return __AddModelHandlingMod(modelid, attrib, mod);
	}

	bool SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t value)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid) || !CanSetHandlingAttrib(attrib))
			return false;

		CHECK_TYPE(attrib, TYPE_BYTE)

		if (!IsValidHandlingValue(attrib, value))
			return false;

		struct stHandlingMod mod;
		mod.bval = value;
		mod.type = TYPE_BYTE;
		return __AddModelHandlingMod(modelid, attrib, mod);
	}

	/* GET */

	bool GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, float &ret)
	{
		if (!CHandlingCompo::IsValidVehicle(vehicleid))
			return false;
		CHECK_TYPE(attrib, TYPE_FLOAT)

		ret = *(float *)GetHandlingAttribPtr(vehicleHandlings[vehicleid].usesModelHandling ? &vehicleHandlings[vehicleid].modelHandling->handlingData : &vehicleHandlings[vehicleid].handlingData, attrib);
		return true;
	}

	bool GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, unsigned int &ret)
	{
		if (!CHandlingCompo::IsValidVehicle(vehicleid))
			return false;
		CHandlingAttribType type = GetHandlingAttributeType(attrib);

		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		ret = *(unsigned int *)GetHandlingAttribPtr(vehicleHandlings[vehicleid].usesModelHandling ? &vehicleHandlings[vehicleid].modelHandling->handlingData : &vehicleHandlings[vehicleid].handlingData, attrib);
		return true;
	}

	bool GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, uint8_t &ret)
	{
		if (!CHandlingCompo::IsValidVehicle(vehicleid))
			return false;
		CHECK_TYPE(attrib, TYPE_BYTE)

		ret = *(uint8_t *)GetHandlingAttribPtr(vehicleHandlings[vehicleid].usesModelHandling ? &vehicleHandlings[vehicleid].modelHandling->handlingData : &vehicleHandlings[vehicleid].handlingData, attrib);
		return true;
	}

	bool GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, float &ret)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;
		CHECK_TYPE(attrib, TYPE_FLOAT)

		ret = *(float *)GetHandlingAttribPtr(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingData, attrib);
		return true;
	}

	bool GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int &ret)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		CHandlingAttribType type = GetHandlingAttributeType(attrib);
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		ret = *(unsigned int *)GetHandlingAttribPtr(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingData, attrib);
		return true;
	}

	bool GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t &ret)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;
		CHECK_TYPE(attrib, TYPE_BYTE)

		ret = *(uint8_t *)GetHandlingAttribPtr(&modelHandlings[VEHICLE_MODEL_INDEX(modelid)].handlingData, attrib);
		return true;
	}

	bool GetDefaultHandling(uint16_t modelid, CHandlingAttrib attrib, float &ret)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		CHECK_TYPE(attrib, TYPE_FLOAT)

		struct tHandlingData *pHandl = HandlingDefault::getDefaultModelHandling(modelid);
		if (pHandl == nullptr)
			return false;
		ret = *(float *)GetHandlingAttribPtr(pHandl, attrib);
		return true;
	}

	bool GetDefaultHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int &ret)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		CHandlingAttribType type = GetHandlingAttributeType(attrib);
		if (!(type == TYPE_UINT || type == TYPE_FLAG))
			return false;

		struct tHandlingData *pHandl = HandlingDefault::getDefaultModelHandling(modelid);
		if (pHandl == nullptr)
			return false;
		ret = *(unsigned int *)GetHandlingAttribPtr(pHandl, attrib);
		return true;
	}

	bool GetDefaultHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t &ret)
	{
		if (!IS_VALID_VEHICLE_MODEL(modelid))
			return false;

		CHECK_TYPE(attrib, TYPE_BYTE)

		struct tHandlingData *pHandl = HandlingDefault::getDefaultModelHandling(modelid);
		if (pHandl == nullptr)
			return false;
		ret = *(uint8_t *)GetHandlingAttribPtr(pHandl, attrib);
		return true;
	}
}
