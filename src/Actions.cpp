#include "Actions.h"
#include "CPlayer.h"
#include "HandlingManager.h"
#include "chandlingsvr.h"

bool Actions::Process(CHandlingAction id, NetworkBitStream &bs, IPlayer &player)
{
	switch (id)
	{
	case ACTION_INIT:
	{
		uint32_t compat_ver;
		bs.Read(compat_ver);

		CHandlingActionPacket pkt(ACTION_INIT_RESPONSE);
		pkt.data.Write((uint32_t)CHANDLING_COMPAT_VERSION);

		int playerid = player.getID();
		if (compat_ver >= CHANDLING_COMPAT_VERSION)
		{
			pkt.data.Write(true);
			gPlayers[playerid].setHasCHandling();
			auto core_ = CHandlingCompo::getCore();
			if (!core_)
			{
				return false;
			}
			core_->logLn(LogLevel::Message, "[CHandling] Player %.*d reports having chandling plugin", playerid);
		}
		else
			pkt.data.Write(false);

		player.sendPacket(Span<uint8_t>(pkt.data.GetData(), pkt.data.GetNumberOfBitsUsed()), 0, false);

		return true;
	}
	}
	return false;
}
