#ifndef _UI_BUTTON_
#define _UI_BUTTON_

#include "UI_Sprite.h"

#include "SDL/include/SDL.h"

enum ButtonType
{
	BUTTON_NONE = 0,
	//MAIN BUTTONS
	PLAY,
	CONTINUE,
	SETTINGS,
	CREDITS,
	EXIT,
	TOGGLE_FULLSCREEN,
	
	WEBPAGE,
	CLOSE_WINDOW,
	RESUME,
	TO_MAIN_SCENE,
};

enum MouseEvents
{
	MOUSE_NONE = 0,
	MOUSE_ENTER,
	MOUSE_LEAVE,
	MOUSE_CLICK,
	MOUSE_RELEASE
};

class UI_Button : public UI_Sprite
{
public:
	UI_Button(ButtonType type, iPoint pos, j1Module* callback, SDL_Rect idle, SDL_Rect hover, SDL_Rect disabled, SDL_Rect clicked);
	~UI_Button();

	bool Update();
	void Draw();

	bool MouseOver(const SDL_Rect& button);
	void SetSection(SDL_Rect idle_sec, SDL_Rect high_sec, SDL_Rect clicked_sec, SDL_Rect disabled_sec = { 0,0,0,0 });
	void SetButtonType(ButtonType type);
	void SetArea(uint w, uint h);

private:

	void ChangeVisualState(const int event);

public:

	ButtonType button_type;
	bool hovering = false;

private:

	bool enabled = true;
	uint click_sfx;

	SDL_Rect clickRect;
	SDL_Rect idleRect;
	SDL_Rect hoverRect;
	SDL_Rect disabledRect;
	SDL_Rect* currentRect = nullptr;

};
#endif //_UI_BUTTON_