#ifndef _UI_SPRITE_
#define _UI_SPRITE_

#include "UI_Element.h"
#include "SDL\include\SDL.h"
class UI_Sprite : public UI_Element
{
public:
	UI_Sprite(SDL_Texture* tex, SDL_Rect* texSection, iPoint pos, j1Module* callback);
	UI_Sprite() {};
	virtual ~UI_Sprite();

	virtual bool Update();

private:

	virtual void Draw();

protected:
	SDL_Texture* uiAtlas = nullptr;
	SDL_Rect* atlasSection = nullptr;
};

#endif //_UI_SPRITE_