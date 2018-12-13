#include "UI_Sprite.h"
#include "j1App.h"
#include "j1Render.h"

UI_Sprite::UI_Sprite(SDL_Texture* tex, SDL_Rect* texSection, iPoint pos, j1Module* callback) : UI_Element(SPRITE, pos, callback)
{
	this->uiAtlas = tex;
	this->atlasSection = texSection;
}

UI_Sprite::~UI_Sprite()
{

}

bool UI_Sprite::Update()
{
	Draw();
	return true;
}

void UI_Sprite::Draw()
{
	App->render->AddToBlitList(uiAtlas, position.x, position.y, atlasSection);
}
