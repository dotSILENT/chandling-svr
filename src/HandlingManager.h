#pragma once
#include "HandlingEnum.h"
#include "HandlingStruct.h"


namespace HandlingMgr
{
	void	ProcessTick();

	void	InitializeModelHandlings();
	void	OnCreateVehicle(int vehicleid); // needs to be called after pawn's CreateVehicle is called in order to reset the data
	void	OnPlayerConnect(int playerid); // call this from OnPlayerConnect (or rather from ACTION_INIT handler) so model handling modifications are sent to the player
	void	OnVehicleStreamIn(int vehicleid, int forplayerid); // call from OnVehicleStreamIn so handling modifications for this individual vehicle are sent to the player

	bool	ResetModelHandling(int modelid); // resets model handling to it's default one, NOTE: this resets any handling modifications for every vehicle of that model
	bool	ResetVehicleHandling(int vehicleid, bool sendToPlayers=true); // resets vehicle handling to it's model handling (and clears the modifications)
	bool	SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, float value);
	bool	SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, unsigned int value);
	bool	SetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, uint8_t value);

	bool	SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, float value);
	bool	SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int value);
	bool	SetModelHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t value);

	bool	GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, float &ret);
	bool	GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, unsigned int &ret);
	bool	GetVehicleHandling(uint16_t vehicleid, CHandlingAttrib attrib, uint8_t &ret);
	
	bool	GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, float &ret);
	bool	GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int &ret);
	bool	GetModelHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t &ret);

	bool	GetDefaultHandling(uint16_t modelid, CHandlingAttrib attrib, float &ret);
	bool	GetDefaultHandling(uint16_t modelid, CHandlingAttrib attrib, unsigned int &ret);
	bool	GetDefaultHandling(uint16_t modelid, CHandlingAttrib attrib, uint8_t &ret);
}