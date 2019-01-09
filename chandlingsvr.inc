/*
 * SA:MP CHandling Server plugin
 * Server-controlled vehicle handling values & per-vehicle handlings
 *
 * Author: .silent
 * 
 * http://github.com/dotSILENT/chandling-svr
 *
*/

#if defined _chandlingsvr_included
#endinput
#endif
#define _chandlingsvr_included

enum CHandlingAttrib
{
	HANDL_IDENTIFIER = 1, // A
	HANDL_FMASS,	// B
	HANDL_FTURNMASS, // c
	HANDL_FDRAGMULTIPLIER, // D
	HANDL_CENTREOFMASS_X,	// F
	HANDL_CENTREOFMASS_Y,	// G
	HANDL_CENTREOFMASS_Z,	// H
	HANDL_NPERCENTSUBMERGED,	// I
	HANDL_FTRACTIONMULTIPLIER,	// J
	HANDL_FTRACTIONLOSS,	// K
	HANDL_FTRACTIONBIAS,	// L
	HANDL_TR_NNUMBEROFGEARS,	// M // transmission data, full name is too long for pawn
	HANDL_TR_FMAXVELOCITY,	// N
	HANDL_TR_FENGINEACCELERATION,	// O
	HANDL_TR_FENGINEINERTIA,	// P
	HANDL_TR_NDRIVETYPE,	// Q
	HANDL_TR_NENGINETYPE,	// R
	HANDL_FBRAKEDECELERATION,	// S
	HANDL_FBRAKEBIAS,	// T
	HANDL_BABS,	// U
	HANDL_FSTEERINGLOCK,	// V
	HANDL_FSUSPENSIONFORCELEVEL,	// a
	HANDL_FSUSPENSIONDAMPINGLEVEL,	// b
	HANDL_FSUSPENSIONHIGHSPDCOMDAMP,	// c
	HANDL_FSUSPENSIONUPPERLIMIT,	// d
	HANDL_FSUSPENSIONLOWERLIMIT,	// e
	HANDL_FSUSPENSIONBIAS,	// f
	HANDL_FSUSPENSIONANTIDIVEMULT,	// g
	HANDL_FSEATOFFSETDISTANCE,	// aa
	HANDL_FCOLLISIONDAMAGEMULT,	// ab
	HANDL_NMONETARYVALUE,	// ac
	HANDL_MODELFLAGS,	// af
	HANDL_HANDLINGFLAGS,	// ag
	HANDL_FRONTLIGHTS,	// ah
	HANDL_REARLIGHTS,	// ai
	HANDL_ANIMGROUP	 // aj
};

/* Handling flags */
enum
{
	HFLAG_1G_BOOST = 0x1,
	HFLAG_2G_BOOST = 0x2,
	HFLAG_NPC_ANTI_ROLL = 0x4,
	HFLAG_NPC_NEUTRAL_HANDL = 0x8,
	HFLAG_NO_HANDBRAKE = 0x10,
	HFLAG_STEER_REARWHEELS = 0x20,
	HFLAG_HB_REARWHEEL_STEER = 0x40,
	HFLAG_ALT_STEER_OPT = 0x80,
	HFLAG_WHEEL_F_NARROW2 = 0x100,
	HFLAG_WHEEL_F_NARROW = 0x200,
	HFLAG_WHEEL_F_WIDE = 0x400,
	HFLAG_WHEEL_F_WIDE2 = 0x800,
	HFLAG_WHEEL_R_NARROW2 = 0x1000,
	HFLAG_WHEEL_R_NARROW = 0x2000,
	HFLAG_WHEEL_R_WIDE = 0x4000,
	HFLAG_WHEEL_R_WIDE2 = 0x8000,
	HFLAG_HYDRAULIC_GEOM = 0x10000,
	HFLAG_HYDRAULIC_INST = 0x20000,
	HFLAG_HYDRAULIC_NONE = 0x40000,
	HFLAG_NOS_INST = 0x80000,
	HFLAG_OFFROAD_ABILITY = 0x100000,
	HFLAG_OFFROAD_ABILITY2 = 0x200000,
	HFLAG_HALOGEN_LIGHTS = 0x400000,
	HFLAG_PROC_REARWHEEL_1ST = 0x800000,
	HFLAG_USE_MAXSP_LIMIT = 0x1000000,
	HFLAG_LOW_RIDER = 0x2000000,
	HFLAG_STREET_RACER = 0x4000000,
	HFLAG_SWINGING_CHASSIS = 0x10000000
};

