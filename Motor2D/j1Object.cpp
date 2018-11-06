#include "j1Object.h"
#include "j1Module.h"
#include "j1Collision.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Module.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "PugiXml/src/pugixml.hpp"
#include "j1Audio.h"
//Objects
#include "ObjPlayer.h"
#include "ObjProjectile.h"
#include "ObjBox.h"
#include "ObjTrigger.h"

j1Object::j1Object() : j1Module() {
	name.create("object");

	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		objects[i] = nullptr;
	}

}

bool j1Object::Awake(pugi::xml_node& node) {
	object_node = node;
	return true;
}

bool j1Object::Start() {
	//INFO: Load all textures here (we don't want each instance of an object to be loading the same texture again and again)
	projectileTex = App->tex->LoadTexture(object_node.child("projectile_image").text().as_string());
	playerIdleTex = App->tex->LoadTexture(object_node.child("player_idle_image").text().as_string());
	playerRunTex = App->tex->LoadTexture(object_node.child("player_run_image").text().as_string());
	playerJumpTex = App->tex->LoadTexture(object_node.child("player_jump_image").text().as_string());
	robotTex = App->tex->LoadTexture(object_node.child("robot_image").text().as_string());
	//Add objects
	fPoint playerStartPos;
	playerStartPos.x = App->map->playerData.x;
	playerStartPos.y = App->map->playerData.y;
	player = App->object->AddObjPlayer(playerStartPos);

	// load general sfx for objects
	impactBoxSFX = App->audio->LoadFx(object_node.child("projectile").find_child_by_attribute("name", "impact").attribute("value").as_string());

	return true;
}

bool j1Object::PreUpdate() {

	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		if (objects[i] != nullptr) {
			objects[i]->PreUpdate();
		}
	}
	return true;
}

bool j1Object::Update(float dt) {
	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		if (objects[i] != nullptr) {
			objects[i]->Update();
		}
	}
	return true;
}

bool j1Object::PostUpdate() {
	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		if (objects[i] != nullptr) {
			objects[i]->PostUpdate();
		}
	}
	return true;
}

bool j1Object::CleanUp() {
	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		if (objects[i] != nullptr) {
			//"delete" call's the object's destructor. We'll use it to reamove all allocated memory.
			objects[i]->OnDestroy();
			delete objects[i];
			objects[i] = nullptr;
		}
	}
	App->tex->UnloadTexture(playerIdleTex);
	App->tex->UnloadTexture(playerRunTex);
	App->tex->UnloadTexture(playerJumpTex);
	App->tex->UnloadTexture(projectileTex);
	App->tex->UnloadTexture(robotTex);
	// unload sfx
	App->audio->UnloadDesiredSFX(impactBoxSFX); // unload a desired sfx
	//App->audio->UnloadSFX(); // unload all sfx on audio sfx list

	// "disable" the module
	active = false;

	return true;
}

int j1Object::FindEmptyPosition() {
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] == nullptr)
		{
			actualObjects++;
			return i;
		}
	}

	LOG("Reached maximum object capacity, returning -1");
	return -1;
}


//You should try to AddObject() on the Start method
//- On the Awake method the object module might have not gotten its node from the xml (depends on the order of the modules and if the module you are calling from is before of after the module object)
//- On the PreUpdate you might miss the PreUpdate of the object module (thus not calling PreUpdate in the object)
//- On the Update you will miss PreUpdate and might miss the object's Update
//- On the PostUpdate you will miss PreUpdate, Update and might miss the object's PostUpdate
ObjPlayer * j1Object::AddObjPlayer(fPoint position) {
	int index = FindEmptyPosition();
	ObjPlayer * ret = nullptr;
	if(index != -1) {
		objects[index] = ret = new ObjPlayer(object_node.child("player"), position, index);
	}
	return ret;
}

ObjProjectile * j1Object::AddObjProjectile(fPoint position, fPoint direction, ObjPlayer * objPlayer) {
	int index = FindEmptyPosition();
	ObjProjectile * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjProjectile (position, index, object_node.child("projectile"), direction, objPlayer);
	}
	return ret;
}

ObjBox * j1Object::AddObjBox (fPoint position) {
	int index = FindEmptyPosition();
	ObjBox * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjBox(position, index, object_node.child("box"));
	}
	return ret;
}

