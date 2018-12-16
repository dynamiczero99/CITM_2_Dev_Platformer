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

	thumbCenterFromCorner.create(thumb->atlasSection.w / 2, thumb->atlasSection.h / 2);

	iPoint newPos;
	newPos.create(thumbMidPos - position.x, middleHeight - position.y);

	newPos -= thumbCenterFromCorner;

	this->Attach(thumb, newPos);
	thumb->draggable = true;
}

UI_Slider::~UI_Slider()
{
}

bool UI_Slider::Update()
{
	CheckThumbPosition();
	UpdateAttached();
	Draw();

	return true;
}

void UI_Slider::Draw()
{
	App->render->Blit(uiAtlas, position.x, position.y, &atlasSection);
}

void UI_Slider::CheckThumbPosition()
{
	iPoint thumbCenter = thumb->position - thumbCenterFromCorner;

	int relativeDistance = thumbCenter.x - position.x;

	percentile = (float)relativeDistance/(float)length;
}
