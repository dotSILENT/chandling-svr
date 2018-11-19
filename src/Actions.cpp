#include "Actions.h"
#include "CPlayer.h"
#include "HandlingManager.h"

bool Actions::Process(CHandlingAction id, RakNet::BitStream *bs, int playerid)
{
	switch (id)
	{
	case ACTION_INIT:
		if (IS_VALID_PLAYERID(playerid))
		{
			sampgdk::logprintf("[chandling] Player %d reports having chandling plugin", playerid);
			gPlayers[playerid].setHasCHandling();

			HandlingMgr::OnPlayerConnect(playerid);
		}

		return true;
	}
	return false;
}