#pragma once

#include "raknet/BitStream.h"
#include "sampgdk/sampgdk.h"
#include "PacketEnum.h"

// action identifier is sent as single byte
enum CHandlingAction : unsigned char
{
	ACTION_INIT = 10, // This is the only packet that is sent  by the player, to indicate that we can speak to him

	ACTION_RESET_MODEL = 15,
	ACTION_RESET_VEHICLE,
	ACTION_SET_VEHICLE_HANDLING,
	ACTION_SET_MODEL_HANDLING
};

struct CHandlingActionPacket
{
	RakNet::BitStream data;

	CHandlingActionPacket(CHandlingAction actionID)
	{
		data.Write((uint8_t)ID_CHANDLING);
		data.Write((uint8_t)actionID);
	}
};

namespace Actions
{
	bool Process(CHandlingAction id, RakNet::BitStream *bs, int playerid);
}