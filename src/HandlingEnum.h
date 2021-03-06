#pragma once
#include <cstdint>

enum CHandlingAttrib : uint8_t
{
	HANDL_UIDENTIFIER = 1, // A
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
	HANDL_UIMONETARYVALUE,	// ac
	HANDL_MODELFLAGS,	// af
	HANDL_HANDLINGFLAGS,	// ag
	HANDL_FRONTLIGHTS,	// ah
	HANDL_REARLIGHTS,	// ai
	HANDL_ANIMGROUP	 // aj
};

/* uncomment in pawn
enum CHandlingLights : unsigned char 
{
	LIGHTS_LONG,
	LIGHTS_SMALL,
	LIGHTS_BIG,
	LIGHTS_TALL
};*/

enum CHandlingAttribType
{
	TYPE_NONE,
	TYPE_UINT,
	TYPE_FLOAT,
	TYPE_BYTE,
	TYPE_FLAG
};

CHandlingAttribType GetHandlingAttribType(CHandlingAttrib attribute);
bool CanSetHandlingAttrib(CHandlingAttrib attribute);

bool IsValidHandlingValue(CHandlingAttrib attribute, float value);
bool IsValidHandlingValue(CHandlingAttrib attribute, uint8_t value);
//bool IsValidHandlingValue(CHandlingAttrib attribute, unsigned int value); // not really needed

void* GetHandlingAttribPtr(struct tHandlingData *handling, CHandlingAttrib attrib);