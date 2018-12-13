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
	if (textTexture != nullptr)
		App->tex->UnloadTexture(textTexture);
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
	textTexture = App->font->Print(content, color, font_size);
	int w, h;
	SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);
	SetArea(w, h);
}

bool UI_Label::Update()
{
	Draw();
	return true;
}

void UI_Label::Draw()
{
	App->render->Blit(textTexture, position.x, position.y);
}