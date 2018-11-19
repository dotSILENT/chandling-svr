#pragma once
#include <cstdint>
/* game structures */

struct  tTransmissionGear
{
	float m_fMaxVelocity;
	float m_fChangeUpVelocity;
	float m_fChangeDownVelocity;
};

class cTransmission {
public:
	tTransmissionGear m_aGears[6];
	unsigned char m_nDriveType; // F/R/4
	unsigned char m_nEngineType; // P/D/E
	unsigned char m_nNumberOfGears; // 1 to 6
	char field_4B;
	unsigned int  m_nHandlingFlags;
	float         m_fEngineAcceleration; // 0.1 to 10.0
	float         m_fEngineInertia; // 0.0 to 50.0
	float         m_fMaxGearVelocity; // 5.0 to 150.0
	int field_5C;
	float         m_fMinGearVelocity;
	float         m_fCurrentSpeed;

	/*cTransmission();
	void InitGearRatios();
	void CalculateGearForSimpleCar(float velocity, unsigned char& currrentGear);
	// it uses printf, so you won't see it actually...
	void DisplayGearRatios();
	float CalculateDriveAcceleration(float& gasPedal, unsigned char& currrentGear, float& gearChangeCount, float& speed, float& unk1, float& unk2, bool allWheelsOnGround, unsigned char handlingType);
	*/
};

enum eVehicleLightsSize : unsigned char {
	LIGHTS_LONG,
	LIGHTS_SMALL,
	LIGHTS_BIG,
	LIGHTS_TALL
};

enum eVehicleHandlingFlags : unsigned int {
	VEHICLE_HANDLING_1G_BOOST = 0x1,
	VEHICLE_HANDLING_2G_BOOST = 0x2,
	VEHICLE_HANDLING_NPC_ANTI_ROLL = 0x4,
	VEHICLE_HANDLING_NPC_NEUTRAL_HANDL = 0x8,
	VEHICLE_HANDLING_NO_HANDBRAKE = 0x10,
	VEHICLE_HANDLING_STEER_REARWHEELS = 0x20,
	VEHICLE_HANDLING_HB_REARWHEEL_STEER = 0x40,
	VEHICLE_HANDLING_ALT_STEER_OPT = 0x80,
	VEHICLE_HANDLING_WHEEL_F_NARROW2 = 0x100,
	VEHICLE_HANDLING_WHEEL_F_NARROW = 0x200,
	VEHICLE_HANDLING_WHEEL_F_WIDE = 0x400,
	VEHICLE_HANDLING_WHEEL_F_WIDE2 = 0x800,
	VEHICLE_HANDLING_WHEEL_R_NARROW2 = 0x1000,
	VEHICLE_HANDLING_WHEEL_R_NARROW = 0x2000,
	VEHICLE_HANDLING_WHEEL_R_WIDE = 0x4000,
	VEHICLE_HANDLING_WHEEL_R_WIDE2 = 0x8000,
	VEHICLE_HANDLING_HYDRAULIC_GEOM = 0x10000,
	VEHICLE_HANDLING_HYDRAULIC_INST = 0x20000,
	VEHICLE_HANDLING_HYDRAULIC_NONE = 0x40000,
	VEHICLE_HANDLING_NOS_INST = 0x80000,
	VEHICLE_HANDLING_OFFROAD_ABILITY = 0x100000,
	VEHICLE_HANDLING_OFFROAD_ABILITY2 = 0x200000,
	VEHICLE_HANDLING_HALOGEN_LIGHTS = 0x400000,
	VEHICLE_HANDLING_PROC_REARWHEEL_1ST = 0x800000,
	VEHICLE_HANDLING_USE_MAXSP_LIMIT = 0x1000000,
	VEHICLE_HANDLING_LOW_RIDER = 0x2000000,
	VEHICLE_HANDLING_STREET_RACER = 0x4000000,
	VEHICLE_HANDLING_SWINGING_CHASSIS = 0x10000000
};

