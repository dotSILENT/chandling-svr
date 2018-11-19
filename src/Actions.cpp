#include "Actions.h"

bool Actions::Process(CHandlingAction id, RakNet::BitStream *bs, int playerid)
{
	switch (id)
	{
	case ACTION_INIT:
		if (IS_VALID_PLAYERID(playerid))
		{
			sampgdk::logprintf("[chandling] Player %d reports having chandling plugin", playerid);
			gPlayers[playerid].setHasCHandling();
		}

		return true;
	}
	return false;
}