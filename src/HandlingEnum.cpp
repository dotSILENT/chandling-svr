#include "HandlingEnum.h"

CHandlingAttribType GetHandlingAttribType(CHandlingAttrib attribute)
{
	switch (attribute)
	{
	case HANDL_FMASS:
	case HANDL_FDRAGMULTIPLIER:
	case HANDL_CENTREOFMASS_X:
	case HANDL_CENTREOFMASS_Y:
	case HANDL_CENTREOFMASS_Z:
	case HANDL_FTRACTIONMULTIPLIER:
	case HANDL_FTRACTIONLOSS:
	case HANDL_FTRACTIONBIAS:
	case HANDL_TR_FMAXVELOCITY:
	case HANDL_TR_FENGINEACCELERATION:
	case HANDL_TR_FENGINEINERTIA:
	case HANDL_FBRAKEDECELERATION:
	case HANDL_FBRAKEBIAS:
	case HANDL_FSTEERINGLOCK:
	case HANDL_FSUSPENSIONFORCELEVEL:
	case HANDL_FSUSPENSIONDAMPINGLEVEL:
	case HANDL_FSUSPENSIONHIGHSPDCOMDAMP:
	case HANDL_FSUSPENSIONUPPERLIMIT:
	case HANDL_FSUSPENSIONLOWERLIMIT:
	case HANDL_FSUSPENSIONBIAS:
	case HANDL_FSUSPENSIONANTIDIVEMULT:
	case HANDL_FSEATOFFSETDISTANCE:
	case HANDL_FCOLLISIONDAMAGEMULT:
		return TYPE_FLOAT;

	case HANDL_NPERCENTSUBMERGED:
	case HANDL_ANIMGROUP:
	case HANDL_FRONTLIGHTS:
	case HANDL_REARLIGHTS:
	case HANDL_TR_NDRIVETYPE:
	case HANDL_TR_NENGINETYPE:
	case HANDL_TR_NNUMBEROFGEARS:
		return TYPE_BYTE;

	case HANDL_IDENTIFIER:
	case HANDL_IMONETARYVALUE:
		return TYPE_INT;

	case HANDL_HANDLINGFLAGS:
	case HANDL_MODELFLAGS:
		return TYPE_FLAG;
	}
	return TYPE_NONE;
}