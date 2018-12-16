#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"

struct SDL_Texture;

class UI_Sprite;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CameraLogic(float dt);

	bool SearchValidCameraPos();

	bool Load(pugi::xml_node& node);
	bool Save(pugi::xml_node& node) const;

	void CreateWidgets();

public:
	p2SString menu;
	bool pauseGame = false;
	bool fullscreen = false;
	bool active_window = false;
	bool enable_continue = false;
	bool escape_menu = false;
	bool in_mainmenu = true;
	float scene_dt = 0.0f;
	UI_Sprite* window_to_close =  nullptr;

private:
	void DebugInput();
	// debug path generation with mouse
	p2DynArray<iPoint> last_path = NULL;
	void CopyLastGeneratedPath();

	bool OnEvent(UI_Button * button);

private:
	fPoint cameraPos = { 0,0 };
	bool firstStart = true;
	bool teleport = false; // testing var, must to be player relative
	SDL_Texture* debug_tex = nullptr;
	//Factor for which the distance to the target position is going to be multiplied
	float	cameraHMultiplier = 0.0f;//When moving horizontally
	float	cameraJumpMultiplier = 0.0f;//When jumping
	float	cameraFallMultiplier = 0.0f;//When falling
	float	horizontalScreenDivision = 0.0f;
	float	verticalScreenDivision = 0.0f;

	
	bool mustClose = true;
	
};

#endif // __j1SCENE_H__