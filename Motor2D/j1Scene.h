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

	/*void CreateWidgets();*/

public:
	p2SString menu;
	bool pauseGame = false;
	bool fullscreen = false;
	bool enable_continue = false;
	bool active_window = false;
	bool in_mainmenu = false;

	float scene_dt = 0.0f;

	//All UI pointers initialized to nullptr
public:
	//---------MainMenu

	UI_Sprite* title = nullptr;

	UI_Button* play =  nullptr;
	UI_Button* _continue = nullptr;
	UI_Button* settings = nullptr;
	UI_Button* credits = nullptr;
	UI_Button* webpage = nullptr;
	UI_Button* exit = nullptr;

	//-------Settings window
	UI_Sprite* settings_window = nullptr;
	UI_Button* toggle_fullscreen = nullptr;
	UI_Button* toggle_framerate = nullptr;
	UI_Button* close_settings = nullptr;
	//Sliders
	
	//------Credits window
	UI_Sprite* credits_window = nullptr;
	/*UI_Label* credits_content = nullptr;*/
	UI_Button* close_credits = nullptr;

	//-----------Ingame window
	UI_Sprite* ingame_window = nullptr;
	UI_Button* resume = nullptr;
	UI_Button* save = nullptr;
	UI_Button* load = nullptr;
	UI_Button* to_mainmenu = nullptr;
	UI_Button* close_ingame_menu = nullptr;


//Widget creation
public:
	void CreateMainMenu();
	void CreateSettings();
	void CreateCredits();
	void CreateInGameMenu();

	void DestroyMainMenu();
	void DestroySettings();
	void DestroyCredits();
	void DestroyInGameMenu();

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