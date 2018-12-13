#ifndef _UI_BUTTON_
#define _UI_BUTTON_

#include "UI_Sprite.h"

#include "SDL/include/SDL.h"

enum ButtonType
{
	BUTTON_NONE = 0,
	PLAY,
	SETTINGS,
	TOGGLE_FULLSCREEN,
	CREDITS,
	EXIT,
	CLOSE_WINDOW,
	RESUME,
	TO_MAIN_SCENE,
	WEBPAGE,
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
	UI_Button(SDL_Rect idle, SDL_Rect hover, SDL_Rect disabled, SDL_Rect clicked, iPoint pos, j1Module* callback);
	~UI_Button();

	bool PreUpdate(float d_time);
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