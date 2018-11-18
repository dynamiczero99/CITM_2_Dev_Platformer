#include "ObjTrigger.h"
#include "j1Object.h"
#include "j1Collision.h"
#include "j1App.h"

ObjTrigger::ObjTrigger(fPoint &position, int index, triggerAction action, iPoint &rectSize) : action(action), GameObject(position, index),
						colRect({ colRect.x = position.x, colRect.y = position.y, colRect.w = rectSize.x, colRect.h = rectSize.y }){

	col = App->collision->AddCollider(colRect, COLLIDER_TRIGGER, this);
}

bool ObjTrigger::Update(float dt)
{


	return true;
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

bool ObjTrigger::OnDestroy()
{
	if (col != nullptr)
	{
		App->collision->DeleteCollider(col);
		col = nullptr;
	}

	return true;
}