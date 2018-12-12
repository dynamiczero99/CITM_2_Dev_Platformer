#ifndef _UI_WINDOW_
#define _UI_WINDOW_

#include "UI_Element.h"
#include "SDL\include\SDL.h"
class UI_Sprite : public UI_Element
{
public:
	UI_Sprite(SDL_Texture* tex, SDL_Rect* texSection, iPoint pos, j1Module* callback);
	virtual ~UI_Sprite();

	virtual bool Update();

private:

	virtual void Draw();

protected:
	SDL_Texture* tex;
	SDL_Rect* texSection;
};

#endif //_UI_WINDOW_