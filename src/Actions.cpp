#include "Actions.h"
#include "sampgdk/sampgdk.h"
#include "CPlayer.h"
#include "HandlingManager.h"
#include "chandlingsvr.h"

bool Actions::Process(CHandlingAction id, RakNet::BitStream *bs, int playerid)
{
	switch (id)
	{
	case ACTION_INIT:
		if (IS_VALID_PLAYERID(playerid))
		{
			uint32_t compat_ver;
			bs->Read(compat_ver);

			CHandlingActionPacket pkt(ACTION_INIT_RESPONSE);
			pkt.data.Write((uint32_t)CHANDLING_COMPAT_VERSION);
			
			if (compat_ver >= CHANDLING_COMPAT_VERSION)
			{
				pkt.data.Write(true);
				gPlayers[playerid].setHasCHandling();
				sampgdk::logprintf("[chandling] Player %d reports having chandling plugin", playerid);
			}
			else pkt.data.Write(false);
			
			pRakServer->Send(&pkt.data, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerid), false);
		}

		return true;
	}
	return false;
}