#ifndef _OBJ_TRIGGER_H_
#define _OBJ_TRIGGER_H_

#include "p2Point.h"
#include "j1Object.h"
#include "p2DynArray.h"

struct Collider;

enum triggerAction {
	none,
	animation,
	undefined
};


class ObjTrigger : public GameObject {
public:
	ObjTrigger(fPoint &position, int index, triggerAction action, iPoint &rectSize);
	void OnCollision(Collider * c1, Collider * c2) override;
	bool OnDestroy();

	p2DynArray<int> objectsEventIDs = NULL;

private:
	triggerAction action = triggerAction::none;
	SDL_Rect colRect;

	
};

#endif
