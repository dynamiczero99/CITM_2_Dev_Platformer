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
#include "ObjEnemyFlying.h"
#include "ObjEnemyLand.h"
#include "ObjDoor.h"

// Profiler ---
#include "Brofiler/Brofiler.h"

j1Object::j1Object() : j1Module() {
	name.create("object");

	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		objects[i] = nullptr;
	}

}

bool j1Object::Awake(pugi::xml_node& node) {
	tileSize = node.child("tile_size").text().as_uint();
	object_node = node;
	return true;
}

bool j1Object::Start() {
	//INFO: Load all textures here (we don't want each instance of an object to be loading the same texture again and again)
	projectileTex = App->tex->LoadTexture(object_node.child("projectile_image").text().as_string());
	playerIdleTex = App->tex->LoadTexture(object_node.child("player_idle_image").text().as_string());
	playerRunTex = App->tex->LoadTexture(object_node.child("player_run_image").text().as_string());
	playerJumpTex = App->tex->LoadTexture(object_node.child("player_jump_image").text().as_string());
	robotTilesetTex = App->tex->LoadTexture(object_node.child("robot_image").text().as_string());
	debugEnemyPathTex = App->tex->LoadTexture(object_node.child("debug_path_enemy_tex").text().as_string());
	shootIndicatorTex = App->tex->LoadTexture(object_node.child("shoot_indicator").text().as_string());
	laserActiveTex = App->tex->LoadTexture(object_node.child("door").child("laser_active_texture").text().as_string());
	laserTurnOnTex = App->tex->LoadTexture(object_node.child("door").child("laser_turn_on_texture").text().as_string());
	laserTurnOffTex = App->tex->LoadTexture(object_node.child("door").child("laser_turn_off_texture").text().as_string());
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
	BROFILER_CATEGORY("Objects PreUpdate", Profiler::Color::LightPink);
	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		if (objects[i] != nullptr) {
			objects[i]->PreUpdate();
		}
	}
	return true;
}

bool j1Object::Update(float dt) {
	//BROFILER_FRAME("ObjectsUpdate");
	BROFILER_CATEGORY("Objects Update", Profiler::Color::Pink);

	Uint32 currTime = SDL_GetTicks();
	for (uint i = 0; i < MAX_OBJECTS; ++i) {
		if (objects[i] != nullptr) { 
			if (currTime >= objects[i]->lastUpdate + objects[i]->updateCycle) {
				objects[i]->TimedUpdate((currTime - objects[i]->lastUpdate) / 1000.0f);
				objects[i]->lastUpdate = currTime;
			}
			objects[i]->Update(dt);
		}
	}
	return true;
}

bool j1Object::PostUpdate() {
	BROFILER_CATEGORY("Objects PostUpdate", Profiler::Color::HotPink);
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
	App->tex->UnloadTexture(robotTilesetTex);
	App->tex->UnloadTexture(debugEnemyPathTex);
	App->tex->UnloadTexture(shootIndicatorTex);
	App->tex->UnloadTexture(laserActiveTex);
	App->tex->UnloadTexture(laserTurnOnTex);
	App->tex->UnloadTexture(laserTurnOffTex);
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

bool j1Object::OnTriggerEnter(ObjTrigger* trigger)
{
	// check if we have any coincidence
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			for (int ids = 0; ids < trigger->objectsEventIDs.Count(); ++ids)
			{
				if (objects[i]->objectID == *trigger->objectsEventIDs.At(ids))
				{
					//LOG("coincidence");
					objects[i]->OnTriggerEnter();
				}
			}
		}
	}

	return true;
}

bool j1Object::OnTriggerExit(ObjTrigger* trigger)
{
	// check if we have any coincidence
	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] != nullptr)
		{
			for (int ids = 0; ids < trigger->objectsEventIDs.Count(); ++ids)
			{
				if (objects[i]->objectID == *trigger->objectsEventIDs.At(ids))
				{
					//LOG("coincidence");
					objects[i]->OnTriggerExit();
				}
			}
		}
	}

	return true;
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

ObjBox * j1Object::AddObjBox (fPoint position, int objectID) {
	int index = FindEmptyPosition();
	ObjBox * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjBox(position, index, object_node.child("box"), objectID);
	}
	return ret;
}

ObjTrigger * j1Object::AddObjTrigger(fPoint position, iPoint rectSize) //triggerAction::none)
{
	int index = FindEmptyPosition();
	ObjTrigger * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjTrigger(position, index, object_node.child("trigger"), rectSize);
	}
	return ret;
}

ObjEnemyFlying * j1Object::AddObjEnemyFlying(fPoint position) {
	int index = FindEmptyPosition();
	ObjEnemyFlying * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjEnemyFlying(position, index, object_node.child("enemy_flying"));
	}
	return ret;
}

ObjEnemyLand * j1Object::AddObjEnemyLand(fPoint position) {
	int index = FindEmptyPosition();
	ObjEnemyLand * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjEnemyLand(position, index, object_node.child("enemy_land"));
	}
	return ret;
}

ObjDoor * j1Object::AddObjDoor(fPoint position, int objectID) {
	int index = FindEmptyPosition();
	ObjDoor * ret = nullptr;
	if (index != -1) {
		objects[index] = ret = new ObjDoor(position, index, object_node.child("door"), objectID);
	}
	return ret;
}


bool j1Object::DeleteObject(GameObject * object) {
	if (object == nullptr || object->index == -1 || object->index >= MAX_OBJECTS) {
		LOG("Invalid object index");
		return false;
	}
	if (objects[object->index] != nullptr)
	{
		int objIndex = object->index;
		//"delete" calls the object's destructor. We'll use it to reamove all allocated memory.
		objects[objIndex]->OnDestroy();
		delete objects[objIndex];
		objects[objIndex] = object = nullptr;
		actualObjects--;
		return true;
	}
	return false;
}

