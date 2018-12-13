#include "j1App.h"
#include "UI_DynamicLabel.h"
#include "j1Fonts.h"
#include "j1Textures.h"


UI_DynamicLabel::UI_DynamicLabel(iPoint pos, j1Module* callback) : UI_Label(pos, callback)
{}


UI_DynamicLabel::~UI_DynamicLabel()
{
}

void UI_DynamicLabel::ChangeContent(const char* new_content)
{
	if (text_texture != nullptr)
		App->tex->UnloadTexture(text_texture);

	text_texture = App->font->Print(new_content);
	int w, h;
	SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
	SetArea(w, h);
}

void UI_DynamicLabel::SetArea(uint w, uint h)
{
	world_area.w = w;
	world_area.h = h;
}