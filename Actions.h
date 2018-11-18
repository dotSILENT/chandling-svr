#include "BitStream.h"
#include "sampgdk.h"

enum CHandlingAction
{
	ACTION_INIT = 10, // This is the only packet that is sent  by the player, to indicate that we can speak to him
	
	ACTION_SET_VEHICLE_HANDLING = 15,
	ACTION_SET_MODEL_HANDLING = 20
};

namespace Actions
{
	bool Process(CHandlingAction id, RakNet::BitStream *bs, int playerid)
	{
		switch (id)
		{
		case ACTION_INIT:
			sampgdk::logprintf("[chandling] Player %d reports having chandling plugin", playerid);
			return true;
		}
		return false;
	}
}