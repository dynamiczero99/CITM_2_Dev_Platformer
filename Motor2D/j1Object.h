#ifndef __j1OBJECT_H__
#define __j1OBJECT_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"
#include "PugiXml/src/pugixml.hpp"

#define MAX_OBJECTS 300

//Objects
class ObjPlayer;
class ObjProjectile;
class ObjBox;
class ObjTrigger;
class ObjEnemyFlying;
class ObjEnemyLand;

struct Collider;
enum COLLIDER_TYPE;
struct SDL_Texture;
class Animation;
enum triggerAction;

enum class dir : uint {
	invalid,
	left,
	right,
	down,
	up,
	max
};

//Pivot vertical
enum class PivotV : uchar {
	top,
	middle,
	bottom
};


//Pivot horizontal
enum class PivotH : uchar {
	left,
	middle,
	right
};

struct Pivot {
	Pivot(PivotV vert, PivotH horiz) : vert(vert), horiz(horiz) {}
	PivotV vert = PivotV::top;
	PivotH horiz = PivotH::left;
};

class GameObject {
	friend class j1Object; // Only j1Object factory can access the properties of this class
	//Methods
public:
	virtual void MarkObject(bool mark);
	virtual void OnCollision(Collider * c1, Collider * c2);
	//- Returns the position of the object specifing a pivot
	iPoint GetObjPivotPos(Pivot pivot);

protected:
	GameObject(fPoint &position, int index);
	virtual bool OnDestroy();
	~GameObject();

	virtual bool PreUpdate();
	virtual bool TimedUpdate(float dt);
	virtual bool Update(float dt);
	virtual bool PostUpdate();

	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;

	//- Returns the top-left position of a rectangle considering a pivot point
	//- Used to draw (Blit) or put the collider (SetPos)
	iPoint GetRectPos(Pivot pivot, int x, int y, uint w, uint h);
	//- Returns the pivot position specifing a rectangle
	iPoint GetPivotPos(Pivot pivot, int x, int y, uint w, uint h);
	bool LoadAnimation(pugi::xml_node &node, Animation &anim);
	float TileToPixel (uint pixel);

	//Variables
public:
	mutable fPoint position = fPoint(0.0F, 0.0F);
	fPoint velocity = fPoint(0.0F, 0.0F);
	fPoint acceleration = fPoint(0.0F, 0.0F);
	bool marked = false;

protected:
	//The position in the objects module array
	int index = -1;
	Uint32 updateCycle = UINT_MAX;
	Uint32 lastUpdate = 0u;
	Pivot pivot = Pivot(PivotV::top, PivotH::left);
	Collider * col = nullptr;
};

class j1Object : public j1Module
{
public:
	j1Object();
	bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	int FindEmptyPosition();

	ObjPlayer * AddObjPlayer(fPoint position);
	ObjProjectile * AddObjProjectile(fPoint position, fPoint direction, ObjPlayer * objPlayer);
	ObjBox * AddObjBox(fPoint position);
	ObjTrigger * AddObjTrigger(fPoint position, triggerAction action, iPoint rectSize);
	ObjEnemyFlying * AddObjEnemyFlying(fPoint position);
	ObjEnemyLand * AddObjEnemyLand(fPoint position);
	bool DeleteObject(GameObject * object);

public:
	SDL_Texture * projectileTex = nullptr;
	SDL_Texture * playerIdleTex = nullptr;
	SDL_Texture * playerRunTex = nullptr;
	SDL_Texture * playerJumpTex = nullptr;
	SDL_Texture * robotTilesetTex = nullptr;
	SDL_Texture * debugEnemyPathTex = nullptr;
	SDL_Texture * shootIndicatorTex = nullptr;
	ObjPlayer * player = nullptr;
	// sfx id
	uint impactBoxSFX = 0u;
	// marked box for load method pointer
	ObjBox* object_box_markedOnLoad = nullptr;
	uint tileSize = 0u;

private:
	GameObject * objects [MAX_OBJECTS];
	uint actualObjects = 0u;
	pugi::xml_node object_node;
	
};

#endif