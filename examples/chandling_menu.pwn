#define FILTERSCRIPT

#include <a_samp>
#include <chandlingsvr>

#tryinclude <Pawn.CMD>

#if defined PAWNCMD_INC_
#define ZCMD
#else
#tryinclude <zcmd>
#endif

#if defined _zcmd_included
#define ZCMD
#endif

enum
{
	DIALOG_MAIN = 28333,
	DIALOG_SETVAL
};

static fmt[1256];

public OnFilterScriptInit()
{
	print("[###] CHandling example filterscript loaded [###]");

	return 1;
}

public OnPlayerConnect(playerid)
{
	if(IsPlayerUsingCHandling(playerid))
		printf("[cHandling] Player id %0 is using CHandling plugin");
	return 1;
}

ShowCHandlingList(playerid)
{
	ShowPlayerDialog(playerid, DIALOG_MAIN, DIALOG_STYLE_LIST, "CHandling",\
		"HANDL_FMASS\n\
		HANDL_FTURNMASS\n\
		HANDL_FDRAGMULTIPLIER\n\
		HANDL_COM_X\n\
		HANDL_COM_Y\n\
		HANDL_COM_Z\n\
		HANDL_NPERCENTSUBMERG\n\
		HANDL_FTRACTIONMULTIP\n\
		HANDL_FTRACTIONLOSS\n\
		HANDL_FTRACTIONBIAS\n\
		HANDL_TR_NUMOFGEARS\n\
		HANDL_TR_FMAXVELOCITY\n\
		HANDL_TR_FENGINEACCEL\n\
		HANDL_TR_FENGINEINERTIA\n\
		HANDL_TR_NDRIVETYPE\n\
		HANDL_TR_NENGINETYPE\n\
		HANDL_FBRAKEDECELERATIO\n\
		HANDL_FBRAKEBIAS\n\
		HANDL_BABS\n\
		HANDL_FSTEERINGLOCK\n\
		HANDL_FSUSPENSIONFORCELVL\n\
		HANDL_FSUSPENSIONDAMPINGLVL\n\
		HANDL_FSUSPENSIONHSPDCMPDAMP\n\
		HANDL_FSUSPENSIONUPPERLIMIT\n\
		HANDL_FSUSPENSIONLOWERLIMIT\n\
		HANDL_FSUSPENSIONBIAS\n\
		HANDL_FSUSPENSIONANTIDIVEMUL\n\
		HANDL_FSEATOFFSETDISTANCE\n\
		HANDL_FCOLLISIONDAMAGEMULT\n\
		HANDL_NMONETARYVALUE\n\
		HANDL_MODELFLAGS\n\
		HANDL_HANDLINGFLAGS\n\
		HANDL_FRONTLIGHTS\n\
		HANDL_REARLIGHTS\n\
		HANDL_ANIMGROUP", "Select", "Cancel");
}

#if defined ZCMD
CMD:chandling(playerid)
{
	if(!IsPlayerUsingCHandling(playerid))
		return 0;

	if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER || !IsPlayerInAnyVehicle(playerid))
			return SendClientMessage(playerid, -1, "You must be driving a vehicle to use CHandling");

	ShowCHandlingList(playerid);
	return 1;
}

