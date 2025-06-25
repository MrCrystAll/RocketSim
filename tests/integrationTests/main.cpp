#include <RocketSim/version.h>
#include <RocketSim/RocketSim.h>

#include <RocketSim/Sim/Arena/Arena.h>	

#include <iostream>

int main() {
	using std::cout, std::endl;
	using namespace RocketSim;

	Init("./resources/collision_meshes", false);

	Arena* arena = Arena::Create(GameMode::SOCCAR);
	arena->Step(100);

	cout << "Successfully integrated RocketSim " << ROCKETSIM_VERSION << "!" << endl;
}