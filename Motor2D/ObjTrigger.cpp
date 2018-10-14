#include "ObjTrigger.h"
#include "j1Object.h"
#include "j1Collision.h"

ObjTrigger::ObjTrigger(fPoint position, int index, triggerAction action) : action(action), Gameobject(position, index) {

}

void ObjTrigger::OnCollision(Collider * c1, Collider * c2) {
	if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) {
		/*switch (action) {
		case triggerAction::next_level:
			break;
		case triggerAction::death:
			break;
		}*/
	}
}