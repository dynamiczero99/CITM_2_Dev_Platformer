#ifndef _UI_LABEL_
#define _UI_LABEL_

#include "UI_Element.h"
#include "j1Fonts.h"

#include "SDL/include/SDL.h"

class UI_Label : public UI_Element
{
public:
	UI_Label(iPoint pos, j1Module* callback);
	~UI_Label();

	bool CleanUp();
	void SetText(const char* content, const SDL_Color &color, _TTF_Font* font_size);
	void Draw();

private:
	void SetArea(uint w, uint h);

public:
	SDL_Texture * text_texture;
};
#endif //_UI_LABEL_