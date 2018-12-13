#include "j1App.h"
#include "UI_Label.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"

#include "p2Log.h"


UI_Label::UI_Label(iPoint pos, j1Module* callback) : UI_Element(UiElemType::LABEL, pos, callback)
{
}

UI_Label::~UI_Label()
{
	if (text_texture != nullptr)
		App->tex->UnloadTexture(text_texture);
}

void UI_Label::SetArea(uint w, uint h)
{
	world_area.w = w;
	world_area.h = h;
}

bool UI_Label::CleanUp()
{
	return true;
}

void UI_Label::SetText(const char* content, const SDL_Color &color, _TTF_Font* font_size)
{
	text_texture = App->font->Print(content, color, font_size);
	int w, h;
	SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
	SetArea(w, h);
}

void UI_Label::Draw()
{
	App->render->AddToBlitList(text_texture, position.x, position.y);
}