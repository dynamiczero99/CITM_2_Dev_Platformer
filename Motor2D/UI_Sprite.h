#ifndef _UI_SPRITE_
#define _UI_SPRITE_

#include "UI_Element.h"
#include "SDL\include\SDL.h"
class UI_Sprite : public UI_Element
{
public:
	UI_Sprite(UiElemType type, SDL_Rect texSection, iPoint pos, j1Module* callback);
	UI_Sprite() {};
	virtual ~UI_Sprite();

	virtual bool Update();

private:

	virtual void Draw();

protected:
	SDL_Texture* uiAtlas = nullptr;
	SDL_Rect atlasSection;
};

#endif //_UI_SPRITE_