/* Model flags */
enum
{
	MFLAG_IS_VAN = 0x1,
	MFLAG_IS_BUS = 0x2,
	MFLAG_IS_LOW = 0x4,
	MFLAG_IS_BIG = 0x8,
	MFLAG_REVERSE_BONNET = 0x10,
	MFLAG_HANGING_BOOT = 0x20,
	MFLAG_TAILGATE_BOOT = 0x40,
	MFLAG_NOSWING_BOOT = 0x80,
	MFLAG_NO_DOORS = 0x100,
	MFLAG_TANDEM_SEATS = 0x200,
	MFLAG_SIT_IN_BOAT = 0x400,
	MFLAG_CONVERTIBLE = 0x800,
	MFLAG_NO_EXHAUST = 0x1000,
	MFLAG_DOUBLE_EXHAUST = 0x2000,
	MFLAG_NO1FPS_LOOK_BEHIND = 0x4000,
	MFLAG_FORCE_DOOR_CHECK = 0x8000,
	MFLAG_AXLE_F_NOTILT = 0x10000,
	MFLAG_AXLE_F_SOLID = 0x20000,
	MFLAG_AXLE_F_MCPHERSON = 0x40000,
	MFLAG_AXLE_F_REVERSE = 0x80000,
	MFLAG_AXLE_R_NOTILT = 0x100000,
	MFLAG_AXLE_R_SOLID = 0x200000,
	MFLAG_AXLE_R_MCPHERSON = 0x400000,
	MFLAG_AXLE_R_REVERSE = 0x800000,
	MFLAG_IS_BIKE = 0x1000000,
	MFLAG_IS_HELI = 0x2000000,
	MFLAG_IS_PLANE = 0x4000000,
	MFLAG_IS_BOAT = 0x8000000,
	MFLAG_BOUNCE_PANELS = 0x10000000,
	MFLAG_DOUBLE_RWHEELS = 0x20000000,
	MFLAG_FORCE_GROUND_CLEARANCE = 0x40000000,
	MFLAG_IS_HATCHBACK = 0x80000000
};

/* Utilities for accessing bit flag values */

#define FLAG_IS_ENABLED(%0,%1) (%0 & %1)
#define FLAG_ENABLE(%0,%1) (%0 |= (%1))
#define FLAG_DISABLE(%0,%1) (%0 &= ~(%1))
#define FLAG_FLIP(%0,%1) (%0 ^= %1)


enum CHandlingLights
{
	LIGHTS_LONG,
	LIGHTS_SMALL,
	LIGHTS_BIG,
	LIGHTS_TALL
};

enum CHandlingAttribType
{
	TYPE_NONE,
	TYPE_UINT,
	TYPE_FLOAT,
	TYPE_BYTE,
	TYPE_FLAG
};

/* Natives list */
native CHandlingAttribType:GetHandlingAttribType(CHandlingAttrib:attribute);

native IsPlayerUsingCHandling(playerid);
native ResetModelHandling(modelid);
native ResetVehicleHandling(vehicleid);
native SetVehicleHandlingFloat(vehicleid, CHandlingAttrib:attribute, Float:value);
native SetVehicleHandlingInt(vehicleid, CHandlingAttrib:attribute, value);

native SetModelHandlingFloat(modelid, CHandlingAttrib:attribute, Float:value);
native SetModelHandlingInt(modelid, CHandlingAttrib:attribute, value);

native GetVehicleHandlingFloat(vehicleid, CHandlingAttrib:attribute, &Float:result);
native GetVehicleHandlingInt(vehicleid, CHandlingAttrib:attribute, &result);

native GetModelHandlingFloat(modelid, CHandlingAttrib:attribute, &Float:result);
native GetModelHandlingInt(modelid, CHandlingAttrib:attribute, &result);

native GetDefaultHandlingFloat(modelid, CHandlingAttrib:attribute, &Float:value);
native GetDefaultHandlingInt(modelid, CHandlingAttrib:attribute, &value);
