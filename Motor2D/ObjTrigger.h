#ifndef _OBJ_TRIGGER_H_
#define _OBJ_TRIGGER_H_

#include "p2Point.h"
#include "j1Object.h"

struct Collider;

class ObjTrigger : public Gameobject {
public:
	ObjTrigger(fPoint position, int index, triggerAction action);
	void OnCollision(Collider * c1, Collider * c2) override;

private:
	triggerAction action = triggerAction::none;
};

#endif
