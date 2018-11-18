#include "ObjTrigger.h"
#include "j1Object.h"
#include "j1Collision.h"
#include "j1App.h"

ObjTrigger::ObjTrigger(fPoint &position, int index, pugi::xml_node& node, iPoint &rectSize) : GameObject(position, index),
						colRect({ colRect.x = position.x, colRect.y = position.y, colRect.w = rectSize.x, colRect.h = rectSize.y }){

	LoadAnimation(node.child("animation").child("small_inactive_animation"), smallAnim[animState::inactive]);
	LoadAnimation(node.child("animation").child("small_active_animation"), smallAnim[animState::active]);
	
	coolDown = node.child("cooldown_time_ms").text().as_uint();

	col = App->collision->AddCollider(colRect, COLLIDER_TRIGGER, this);
}

bool ObjTrigger::Update(float dt)
{
	
	if (timer.Read() > coolDown && activated)
	{
		//LOG("unswitched");
		activated = false;
		currentState = animState::inactive;
		App->object->OnTriggerExit(this);
	}

	return true;
}

bool ObjTrigger::PostUpdate()
{
	
	App->render->Blit(App->object->robotTilesetTex, position.x, position.y, &smallAnim[currentState].GetCurrentFrame());

	return true;
}

void ObjTrigger::OnCollision(Collider * c1, Collider * c2) {
	if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER || c2->type == COLLIDER_TYPE::COLLIDER_BOX) {
		timer.Start();
		if (!activated)
		{
			switch (c2->type)
			{
				case COLLIDER_PLAYER:
					LOG("player triggered");
					break;
				case COLLIDER_BOX:
					LOG("collider box triggered");
					break;
			}
			activated = true;
			currentState = animState::active;
			App->object->OnTriggerEnter(this);
		}
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