enum eVehicleHandlingModelFlags : unsigned int {
	VEHICLE_HANDLING_MODEL_IS_VAN = 0x1,
	VEHICLE_HANDLING_MODEL_IS_BUS = 0x2,
	VEHICLE_HANDLING_MODEL_IS_LOW = 0x4,
	VEHICLE_HANDLING_MODEL_IS_BIG = 0x8,
	VEHICLE_HANDLING_MODEL_REVERSE_BONNET = 0x10,
	VEHICLE_HANDLING_MODEL_HANGING_BOOT = 0x20,
	VEHICLE_HANDLING_MODEL_TAILGATE_BOOT = 0x40,
	VEHICLE_HANDLING_MODEL_NOSWING_BOOT = 0x80,
	VEHICLE_HANDLING_MODEL_NO_DOORS = 0x100,
	VEHICLE_HANDLING_MODEL_TANDEM_SEATS = 0x200,
	VEHICLE_HANDLING_MODEL_SIT_IN_BOAT = 0x400,
	VEHICLE_HANDLING_MODEL_CONVERTIBLE = 0x800,
	VEHICLE_HANDLING_MODEL_NO_EXHAUST = 0x1000,
	VEHICLE_HANDLING_MODEL_DOUBLE_EXHAUST = 0x2000,
	VEHICLE_HANDLING_MODEL_NO1FPS_LOOK_BEHIND = 0x4000,
	VEHICLE_HANDLING_MODEL_FORCE_DOOR_CHECK = 0x8000,
	VEHICLE_HANDLING_MODEL_AXLE_F_NOTILT = 0x10000,
	VEHICLE_HANDLING_MODEL_AXLE_F_SOLID = 0x20000,
	VEHICLE_HANDLING_MODEL_AXLE_F_MCPHERSON = 0x40000,
	VEHICLE_HANDLING_MODEL_AXLE_F_REVERSE = 0x80000,
	VEHICLE_HANDLING_MODEL_AXLE_R_NOTILT = 0x100000,
	VEHICLE_HANDLING_MODEL_AXLE_R_SOLID = 0x200000,
	VEHICLE_HANDLING_MODEL_AXLE_R_MCPHERSON = 0x400000,
	VEHICLE_HANDLING_MODEL_AXLE_R_REVERSE = 0x800000,
	VEHICLE_HANDLING_MODEL_IS_BIKE = 0x1000000,
	VEHICLE_HANDLING_MODEL_IS_HELI = 0x2000000,
	VEHICLE_HANDLING_MODEL_IS_PLANE = 0x4000000,
	VEHICLE_HANDLING_MODEL_IS_BOAT = 0x8000000,
	VEHICLE_HANDLING_MODEL_BOUNCE_PANELS = 0x10000000,
	VEHICLE_HANDLING_MODEL_DOUBLE_RWHEELS = 0x20000000,
	VEHICLE_HANDLING_MODEL_FORCE_GROUND_CLEARANCE = 0x40000000,
	VEHICLE_HANDLING_MODEL_IS_HATCHBACK = 0x80000000
};

struct CVector
{
	float X;
	float Y;
	float Z;
};

