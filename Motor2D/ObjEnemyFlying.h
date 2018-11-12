#ifndef _OBJ_ENEMY_FLYING_H_
#define _OBJ_ENEMY_FLYING_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2DynArray.h"

// TODO: config.xml, testing for now
#define MIN_DISTANCE 15
#define MAX_DISTANCE 25
#define MAX_IDLE_RPATH 6

struct Collider;

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
	void CopyLastGeneratedPath();
	bool isPlayerInTileRange(const uint range) const; // returns true if player are on input range
	void followPath(float dt);

	// variables --
	mutable p2DynArray<iPoint> last_path = NULL;
	fPoint lastValidPos = { 0.0F,0.0F };

	// provisional timer --
	Uint32 start_time = 0;
	Uint32 frequency_time = 1500;
	// --------------------------------------

	//enemy state machine
	enemyState enemy_state = enemyState::SEARCHING; // default state

	// idle movement
	void idleMovement(float dt);
	p2DynArray<iPoint> idlePath = NULL;

	// utils
	int GetRandomValue(const int min,const  int max) const;
	void GenerateNewIdlePath(const int minTiles, const int maxTiles);

};

#endif