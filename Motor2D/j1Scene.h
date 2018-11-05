#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CameraLogic();

	bool searchValidCameraPos();

	bool Load(pugi::xml_node& node);
	bool Save(pugi::xml_node& node) const;

private:

	fPoint cameraPos = { 0,0 };
	

	bool firstStart = true;

	bool teleport = false; // testing var, must to be player relative

	void DebugInput();

};

#endif // __j1SCENE_H__