struct tHandlingData {
	int           m_iIndex;
	float         m_fMass; // 1.0 to 50000.0
	float field_8;
	float         m_fTurnMass;
	float         m_fDragMult;
	struct CVector       m_vecCentreOfMass; // x, y, z - 1.0 to 50000.0
	uint8_t m_nPercentSubmerged; // 10 to 120 (> 100% vehicle sinks)
	float         m_fBuoyancyConstant;
	float         m_fTractionMultiplier; // 0.5 to 2.0
	cTransmission m_transmissionData;
	float         m_fBrakeDeceleration; // 0.1 to 10.0
	float         m_fBrakeBias; // 0.0 > x > 1.0
	char          m_bABS; // 0/1
	char field_9D;
	char field_9E;
	char field_9F;
	float         m_fSteeringLock; // 10.0 to 40.0
	float         m_fTractionLoss;
	float         m_fTractionBias;
	float         m_fSuspensionForceLevel; // not [L/M/H]
	float         m_fSuspensionDampingLevel; // not [L/M/H]
	float         m_fSuspensionHighSpdComDamp; // often zero - 200.0 or more for bouncy vehicles
	float         m_fSuspensionUpperLimit;
	float         m_fSuspensionLowerLimit;
	float         m_fSuspensionBiasBetweenFrontAndRear;
	float         m_fSuspensionAntiDiveMultiplier;
	float         m_fCollisionDamageMultiplier; // 0.2 to 5.0
	union {
		eVehicleHandlingModelFlags m_nModelFlags;
		struct {
			unsigned int m_bIsVan : 1;
			unsigned int m_bIsBus : 1;
			unsigned int m_bIsLow : 1;
			unsigned int m_bIsBig : 1;
			unsigned int m_bReverseBonnet : 1;
			unsigned int m_bHangingBoot : 1;
			unsigned int m_bTailgateBoot : 1;
			unsigned int m_bNoswingBoot : 1;
			unsigned int m_bNoDoors : 1;
			unsigned int m_bTandemSeats : 1;
			unsigned int m_bSitInBoat : 1;
			unsigned int m_bConvertible : 1;
			unsigned int m_bNoExhaust : 1;
			unsigned int m_bDoubleExhaust : 1;
			unsigned int m_bNo1fpsLookBehind : 1;
			unsigned int m_bForceDoorCheck : 1;
			unsigned int m_bAxleFNotlit : 1;
			unsigned int m_bAxleFSolid : 1;
			unsigned int m_bAxleFMcpherson : 1;
			unsigned int m_bAxleFReverse : 1;
			unsigned int m_bAxleRNotlit : 1;
			unsigned int m_bAxleRSolid : 1;
			unsigned int m_bAxleRMcpherson : 1;
			unsigned int m_bAxleRReverse : 1;
			unsigned int m_bIsBike : 1;
			unsigned int m_bIsHeli : 1;
			unsigned int m_bIsPlane : 1;
			unsigned int m_bIsBoat : 1;
			unsigned int m_bBouncePanels : 1;
			unsigned int m_bDoubleRwheels : 1;
			unsigned int m_bForceGroundClearance : 1;
			unsigned int m_bIsHatchback : 1;
		};
	};
	union {
		eVehicleHandlingFlags m_nHandlingFlags;
		struct {
			unsigned int m_b1gBoost : 1;
			unsigned int m_b2gBoost : 1;
			unsigned int m_bNpcAntiRoll : 1;
			unsigned int m_bNpcNeutralHandl : 1;
			unsigned int m_bNoHandbrake : 1;
			unsigned int m_bSteerRearwheels : 1;
			unsigned int m_bHbRearwheelSteer : 1;
			unsigned int m_bAltSteerOpt : 1;
			unsigned int m_bWheelFNarrow2 : 1;
			unsigned int m_bWheelFNarrow : 1;
			unsigned int m_bWheelFWide : 1;
			unsigned int m_bWheelFWide2 : 1;
			unsigned int m_bWheelRNarrow2 : 1;
			unsigned int m_bWheelRNarrow : 1;
			unsigned int m_bWheelRWide : 1;
			unsigned int m_bWheelRWide2 : 1;
			unsigned int m_bHydraulicGeom : 1;
			unsigned int m_bHydraulicInst : 1;
			unsigned int m_bHydraulicNone : 1;
			unsigned int m_bNosInst : 1;
			unsigned int m_bOffroadAbility : 1;
			unsigned int m_bOffroadAbility2 : 1;
			unsigned int m_bHalogenLights : 1;
			unsigned int m_bProcRearwheelFirst : 1;
			unsigned int m_bUseMaxspLimit : 1;
			unsigned int m_bLowRider : 1;
			unsigned int m_bStreetRacer : 1;
			unsigned int m_bSwingingChassis : 1;
		};
	};
	float              m_fSeatOffsetDistance; // // ped seat position offset towards centre of car
	unsigned int       m_nMonetaryValue; // 1 to 100000
	eVehicleLightsSize m_nFrontLights;
	eVehicleLightsSize m_nRearLights;
	unsigned char      m_nAnimGroup;
};