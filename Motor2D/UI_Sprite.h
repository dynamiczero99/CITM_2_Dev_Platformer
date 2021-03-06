#ifndef _UI_SPRITE_
#define _UI_SPRITE_

#include "UI_Element.h"
#include "SDL\include\SDL.h"
class UI_Sprite : public UI_Element
{
public:
	UI_Sprite(UiElemType type, iPoint pos, j1Module* callback, SDL_Rect texSection);
	UI_Sprite() {};
	virtual ~UI_Sprite();

	virtual bool Update();

private:

	virtual void Draw();

public:

	SDL_Rect atlasSection;

protected:
	SDL_Texture* uiAtlas = nullptr;
};

#endif //_UI_SPRITE_