ObjTrigger * j1Object::AddObjTrigger(fPoint position, triggerAction action, iPoint rectSize) //triggerAction::none)
{
	int index = FindEmptyPosition();
	ObjTrigger * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjTrigger(position, index, action, rectSize);
	}
	return ret;
}


bool j1Object::DeleteObject(Gameobject * object) {
	assert(object != nullptr);
	assert(object->index != -1);
	if (object == nullptr || object->index == -1) {
		LOG("Invalid collider index");
		return false;
	}
	//TODO: Also check if the collider index exceeds the bound of the collider array
	if (objects[object->index] != nullptr)
	{
		//"delete" calls the object's destructor. We'll use it to reamove all allocated memory.
		objects[object->index]->OnDestroy();
		delete objects[object->index];
		objects[object->index] = nullptr;
		object = nullptr;
		actualObjects--;
	}
	return true;
}

bool j1Object::Load(pugi::xml_node& node)
{
	for (int i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
			objects[i]->Load(node);
	}

	return true;
}

bool j1Object::Save(pugi::xml_node& node) const
{
	for (int i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
			objects[i]->Save(node);
	}



	return true;
}

//Gameobject class methods -------------------------------------------

Gameobject::Gameobject(fPoint position, int index) :
	position(position),
	index(index) {
}

Gameobject::~Gameobject () {

}

bool Gameobject::PreUpdate() {
	return true;
}

bool Gameobject::Update() {
	return true;
}

bool Gameobject::PostUpdate() {
	return true;
}

bool Gameobject::OnDestroy() {
	return true;
}

//Virtual method rewritten in each gameobject that does something when it collides
void Gameobject::OnCollision(Collider * c1, Collider * c2) {
}

//Virtual method rewritten in each gameobject that is able to be marked
void Gameobject::MarkObject(bool mark) {
}

//Method that returns the pivot position of the object from the top left position of a rectangle
iPoint Gameobject::GetPivotPos(pivot pivot, int x, int y, uint w, uint h) {
	switch (pivot) {
	case pivot::top_left:
		return iPoint(x, y);
		break;
	case pivot::top_middle:
		return iPoint(x + w / 2, y);
		break;
	case pivot::top_right:
		return iPoint(x + w, y);
		break;
	case pivot::middle_left:
		return iPoint(x, y + h / 2);
		break;
	case pivot::middle_middle:
		return iPoint(x + w / 2, y + h / 2);
		break;
	case pivot::middle_right:
		return iPoint(x + w, y + h / 2);
		break;
	case pivot::bottom_left:
		return iPoint(x, y + h);
		break;
	case pivot::bottom_middle:
		return iPoint(x + w / 2, y + h);
		break;
	case pivot::bottom_right:
		return iPoint(x + w, y + h);
		break;
	default:
		LOG("Error reading the pivot");
		break;
	}
}

//Method that returns the top-left position considering the pivot point of the object
iPoint Gameobject::GetRectPos(pivot pivot, int x, int y, uint w, uint h) {
	switch (pivot) {
	case pivot::top_left:
		return iPoint(x, y);
		break;
	case pivot::top_middle:
		return iPoint(x - w / 2, y);
		break;
	case pivot::top_right:
		return iPoint(x - w, y);
		break;
	case pivot::middle_left:
		return iPoint(x, y - h / 2);
		break;
	case pivot::middle_middle:
		return iPoint(x - w / 2, y - h / 2);
		break;
	case pivot::middle_right:
		return iPoint(x - w, y - h / 2);
		break;
	case pivot::bottom_left:
		return iPoint(x, y - h);
		break;
	case pivot::bottom_middle:
		return iPoint(x - w / 2, y - h);
		break;
	case pivot::bottom_right:
		return iPoint(x - w, y - h);
		break;
	default:
		LOG("Error reading the pivot");
		break;
	}
}

bool Gameobject::LoadAnimation(pugi::xml_node &node, Animation &anim) {
	for (; node; node = node.next_sibling("sprite")) {
		SDL_Rect frame;
		frame.x = node.attribute("x").as_int();
		frame.y = node.attribute("y").as_int();
		frame.w = node.attribute("w").as_int();
		frame.h = node.attribute("h").as_int();
		anim.PushBack(frame);
	}
	return true;
}

bool Gameobject::Load(pugi::xml_node& node)
{
	return true;
}

bool Gameobject::Save(pugi::xml_node& node) const
{
	return true;
}