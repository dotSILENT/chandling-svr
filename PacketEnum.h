// CHandling packets

enum CHandlingPacket
{
	ID_CHANDLING = 251 // one packet to rule them all, we don't want to use any ID occupied by samp so let's keep it minimal
};

enum CHandlingAction
{
	ACTION_PLAYER_JOIN = 10, // This is the only packet that is sent  by the player, to indicate that we can speak to him
	ACTION__SET_VEHICLE_HANDLING = 15,
	ACTION_SET_MODEL_HANDLING = 20
};