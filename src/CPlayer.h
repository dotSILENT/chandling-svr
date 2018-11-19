#pragma once

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 1000
#endif

#define IS_VALID_PLAYERID(id) \
	(id >= 0 && id < MAX_PLAYERS)

class CPlayer
{
private:
	bool _hasCHandling = false;

public:
	bool hasCHandling() { return this->_hasCHandling; };

	void setHasCHandling() { this->_hasCHandling = true; };

	void Reset();
};

extern CPlayer gPlayers[];