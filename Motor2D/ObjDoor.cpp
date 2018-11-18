#include "ObjDoor.h"
#include "j1Object.h"
#include "j1Collision.h"
#include "j1App.h"

ObjDoor::ObjDoor(fPoint &position, int index, pugi::xml_node &node, int objectID) : GameObject(position, index, objectID) 
{

	LoadAnimation(node.child("animation").child("active"), anim[animState::active]);
	LoadAnimation(node.child("animation").child("turn_on"), anim[animState::turn_on]);
	anim[animState::turn_on].loop = false;
	LoadAnimation(node.child("animation").child("turn_off"), anim[animState::turn_off]);
	anim[animState::turn_off].loop = false;

	//coolDown = node.child("cooldown_time_ms").text().as_uint();
	rectSize = { node.child("animation").child("active").child("sprite").attribute("w").as_int(),
					    node.child("animation").child("active").child("sprite").attribute("h").as_int() };
	colRect = { (int)position.x,(int)position.y, rectSize.x,rectSize.y };
	col = App->collision->AddCollider(colRect, COLLIDER_WALL, this);

}

bool ObjDoor::Update(float dt)
{
	if (currTex == nullptr) // loads first start texture
		currTex = App->object->laserActiveTex;
	// check laser door state
	if (anim[turn_on].Finished())
	{
		SetState(animState::active);
		// and adds collider
		if(col == nullptr)
			col = App->collision->AddCollider(colRect, COLLIDER_WALL, this);
	}

	return true;
}

bool ObjDoor::PostUpdate()
{

	App->render->Blit(currTex, position.x, position.y, &anim[currentState].GetCurrentFrame());

	return true;
}

void ObjDoor::OnCollision(Collider * c1, Collider * c2) {
	/*if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) {
		timer.Start();
		if (!activated)
		{
			LOG("player triggered");
			activated = true;
			currentState = animState::active;
			App->object->OnTriggerEnter(this);
		}*/
		/*switch (action) {
		case triggerAction::next_level:
			break;
		case triggerAction::death:
			break;
		}*/
	//}
}

bool ObjDoor::OnDestroy()
{
	if (col != nullptr)
	{
		App->collision->DeleteCollider(col);
		col = nullptr;
	}

	return true;
}

bool ObjDoor::OnTriggerEnter()
{

	SetState(animState::turn_off);
	return true;
}
bool ObjDoor::OnTriggerExit()
{
	SetState(animState::turn_on);
	return true;
}

enum animState;
bool ObjDoor::SetState(animState state)
{
	anim[currentState].Reset();
	switch (state)
	{
	case ObjDoor::active:
		currentState = animState::active;
		anim[currentState].Reset();
		currTex = App->object->laserActiveTex;
		break;
	case ObjDoor::turn_on:
		currentState = animState::turn_on;
		anim[currentState].Reset();
		currTex = App->object->laserTurnOnTex;
		break;
	case ObjDoor::turn_off:
		currentState = animState::turn_off;
		anim[currentState].Reset();
		currTex = App->object->laserTurnOffTex;
		// and deletes collider
		if (col != nullptr)
		{
			App->collision->DeleteCollider(col);
			col = nullptr;
		}
		break;
	case ObjDoor::inactive:
		break;
	case ObjDoor::max:
		break;
	default:
		break;
	}
	
	return true;
}