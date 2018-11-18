#ifndef _OBJ_ENEMY_H_
#define _OBJ_ENEMY_H_

#include "j1Object.h"
#include "p2DynArray.h"
#include "SDL/include/SDL_thread.h"

struct threadData
{
	iPoint origin = { 0,0 };
	iPoint destination = { 0,0 };
	bool waitingForPath = false;
	bool ready = false;
	int index = -1;
	// for assure the enemy is saved in correct place, needs to be mutable for how the methods works, see save()
	// maybe now that we have randomly generated idle paths, this can be changed
	mutable p2DynArray<iPoint> path = NULL;

	void CopyLastGeneratedPath();
};

class ObjEnemy : public GameObject {
public:
	ObjEnemy(fPoint &position, int index);
protected:
	enum enemyState
	{
		INVALID,
		IDLE, // represents idle state, searching a player in manhattan range
		CHASING, // player is in range and has a viable path
		DEATH
	};
	bool IsPlayerInTileRange(const uint range) const;
	//void StartNewPathThread();
	int GetRandomValue(const int min, const  int max) const;
	void DebugPath();

	//Variables
protected:
	// data to send to thread
	threadData pathData;
	SDL_Thread* threadID = nullptr;

	// provisional timer --------------------
	Uint32 start_time = 0;
	Uint32 frequency_time = 1500;

	// path frequency intervals
	Uint32 min_ms = 0u;
	Uint32 max_ms = 0u;

	//enemy state machine
	enemyState enemy_state = enemyState::IDLE; // default state

	bool pathDebugDraw = false;
};

#endif // !