#else

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(IsPlayerUsingCHandling(playerid) && !strcmp(cmdtext, "/chandling"))
	{
		if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER || !IsPlayerInAnyVehicle(playerid))
			return SendClientMessage(playerid, -1, "You must be driving a vehicle to use CHandling");

		ShowCHandlingList(playerid);
		return 1;
	}
	return 0;
}
#endif

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if(dialogid == DIALOG_MAIN && IsPlayerUsingCHandling(playerid))
	{
		if(!response)
			return 1;

		new vid = GetPlayerVehicleID(playerid);
		if(!vid)
			return SendClientMessage(playerid, -1, "You must be driving a vehicle to use /chandling");

		new CHandlingAttrib:attrib = CHandlingAttrib:(listitem+2); // +2 because we ignore 0 and HANDL_IDENTIFIER

		new Float:fval, ival;

		switch(GetHandlingAttribType(attrib))
		{
			case TYPE_FLOAT:
			{
				GetVehicleHandlingFloat(vid, attrib, fval);
				format(fmt, sizeof fmt, "Current value (vehicle %d, model %d): %f", vid, GetVehicleModel(vid), fval);
			}
			case TYPE_UINT, TYPE_BYTE:
			{
				GetVehicleHandlingInt(vid, attrib, ival);
				if(attrib == HANDL_TR_NDRIVETYPE || attrib == HANDL_TR_NENGINETYPE)
					format(fmt, sizeof fmt, "Current value (vehicle %d, model %d): %c", vid, GetVehicleModel(vid), ival);
				else
					format(fmt, sizeof fmt, "Current value (vehicle %d, model %d): %d", vid, GetVehicleModel(vid), ival);
			}
			case TYPE_FLAG:
			{
				// show flag dialog
				GetVehicleHandlingInt(vid, attrib, ival);
				#define ADD_FLAG(%0) format(fmt, sizeof fmt, "%s%d {%06x}" #%0 "\n", fmt, %0, FLAG_IS_ENABLED(ival, %0) ? 0x00FF00 : 0xFF0000)
				if(attrib == HANDL_HANDLINGFLAGS)
				{
					format(fmt, sizeof fmt, "%d {%06x}HFLAG_1G_BOOST\n", HFLAG_1G_BOOST, FLAG_IS_ENABLED(ival, HFLAG_1G_BOOST) ? 0x00FF00 : 0xFF0000);
					ADD_FLAG(HFLAG_2G_BOOST);
					ADD_FLAG(HFLAG_NPC_ANTI_ROLL);
					ADD_FLAG(HFLAG_NPC_NEUTRAL_HANDL);
					ADD_FLAG(HFLAG_NO_HANDBRAKE);
					ADD_FLAG(HFLAG_STEER_REARWHEELS);
					ADD_FLAG(HFLAG_HB_REARWHEEL_STEER);
					ADD_FLAG(HFLAG_ALT_STEER_OPT);
					ADD_FLAG(HFLAG_WHEEL_F_NARROW2);
					ADD_FLAG(HFLAG_WHEEL_F_NARROW);
					ADD_FLAG(HFLAG_WHEEL_F_WIDE);
					ADD_FLAG(HFLAG_WHEEL_F_WIDE2);
					ADD_FLAG(HFLAG_WHEEL_R_NARROW2);
					ADD_FLAG(HFLAG_WHEEL_R_NARROW);
					ADD_FLAG(HFLAG_WHEEL_R_WIDE);
					ADD_FLAG(HFLAG_WHEEL_R_WIDE2);
					ADD_FLAG(HFLAG_HYDRAULIC_GEOM);
					ADD_FLAG(HFLAG_HYDRAULIC_INST);
					ADD_FLAG(HFLAG_HYDRAULIC_NONE);
					ADD_FLAG(HFLAG_NOS_INST);
					ADD_FLAG(HFLAG_OFFROAD_ABILITY);
					ADD_FLAG(HFLAG_OFFROAD_ABILITY2);
					ADD_FLAG(HFLAG_HALOGEN_LIGHTS);
					ADD_FLAG(HFLAG_PROC_REARWHEEL_1ST);
					ADD_FLAG(HFLAG_USE_MAXSP_LIMIT);
					ADD_FLAG(HFLAG_LOW_RIDER);
					ADD_FLAG(HFLAG_STREET_RACER);
					ADD_FLAG(HFLAG_SWINGING_CHASSIS);
				}
				else if(attrib == HANDL_MODELFLAGS)
				{
					format(fmt, sizeof fmt, "%d {%06x}MFLAG_IS_VAN\n", MFLAG_IS_VAN, FLAG_IS_ENABLED(ival, MFLAG_IS_VAN) ? 0x00FF00 : 0xFF0000);
					ADD_FLAG(MFLAG_IS_BUS);
					ADD_FLAG(MFLAG_IS_LOW);
					ADD_FLAG(MFLAG_IS_BIG);
					ADD_FLAG(MFLAG_REVERSE_BONNET);
					ADD_FLAG(MFLAG_HANGING_BOOT);
					ADD_FLAG(MFLAG_TAILGATE_BOOT);
					ADD_FLAG(MFLAG_NOSWING_BOOT);
					ADD_FLAG(MFLAG_NO_DOORS);
					ADD_FLAG(MFLAG_TANDEM_SEATS);
					ADD_FLAG(MFLAG_SIT_IN_BOAT);
					ADD_FLAG(MFLAG_CONVERTIBLE);
					ADD_FLAG(MFLAG_NO_EXHAUST);
					ADD_FLAG(MFLAG_DOUBLE_EXHAUST);
					ADD_FLAG(MFLAG_NO1FPS_LOOK_BEHIND);
					ADD_FLAG(MFLAG_FORCE_DOOR_CHECK);
					ADD_FLAG(MFLAG_AXLE_F_NOTILT);
					ADD_FLAG(MFLAG_AXLE_F_SOLID);
					ADD_FLAG(MFLAG_AXLE_F_MCPHERSON);
					ADD_FLAG(MFLAG_AXLE_F_REVERSE);
					ADD_FLAG(MFLAG_AXLE_R_NOTILT);
					ADD_FLAG(MFLAG_AXLE_R_SOLID);
					ADD_FLAG(MFLAG_AXLE_R_MCPHERSON);
					ADD_FLAG(MFLAG_AXLE_R_REVERSE);
					ADD_FLAG(MFLAG_IS_BIKE);
					ADD_FLAG(MFLAG_IS_HELI);
					ADD_FLAG(MFLAG_IS_PLANE);
					ADD_FLAG(MFLAG_IS_BOAT);
					ADD_FLAG(MFLAG_BOUNCE_PANELS);
					ADD_FLAG(MFLAG_DOUBLE_RWHEELS);
					ADD_FLAG(MFLAG_FORCE_GROUND_CLEARANCE);
					// format cant handle intmax unless in hex
					format(fmt, sizeof fmt, "%s2147483648 {%06x}MFLAG_IS_HATCHBACK\n", fmt, FLAG_IS_ENABLED(ival, MFLAG_IS_HATCHBACK) ? 0x00FF00 : 0xFF0000);
				} else return SendClientMessage(playerid, -1, "Invalid flag attrib");

				ShowPlayerDialog(playerid, DIALOG_SETVAL, DIALOG_STYLE_LIST, "CHandling flags", fmt, "Flip", "Cancel");
				SetPVarInt(playerid, "chandlattrib", _:attrib);
				#undef ADD_FLAG
				return 1;
			}
			case TYPE_NONE:
			{
				SendClientMessage(playerid, -1, "There was some problem with the dialog (attrib unknown)");
				return 1;
			}
		}

		SetPVarInt(playerid, "chandlattrib", _:attrib);

		ShowPlayerDialog(playerid, DIALOG_SETVAL, DIALOG_STYLE_INPUT, "CHandling->Set value", fmt, "Ok", "Cancel");
		return 1;
	}
	else if(dialogid == DIALOG_SETVAL && IsPlayerUsingCHandling(playerid))
	{
		if(!response)
		{
			ShowCHandlingList(playerid);
			return 1;
		}

		new vid = GetPlayerVehicleID(playerid);
		if(!vid)
			return SendClientMessage(playerid, -1, "You must be driving a vehicle");

		new CHandlingAttrib:attrib = CHandlingAttrib:GetPVarInt(playerid, "chandlattrib");

		new Float:fval, ival;

		switch(GetHandlingAttribType(attrib))
		{
			case TYPE_FLOAT:
			{
				fval = floatstr(inputtext);

				if(!SetVehicleHandlingFloat(vid, attrib, fval))
					return SendClientMessage(playerid, -1, "Failed to set handling value, check if it's correct");
			}
			case TYPE_UINT, TYPE_BYTE:
			{
				ival = strval(inputtext);

				if(attrib == HANDL_TR_NDRIVETYPE || attrib == HANDL_TR_NENGINETYPE)
					ival = inputtext[0];

				if(!SetVehicleHandlingInt(vid, attrib, ival))
					return SendClientMessage(playerid, -1, "Failed to set handling value, check if it's correct");
			}
			case TYPE_FLAG:
			{
				GetVehicleHandlingInt(vid, attrib, ival);
				new flag = strval(inputtext);
				FLAG_FLIP(ival, flag);

				if(!SetVehicleHandlingInt(vid, attrib, ival))
					return SendClientMessage(playerid, -1, "Failed to set handling value, check if it's correct");
			}
			case TYPE_NONE:
			{
				SendClientMessage(playerid, -1, "There was some problem with the dialog (attrib unknown)");
			}
		}
		return 1;
	}
	return 0;
}