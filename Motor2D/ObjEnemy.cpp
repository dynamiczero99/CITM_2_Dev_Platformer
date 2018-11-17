#include "ObjEnemy.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Object.h"
#include "ObjPlayer.h"
#include "p2Point.h"
#include "j1Pathfinding.h"
#include <random>
#include "Brofiler/Brofiler.h"

ObjEnemy::ObjEnemy(fPoint & position, int index) : GameObject(position, index)
{

}

bool ObjEnemy::isPlayerInTileRange(const uint range) const
{
	// translate to map coords
	iPoint thisPos = App->map->WorldToMap((int)position.x, (int)position.y);
	iPoint playerPos = App->map->WorldToMap((int)App->object->player->position.x, (int)App->object->player->position.y);

	return (thisPos.DistanceManhattan(playerPos) < range);
}

void ObjEnemy::StartNewPathThread()
{
	//BROFILER_CATEGORY("NEW worker thread", Profiler::Color::AliceBlue);
	//BROFILER_THREAD("new worker");
	//BROFILER_FRAME("newthread");
	iPoint thisPos = App->map->WorldToMap((int)position.x, (int)position.y);
	iPoint playerPos = App->object->player->GetObjPivotPos(Pivot(PivotV::middle, PivotH::middle));//App->map->WorldToMap((int)App->object->player->position.x, (int)App->object->player->position.y);
	playerPos = App->map->WorldToMap(playerPos.x, playerPos.y);

	if (thisPos.DistanceManhattan(playerPos) > 1) // if the enemy is at more than 1 distance manhattan
	{
		pathData.origin = thisPos;
		pathData.destination = playerPos;
		pathData.index = index;
		pathData.waitingForPath = true;

		j1PathFinding* newPathfinding = new j1PathFinding();
		threadID = SDL_CreateThread(newPathfinding->multiThreadCreatePath, "test", (void*)&pathData);

		//SDL_WaitThread(threadID, 0);

		frequency_time = GetRandomValue(min_ms, max_ms);

		delete newPathfinding;
	}
}

int ObjEnemy::GetRandomValue(const int min, const int max) const
{
	int value = 0;

	// recalcule new frequency time c++11 random engine
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> newRN(min, max); // distribution in range [min, max]

	value = newRN(rng);

	//LOG("random value: %i", value);

	return value;
}

void threadData::CopyLastGeneratedPath()
{
	const p2DynArray<iPoint>* pathToCopy = App->pathfinding->GetLastPath();

	last_path.Clear();
	for (uint i = 0; i < pathToCopy->Count(); ++i)
	{
		last_path.PushBack(*pathToCopy->At(i));
	}
	last_path.Flip();
}