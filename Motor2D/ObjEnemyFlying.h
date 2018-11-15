#ifndef _OBJ_ENEMY_FLYING_H_
#define _OBJ_ENEMY_FLYING_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2DynArray.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_thread.h"
#include "j1Input.h"

// TODO: config.xml, testing for now
#define MIN_DISTANCE 15
#define MAX_DISTANCE 25
#define MAX_IDLE_RPATH 6

struct Collider;

struct threadData
{
	iPoint origin = { 0,0 };
	iPoint destination = { 0,0 };
	bool waitingForPath = false;
	bool ready = false;
	int index = -1;
	// for assure the enemy is saved in correct place, needs to be mutable for how the methods works, see save()
	// maybe now that we have randomly generated idle paths, this can be changed
	mutable p2DynArray<iPoint> last_path = NULL; 
	
	void CopyLastGeneratedPath();
};

class ObjEnemyFlying : public GameObject {
protected:
	enum enemyState
	{
		SEARCHING, // represents idle state, searching a player in manhattan range
		FOLLOWING, // player is in range and has a viable path
		DEATH
	};

public:

	ObjEnemyFlying(fPoint position, int index, pugi::xml_node &object_node);
	bool Update(float dt) override;
	bool PreUpdate() override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void MarkObject(bool mark) override;

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

public:
	Animation idleAnimSearching;
	Animation idleAnimDetected;
	Animation idleAnimMarked;
	Animation jetPackFire;
	Animation * currAnim = nullptr;

private:
	uint animTileWidth = 0u;
	uint animTileHeight = 0u;
	Collider * collider = nullptr;

	// pathfinding relatives ----------------
	//functions --
	iPoint GetNextWorldNode() const;
	iPoint GetMapPosition() const;
	void MoveToWorldNode(const iPoint& node, float dt) const;
	bool isPlayerInTileRange(const uint range) const; // returns true if player are on input range
	void followPath(float dt) const;
	void StartNewPathThread();
	// variables --
	//bool multiThreadEnabled = false;
	SDL_Thread* threadID = nullptr;
	fPoint lastValidPos = { 0.0F,0.0F };
	// --------------------------------------

	// provisional timer --------------------
	Uint32 start_time = 0;
	Uint32 frequency_time = 1500;
	// --------------------------------------
	// path frequency intervals
	Uint32 min_ms = 0u;
	Uint32 max_ms = 0u;

	//enemy state machine
	enemyState enemy_state = enemyState::SEARCHING; // default state
	bool marked = false;

	// idle movement
	void idleMovement(float dt);
	p2DynArray<iPoint> idlePath = NULL;
	int posIndex = 0;

	// utils
	int GetRandomValue(const int min,const  int max) const;
	void GenerateNewIdlePath(const int minTiles, const int maxTiles);

	// flip animation
	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
	void CheckFacingDirection();
	fPoint previousPos = { 0,0 };

	// data to send to thread
	threadData pathData;

	// debug pathfinding debugdraw
	bool pathDebugDraw = true;

	// follow speeds from xml
	float chasingSpeed = 0.0F;
	float idleSpeed = 0.0F;
	// tilePoints for idle generator
	int minTiles, maxTiles = 0;
};

//extern ObjEnemyFlying* mytest;

#endif