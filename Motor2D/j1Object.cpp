#include "j1Object.h"
#include "j1Collision.h"
#include "j1App.h"
#include "j1Module.h"
#include "p2Log.h"
//Objects
#include "ObjPlayer.h"
#include "PugiXml/src/pugixml.hpp"

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
			delete objects[i];
			objects[i] = nullptr;
		}
	}
	return true;
}

//You should try to AddObject() on the Start method
//- On the Awake method the object module might have not gotten its node from the xml (depends on the order of the modules and if the module you are calling from is before of after the module object)
//- On the PreUpdate you might miss the PreUpdate of the object module (thus not calling PreUpdate in the object)
//- On the Update you will miss PreUpdate and might miss the object's Update
//- On the PostUpdate you will miss PreUpdate, Update and might miss the object's PostUpdate
Gameobject * j1Object::AddObject(OBJECT_TYPE type, fPoint position) {
	Gameobject * returnObj = nullptr;

	for (uint i = 0; i < MAX_OBJECTS; ++i)
	{
		if (objects[i] == nullptr)
		{
			//"new" calls the object's constructor. We'll use it to initialize all the variables necessary for the object behaviour.
			switch (type) {
			case OBJECT_TYPE::NONE:
				break;
			case OBJECT_TYPE::PLAYER:
				returnObj = objects[i] = new ObjPlayer(object_node, position, i);
				break;
			case OBJECT_TYPE::BOX:
				returnObj = objects[i] = new Gameobject(position, i);
				break;
			case OBJECT_TYPE::ENEMY:
				returnObj = objects[i] = new Gameobject(position, i);
				break;
			}
			actualObjects++;
			return returnObj;
		}
	}

	LOG("Reached maximum object capacity, no more objects can be added.");
	return nullptr;
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
		delete objects[object->index];
		objects[object->index] = nullptr;
		actualObjects--;
	}
	return true;
}

//Gameobject class methods -------------------------------------------

Gameobject::Gameobject(fPoint position, int index) :
	position(position),
	index(index) {
	velocity = fPoint(0.0f, 0.0f);
	acceleration = fPoint(0.0f, 0.0f);
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

void Gameobject::OnCollision(Collider * c1, Collider * c2) {

}