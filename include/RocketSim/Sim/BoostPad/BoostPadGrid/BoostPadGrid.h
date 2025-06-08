#pragma once

#include <RocketSim/BaseInc.h>
#include <RocketSim/RLConst.h>

#include <RocketSim/Sim/BoostPad/BoostPad.h>

RS_NS_START

struct BoostPadGrid {
	constexpr static float
		EXTENT_X = 4096.f,
		EXTENT_Y = 5120.f,
		EXTENT_Z = RLConst::BoostPads::CYL_HEIGHT + 250.f;

	constexpr static int
		CELLS_X = 8,
		CELLS_Y = 10,
		CELL_SIZE_X = (int)(EXTENT_X / (CELLS_X / 2)),
		CELL_SIZE_Y = (int)(EXTENT_Y / (CELLS_Y / 2)),
		CELL_AMOUNT = CELLS_X * CELLS_Y;

	BoostPad* pads[CELLS_X][CELLS_Y] = {};

	BoostPadGrid() = default;

	void CheckCollision(Car* car);
	void Add(BoostPad* pad);
};

RS_NS_END