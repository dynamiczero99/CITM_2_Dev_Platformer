#ifndef _OBJ_TRIGGER_H_
#define _OBJ_TRIGGER_H_

#include "p2Point.h"
#include "j1Object.h"
#include "p2DynArray.h"
#include "j1Timer.h"
#include "p2Animation.h"
#include "p2Log.h"

struct Collider;

//enum triggerAction {
//	none,
//	animation,
//	undefined
//};


class ObjTrigger : public GameObject {
public:
	ObjTrigger(fPoint &position, int index, pugi::xml_node& node,iPoint &rectSize);
	bool Update(float dt);
	bool PostUpdate();
	void OnCollision(Collider * c1, Collider * c2) override;
	bool OnDestroy();

	p2DynArray<int> objectsEventIDs = NULL;

private:

	enum animState
	{
		active,
		inactive,
		max
	};
	
	SDL_Rect colRect;
	j1Timer timer;
	Animation smallAnim[animState::max];
	bool activated = false;
	uint32 coolDown = 0u;
	animState currentState = animState::inactive;

};

#endif
