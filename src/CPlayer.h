#pragma once

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 1000
#endif

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
