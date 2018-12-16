#include "j1App.h"
#include "UI_Slider.h"
#include "j1Render.h"

UI_Slider::UI_Slider(UiElemType type, iPoint pos, j1Module * callback, SDL_Rect texSection, UI_Sprite * _thumb) : UI_Sprite(type, pos, callback, texSection)
{
	thumb = _thumb;

	thumbMinPos = position.x;
	thumbMaxPos = position.x + atlasSection.x;
	thumbMidPos = (thumbMinPos + thumbMaxPos) / 2;
	middleHeight = (position.y + (position.y + atlasSection.y)) / 2;
	length = thumbMaxPos - thumbMinPos;

	thumbCenterFromCorner.create(atlasSection.x / 2, atlasSection.y / 2);

	iPoint newPos;
	newPos.create(thumbMidPos, middleHeight);

	newPos -= thumbCenterFromCorner;

	this->Attach(thumb, newPos);
	thumb->draggable = true;
}

UI_Slider::~UI_Slider()
{
}

void Update()
{

}

void UI_Slider::Draw()
{
	App->render->Blit(uiAtlas, position.x, position.y, &atlasSection);
	CheckThumbPosition();
}

void UI_Slider::CheckThumbPosition()
{
	iPoint thumbCenter = thumb->position - thumbCenterFromCorner;

	int relativeDistance = position.x - thumbCenter.x;

	percentile = length / relativeDistance;
}
