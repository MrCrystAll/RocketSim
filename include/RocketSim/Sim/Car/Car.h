#pragma once

#include <RocketSim/Sim/PhysState/PhysState.h>
#include <RocketSim/Sim/Car/CarConfig/CarConfig.h>
#include <RocketSim/Sim/btVehicleRL/btVehicleRL.h>
#include <RocketSim/Sim/CarControls.h>
#include <RocketSim/Sim/BallHitInfo/BallHitInfo.h>
#include <RocketSim/Sim/MutatorConfig/MutatorConfig.h>

#include <bullet3-3.24/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet3-3.24/BulletDynamics/Vehicle/btDefaultVehicleRaycaster.h>
#include <bullet3-3.24/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet3-3.24/BulletCollision/CollisionShapes/btCompoundShape.h>
#include <RocketSim/Sim/btVehicleRL/btVehicleRL.h>


RS_NS_START

struct RS_API CarState : public PhysState {

	// Incremented every update, reset when SetState() is called
	// Used for telling if a stateset occured
	// Not serialized
	uint64_t tickCountSinceUpdate = 0;

	// True if 3 or more wheels have contact
	bool isOnGround = true;

	// Whether each of the 4 wheels have contact
	// First two are front
	// If your car has 3 wheels, the 4th bool will always be false
	bool wheelsWithContact[4] = {}; 

	// Whether we jumped to get into the air
	// Can be false while airborne, if we left the ground with a flip reset
	bool hasJumped = false;

	// True if we have double jumped and are still in the air
	// NOTE: Flips DO NOT COUNT as double jumps! This is not RLBot.
	bool hasDoubleJumped = false;

	// True if we are in the air, and (have flipped or are currently flipping)
	bool hasFlipped = false;

	// Relative torque direction of the flip
	// Forward flip will have positive Y
	Vec flipRelTorque = { 0, 0, 0 };

	float jumpTime = 0; // When currently jumping, the time since we started jumping, else 0
	float flipTime = 0; // When currently flipping, the time since we started flipping, else 0

	// True during a flip (not an auto-flip, and not after a flip)
	bool isFlipping = false;

	// True during a jump
	bool isJumping = false;

	// Total time spent in the air
	float airTime = 0;

	// Time spent in the air once !isJumping
	// If we never jumped, it is 0
	float airTimeSinceJump = 0;

	// Goes from 0 to 100
	float boost = RLConst::BOOST_SPAWN_AMOUNT;

	// Used for recharge boost, counts up from 0 on spawn (in seconds)
	float timeSinceBoosted = 0.f;

	// True if we boosted that tick
	// There exists a minimum boosting time, thus why we must track boosting time
	bool isBoosting = false;
	float boostingTime = 0;

	bool isSupersonic = false;

	// Time spent supersonic, for checking with the supersonic maintain time (see RLConst.h)
	float supersonicTime = 0;

	// This is a state variable due to the rise/fall rate of handbrake inputs (see RLConst.h)
	float handbrakeVal = 0;

	bool isAutoFlipping = false;
	float autoFlipTimer = 0; // Counts down when auto-flipping
	float autoFlipTorqueScale = 0;

	struct {
		bool hasContact = false;
		Vec contactNormal;
	} worldContact;

	struct {
		uint32_t otherCarID = 0;
		float cooldownTimer = 0;
	} carContact;

	bool isDemoed = false;
	float demoRespawnTimer = 0;

	BallHitInfo ballHitInfo = BallHitInfo();

	// Controls from last tick, set to this->controls after simulation
	CarControls lastControls = CarControls();

	CarState() : PhysState() {
		pos.z = RLConst::CAR_SPAWN_REST_Z;
	}

	// Returns true if the car is currently able to jump, double-jump, or flip
	bool HasFlipOrJump() const;

	// Returns true if we currently have a flip, and got it from a flip reset (instead of a jump)
	bool HasFlipReset() const;

	// Returns true if we are in the air, and became airborne from a flip reset
	// This returns true regardless of whether or not we still have the flip
	bool GotFlipReset() const;

	void Serialize(DataStreamOut& out) const;
	void Deserialize(DataStreamIn& in);
};

