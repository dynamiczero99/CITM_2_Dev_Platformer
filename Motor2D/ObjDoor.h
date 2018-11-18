#ifndef _OBJ_DOOR_H_
#define _OBJ_DOOR_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"

struct Collider;

class ObjDoor : public GameObject {
public:
	ObjDoor(fPoint &position, int index, pugi::xml_node &object_node, int objectID);
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool OnDestroy() override;

	void OnCollision(Collider * c1, Collider * c2) override;
	bool OnTriggerEnter();
	bool OnTriggerExit();

	//bool Load(pugi::xml_node& loadNode);
	//bool Save(pugi::xml_node& saveNode) const;

public:

	Animation * currAnim = nullptr;

private:
	enum animState
	{
		active,
		turn_on,
		turn_off,
		inactive,
		max
	};

	Animation anim[animState::max];
	animState currentState = animState::active;
	SDL_Texture* currTex = nullptr;
	iPoint rectSize = { 0,0 };
	SDL_Rect colRect;

//private:
	bool SetState(animState state);
	
};

#endif
