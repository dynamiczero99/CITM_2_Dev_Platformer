#include "UI_Sprite.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"

UI_Sprite::UI_Sprite(UiElemType type, iPoint pos, j1Module* callback, SDL_Rect texSection) : UI_Element(type, pos, callback)
{
	uiAtlas = App->gui->GetAtlas();
	this->atlasSection = texSection;
}

UI_Sprite::~UI_Sprite()
{

}

bool UI_Sprite::Update()
{
	Drag();
	UpdateAttached();
	Draw();
	return true;
}

void UI_Sprite::Draw()
{
	App->render->Blit(uiAtlas, position.x, position.y, &atlasSection);
}
