#pragma once

#include "CPlayer.h"
#include "Hooks.hpp"
#include "HandlingEnum.h"
#include "HandlingManager.h"
#include "chandlingsvr.h"
#include <cstring>
#include <sdk.hpp>
#include <Server/Components/Pawn/pawn.hpp>
#include <Server/Components/Pawn/Impl/pawn_natives.hpp>

// Vehicle related funcs hooks
using namespace NativeHook;

void RegisterNativeHooks();
void RegisterNativeHooks()
{
	NativeHookManager::Instance().RegisterHookByName("AddStaticVehicle", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked CreateVehicle");
		int vehicleid = orig(amx, params);
		HandlingMgr::OnCreateVehicle(vehicleid);
		CHandlingCompo::VehicleStorage.push_back(vehicleid);
		return static_cast<cell>(vehicleid);
	});

	NativeHookManager::Instance().RegisterHookByName("AddStaticVehicle", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked AddStaticVehicle");
		int vehicleid = orig(amx, params);
		HandlingMgr::OnCreateVehicle(vehicleid);
		CHandlingCompo::VehicleStorage.push_back(vehicleid);
		return static_cast<cell>(vehicleid);
	});

	NativeHookManager::Instance().RegisterHookByName("AddStaticVehicleEx", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked AddStaticVehicleEx");
		int vehicleid = orig(amx, params);
		HandlingMgr::OnCreateVehicle(vehicleid);
		CHandlingCompo::VehicleStorage.push_back(vehicleid);
		return static_cast<cell>(vehicleid);
	});

	NativeHookManager::Instance().RegisterHookByName("DestroyVehicle", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked DestroyVehicle");
		int ret = orig(amx, params);
		int vehicleid = params[1];
		//HandlingMgr::OnDestroyVehicle(vehicleid);
		CHandlingCompo::VehicleStorage.erase(std::remove(CHandlingCompo::VehicleStorage.begin(), CHandlingCompo::VehicleStorage.end(), vehicleid), CHandlingCompo::VehicleStorage.end());
		return static_cast<cell>(ret);
	});
}

// Vehicle handling related funcs
SCRIPT_API(GetHandlingAttribType, cell(int attr))
{
	CHandlingAttrib handlingAttr = static_cast<CHandlingAttrib>(attr);
	CHandlingAttribType type = GetHandlingAttributeType(handlingAttr);
	return static_cast<cell>(type);
}

SCRIPT_API(IsPlayerUsingCHandling, bool(IPlayer &player))
{
	if (&player != nullptr)
	{
		int playerid = player.getID();
		return gPlayers[playerid].hasCHandling();
	}
	return false;
}

SCRIPT_API(ResetModelHandling, bool(int modelid))
{
	return HandlingMgr::ResetModelHandling(modelid);
}

SCRIPT_API(ResetVehicleHandling, bool(IVehicle &vehicle))
{
	HandlingMgr::ResetVehicleHandling(vehicle);
	return true;
}

SCRIPT_API(SetVehicleHandlingFloat, bool(int vehicleid, CHandlingAttrib attrib, float value))
{
	return HandlingMgr::SetVehicleHandling(vehicleid, attrib, value);
}

SCRIPT_API(SetVehicleHandlingInt, bool(int vehicleid, CHandlingAttrib attrib, int value))
{
	if (GetHandlingAttributeType(attrib) == TYPE_BYTE)
		return HandlingMgr::SetVehicleHandling(vehicleid, attrib, (uint8_t)value);

	return HandlingMgr::SetVehicleHandling(vehicleid, attrib, (unsigned int)value);
}

SCRIPT_API(SetModelHandlingFloat, bool(int modelid, CHandlingAttrib attrib, float value))
{
	return HandlingMgr::SetModelHandling((uint16_t)modelid, attrib, value);
}

SCRIPT_API(SetModelHandlingInt, bool(int modelid, CHandlingAttrib attrib, int value))
{
	if (GetHandlingAttributeType(attrib) == TYPE_BYTE)
		return HandlingMgr::SetModelHandling((uint16_t)modelid, attrib, (uint8_t)value);

	return HandlingMgr::SetModelHandling((uint16_t)modelid, attrib, (unsigned int)value);
}

SCRIPT_API(GetVehicleHandlingFloat, bool(int vehicleid, CHandlingAttrib attrib, float &value))
{
    value = 0.0f;
    return HandlingMgr::GetVehicleHandling((uint16_t)vehicleid, attrib, value);
}

SCRIPT_API(GetVehicleHandlingInt, bool(int vehicleid, CHandlingAttrib attrib, unsigned int &value))
{
	value = 0;
	bool ret = false;

	if (GetHandlingAttributeType(attrib) == TYPE_BYTE)
		ret = HandlingMgr::GetVehicleHandling((uint16_t)vehicleid, attrib, value);
	else
		ret = HandlingMgr::GetVehicleHandling((uint16_t)vehicleid, attrib, value);
	return ret;
}

SCRIPT_API(GetModelHandlingFloat, bool(int modelid, CHandlingAttrib attrib, float &value))
{
	value = 0.0f;
	return HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, value);
}

SCRIPT_API(GetModelHandlingInt, bool(int modelid, CHandlingAttrib attrib, unsigned int &value))
{
	value = 0;
	bool ret = false;

	if (GetHandlingAttributeType(attrib) == TYPE_BYTE)
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, value);
	else
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, value);
	return ret;
}

SCRIPT_API(GetDefaultHandlingFloat, bool(int modelid, CHandlingAttrib attrib, float &value))
{
	value = 0.0f;
	return HandlingMgr::GetDefaultHandling((uint16_t)modelid, attrib, value);
}

SCRIPT_API(GetDefaultHandlingInt, bool(int modelid, CHandlingAttrib attrib, unsigned int &value))
{
	bool ret = false;

	if (GetHandlingAttributeType(attrib) == TYPE_BYTE)
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, value);
	else
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, value);
	return ret;
}