#define CARSTATE_SERIALIZATION_FIELDS \
pos, rotMat, vel, angVel, isOnGround, hasJumped, hasDoubleJumped, hasFlipped, \
flipRelTorque, jumpTime, isFlipping, flipTime, isJumping, airTimeSinceJump, \
boost, isBoosting, boostingTime, timeSinceBoosted, supersonicTime, handbrakeVal, isAutoFlipping, \
autoFlipTimer, autoFlipTorqueScale, isDemoed, demoRespawnTimer, lastControls, \
worldContact.hasContact, worldContact.contactNormal, \
carContact.otherCarID, carContact.cooldownTimer

enum class Team : byte {
	BLUE = 0,
	ORANGE = 1
};

#define RS_OPPOSITE_TEAM(team) ((team) == Team::BLUE ? Team::ORANGE : Team::BLUE)
#define RS_TEAM_FROM_Y(y) ((y) < 0 ? Team::BLUE : Team::ORANGE)

class RS_API Car {
public:
	// Configuration for this car
	CarConfig config;
	Team team;

	// Each car is given a unique ID when created by the arena
	// Will always be >0 (unless you somehow reach integer overflow)
	uint32_t id;

	// The controls to simulate the car with
	CarControls controls;

	CarState GetState();
	void SetState(const CarState& state);

	void Demolish(float respawnDelay = RLConst::DEMO_RESPAWN_TIME);

	// Respawn the car, called after we have been demolished and waited for the respawn timer
	void Respawn(GameMode gameMode, int seed = -1, float boostAmount = RLConst::BOOST_SPAWN_AMOUNT);

	btVehicleRL _bulletVehicle;
	btDefaultVehicleRaycaster _bulletVehicleRaycaster;
	btRigidBody _rigidBody;
	btCompoundShape _compoundShape;
	btBoxShape _childHitboxShape;

	// NOTE: Not all values are updated because they are unneeded for internal simulation
	// Those values are only updated when GetState() is called
	CarState _internalState;

	// Get the forward direction as a unit vector
	Vec GetForwardDir() const {
		return _internalState.rotMat.forward;
	}

	// Get the rightward direction as a unit vector
	Vec GetRightDir() const {
		return _internalState.rotMat.right;
	}

	// Get the upward direction as a unit vector
	Vec GetUpDir() const {
		return _internalState.rotMat.up;
	}

	void _PreTickUpdate(GameMode gameMode, float tickTime, const MutatorConfig& mutatorConfig);
	void _PostTickUpdate(GameMode gameMode, float tickTime, const MutatorConfig& mutatorConfig);

	Vec _velocityImpulseCache = { 0,0,0 };
	void _FinishPhysicsTick(const MutatorConfig& mutatorConfig);

	void _BulletSetup(GameMode gameMode, class btDynamicsWorld* bulletWorld, const MutatorConfig& mutatorConfig);
	
	// For construction by Arena
	static Car* _AllocateCar() { return new Car(); }

	void Serialize(DataStreamOut& out);
	void _Deserialize(DataStreamIn& in);

	Car(const Car& other) = delete;
	Car& operator=(const Car& other) = delete;

	~Car() {}

private:
	void _UpdateWheels(float tickTime, const MutatorConfig& mutatorConfig, int numWheelsInContact, float forwardSpeed_UU);
	void _UpdateBoost(float tickTime, const MutatorConfig& mutatorConfig, float forwardSpeed_UU);
	void _UpdateJump(float tickTime, const MutatorConfig& mutatorConfig, bool jumpPressed);
	void _UpdateAirTorque(float tickTime, const MutatorConfig& mutatorConfig, bool doAirControl);
	void _UpdateDoubleJumpOrFlip(float tickTime, const MutatorConfig& mutatorConfig, bool jumpPressed, float forwardSpeed_UU);
	void _UpdateAutoFlip(float tickTime, const MutatorConfig& mutatorConfig, bool jumpPressed);
	void _UpdateAutoRoll(float tickTime, const MutatorConfig& mutatorConfig, int numWheelsInContact);

	Car() {}
};

RS_NS_END