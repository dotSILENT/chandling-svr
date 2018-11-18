#include "CPlayer.h"

CPlayer gPlayers[MAX_PLAYERS];


void CPlayer::Reset()
{
	this->_hasCHandling = false;
}