bool j1Object::Load(pugi::xml_node& node)
{
	// check if the scene has gameObjects to load
	// boxes
	for (pugi::xml_node boxes = node.child("Box"); boxes; boxes = boxes.next_sibling("Box"))
	{
		LOG("node found");
		// check if the box has the attribute marked, we only can have 1 marked box at a time
		if (boxes.attribute("isMarked"))
		{
			object_box_markedOnLoad = App->object->AddObjBox({ boxes.attribute("x").as_float() + boxes.attribute("width").as_float() / 2,
															   boxes.attribute("y").as_float() + boxes.attribute("height").as_float() },
															   boxes.attribute("id").as_int());
			object_box_markedOnLoad->MarkObject(true);
		}
		else
			App->object->AddObjBox({ boxes.attribute("x").as_float() + boxes.attribute("width").as_float() / 2,
									 boxes.attribute("y").as_float() + boxes.attribute("height").as_float() }, 
									 boxes.attribute("id").as_int());
	}
	// enemies --------------------------------
	// flying enemy ---
	for (pugi::xml_node flyingEnemy = node.child("FlyingEnemy"); flyingEnemy; flyingEnemy = flyingEnemy.next_sibling("FlyingEnemy"))
	{
		ObjEnemyFlying* fe = App->object->AddObjEnemyFlying({ flyingEnemy.attribute("x").as_float(), flyingEnemy.attribute("y").as_float() });
		if (flyingEnemy.attribute("isMarked"))
		{
			fe->MarkObject(true);
			player->SetSwapObject(fe);
		}
	}

	// land enemy ---
	for (pugi::xml_node landEnemy = node.child("landEnemy"); landEnemy; landEnemy = landEnemy.next_sibling("landEnemy"))
	{
		ObjEnemyLand * le = App->object->AddObjEnemyLand({ landEnemy.attribute("x").as_float(), landEnemy.attribute("y").as_float() });
		//if (landEnemy.attribute("isMarked"))
		//{
		//	le->MarkObject(true);
		//	player->SetSwapObject(le);
		//}
	}

	// and load the rest of data
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
GameObject::GameObject(fPoint &position, int index) :
	position(position),
	index(index){
}

GameObject::GameObject(fPoint &position, int index, int objectID) :
	position(position),
	index(index),
	objectID(objectID){
}

GameObject::~GameObject () {

}

bool GameObject::PreUpdate() {
	return true;
}

bool GameObject::Update(float dt) {
	return true;
}

bool GameObject::TimedUpdate(float dt)
{
	return false;
}

bool GameObject::PostUpdate() {
	return true;
}

bool GameObject::OnDestroy() {
	return true;
}

bool GameObject::OnTriggerEnter(){
	return true;
}

bool GameObject::OnTriggerExit() {
	return true;
}

//Virtual method rewritten in each gameobject that does something when it collides
void GameObject::OnCollision(Collider * c1, Collider * c2) {
}

iPoint GameObject::GetObjPivotPos(Pivot pivot)
{
	return GetPivotPos(pivot, col->rect.x, col->rect.y, col->rect.w, col->rect.h);
}

//Virtual method rewritten in each gameobject that is able to be marked
void GameObject::MarkObject(bool mark) {
}

//Method that returns the pivot position of the object from the top left position of a rectangle
iPoint GameObject::GetPivotPos(Pivot pivot, int x, int y, uint w, uint h) {

	switch (pivot.vert) {
	//case pivotVert::top: doesn't do anything
	case PivotV::middle:
		y += h * 0.5f;
		break;
	case PivotV::bottom:
		y += h;
		break;
	}

	switch (pivot.horiz) {
	//case pivotHoriz::left: doesn't do anything
	case PivotH::middle:
		x += w * 0.5f;
		break;
	case PivotH::right:
		x += w;
		break;
	}

	return (iPoint(x, y));
}

//Method that returns the top-left position considering the pivot point of the object
iPoint GameObject::GetRectPos(Pivot pivot, int x, int y, uint w, uint h) {

	switch (pivot.vert) {
	//case pivotVert::top: doesn't do anything
	case PivotV::middle:
		y -= h * 0.5f;
		break;
	case PivotV::bottom:
		y -= h;
		break;
	}

	switch (pivot.horiz) {
	//case pivotHoriz::left: doesn't do anything
	case PivotH::middle:
		x -= w * 0.5f;
		break;
	case PivotH::right:
		x -= w;
		break;
	}

	return (iPoint(x, y));
}

bool GameObject::LoadAnimation(pugi::xml_node &node, Animation &anim) {
	anim.speed = node.attribute("speed").as_float();
	for (node = node.child("sprite"); node; node = node.next_sibling("sprite")) {
		SDL_Rect frame;
		frame.x = node.attribute("x").as_int();
		frame.y = node.attribute("y").as_int();
		frame.w = node.attribute("w").as_int();
		frame.h = node.attribute("h").as_int();
		anim.PushBack(frame);
	}
	return true;
}

bool GameObject::Load(pugi::xml_node& node)
{
	return true;
}

bool GameObject::Save(pugi::xml_node& node) const
{
	return true;
}

//We can use this variable to make it easier for us to understand the different distance the player can move or jump while building levels in the tiled editor
float GameObject::TileToPixel(uint pixel) {
	return pixel * App->object->tileSize;
}

void GameObject::LimitFallSpeed(float dt) {
	if (velocity.y * dt > maxFallSpeed) {
		velocity.y = maxFallSpeed / dt;
	}
}
