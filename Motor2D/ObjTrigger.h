#ifndef _OBJ_TRIGGER_H_
#define _OBJ_TRIGGER_H_

#include "p2Point.h"
#include "j1Object.h"

struct Collider;

enum triggerAction {
	none,
	animation,
	undefined
};


class ObjTrigger : public GameObject {
public:
	ObjTrigger(fPoint position, int index, triggerAction action, iPoint rectSize);
	void OnCollision(Collider * c1, Collider * c2) override;
	bool OnDestroy();

private:
	triggerAction action = triggerAction::none;
	SDL_Rect colRect;
	Collider* collider = nullptr;
};

#endif
