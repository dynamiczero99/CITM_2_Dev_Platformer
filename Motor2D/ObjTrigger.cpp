#include "ObjTrigger.h"
#include "j1Object.h"
#include "j1Collision.h"
#include "j1App.h"

ObjTrigger::ObjTrigger(fPoint &position, int index, pugi::xml_node& node, iPoint &rectSize) : GameObject(position, index),
						colRect({ colRect.x = position.x, colRect.y = position.y, colRect.w = rectSize.x, colRect.h = rectSize.y }){

	LoadAnimation(node.child("animation").child("small_inactive_animation"), smallAnim[animState::inactive]);
	LoadAnimation(node.child("animation").child("small_active_animation"), smallAnim[animState::active]);

	col = App->collision->AddCollider(colRect, COLLIDER_TRIGGER, this);
}

bool ObjTrigger::Update(float dt)
{
	
	if (timer.Read() > 1000 && activated)
	{
		LOG("unswitched");
		activated = false;
	}

	return true;
}

bool ObjTrigger::PostUpdate()
{
	
	App->render->Blit(App->object->robotTilesetTex, position.x, position.y, &smallAnim[animState::active].GetCurrentFrame());

	return true;
}

void ObjTrigger::OnCollision(Collider * c1, Collider * c2) {
	if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) {
		LOG("player triggered");
		timer.Start();
		if (!activated)
			activated = true;
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