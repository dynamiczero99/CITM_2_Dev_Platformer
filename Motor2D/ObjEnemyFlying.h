#ifndef _OBJ_ENEMY_FLYING_H_
#define _OBJ_ENEMY_FLYING_H_

#include "ObjEnemy.h"
#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2DynArray.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_thread.h"
#include "j1Input.h"

// TODO: config.xml, testing for now
#define MIN_DISTANCE 12
#define MAX_DISTANCE 20
#define MAX_IDLE_RPATH 6

struct Collider;

class ObjEnemyFlying : public ObjEnemy {
public:
	ObjEnemyFlying(fPoint &position, int index, pugi::xml_node &object_node);
	bool Update(float dt) override;
	bool TimedUpdate(float dt) override;
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

	// pathfinding relatives ----------------
	//functions --
	iPoint GetNextWorldNode() const;
	iPoint GetMapPosition() const;
	void MoveToWorldNode(const iPoint& node, float dt) const;
	void followPath(float dt) const;
	bool SearchNewPath();
	// variables --
	//bool multiThreadEnabled = false;
	fPoint lastValidPos = { 0.0F,0.0F };

	//bool marked = false;

	// idle movement
	void idleMovement(float dt);
	p2DynArray<iPoint> idlePath = NULL;
	int posIndex = 0;

	// Utilities
	void GenerateNewIdlePath(const int minTiles, const int maxTiles);

	// flip animation
	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
	void CheckFacingDirection();
	fPoint previousPos = { 0,0 };

	// follow speeds from xml
	float chasingSpeed = 0.0F;
	float idleSpeed = 0.0F;
	// tilePoints for idle generator
	int minTiles, maxTiles = 0;
};

//extern ObjEnemyFlying* mytest;

#endif