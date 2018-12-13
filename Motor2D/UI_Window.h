#ifndef _UI_WINDOW_
#define _UI_WINDOW_

#include "UI_Sprite.h"
#include "SDL\include\SDL.h"

enum WindowType
{
	HORIZONTAL_WINDOW,
	VERTICAL_WINDOW,
	HORIZONTAL_WINDOW_S,
	TITLE_WINDOW
};

class UI_Window : public UI_Sprite
{
public:
	UI_Window(iPoint pos, j1Module* callback);
	~UI_Window();

	void Draw();
	void SetWindowType(WindowType type);

private:
	void SetArea(uint w, uint h);

public:

	SDL_Rect verticalWindow;
	SDL_Rect horizontalWindow;
	SDL_Rect titleWindow;
	SDL_Rect horizontalWindowSmall;
};

#endif //_UI_WINDOW_