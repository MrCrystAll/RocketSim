#pragma once

#include <RocketSim/BaseInc.h>
#include <RocketSim/Sim/Car/Car.h>

#include <RocketSim/DataStream/DataStreamIn.h>
#include <RocketSim/DataStream/DataStreamOut.h>

#include <RocketSim/Sim/MutatorConfig/MutatorConfig.h>

RS_NS_START

struct RS_API BoostPadConfig {
	Vec pos;
	bool isBig = false;

	void Serialize(DataStreamOut& out) const;
	void Deserialize(DataStreamIn& in);
};
#define BOOSTPADCONFIG_SERIALIZATION_FIELDS \
pos, isBig

struct RS_API BoostPadState {
	bool isActive = true;
	float cooldown = 0;

	Car* curLockedCar = NULL;
	uint32_t prevLockedCarID = NULL;

	void Serialize(DataStreamOut& out) const;
	void Deserialize(DataStreamIn& in);
};
#define BOOSTPAD_SERIALIZATION_FIELDS \
isActive, cooldown, prevLockedCarID

class RS_API BoostPad {
public:
	BoostPadConfig config;

	Vec _posBT;
	Vec _boxMinBT, _boxMaxBT;

	BoostPadState _internalState;

	BoostPadState GetState() const { return _internalState; }
	void SetState(const BoostPadState& state) { _internalState = state; }

	// For construction by Arena
	static BoostPad* _AllocBoostPad();
	void _Setup(const BoostPadConfig& config);

	void _CheckCollide(Car* car);

	void _PreTickUpdate(float tickTime);
	void _PostTickUpdate(float tickTime, const MutatorConfig& mutatorConfig);
private:
	BoostPad() {}
};

RS_NS_END