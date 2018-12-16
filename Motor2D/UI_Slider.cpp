#include "j1App.h"
#include "UI_Slider.h"
#include "j1Render.h"

UI_Slider::UI_Slider(UiElemType type, iPoint pos, j1Module * callback, SDL_Rect texSection, UI_Sprite * _thumb, int* assignedNumber) : UI_Sprite(type, pos, callback, texSection)
{
	thumb = _thumb;

	thumbMinPos = position.x;
	thumbMaxPos = position.x + atlasSection.w;
	thumbMidPos = (thumbMinPos + thumbMaxPos) / 2;
	middleHeight = (position.y + (position.y + atlasSection.h)) / 2;
	length = thumbMaxPos - thumbMinPos;

	thumbCenterFromCorner.create(thumb->atlasSection.w / 2, thumb->atlasSection.h / 2);

	iPoint newPos;
	newPos.create(thumbMidPos - position.x, middleHeight - position.y);

	newPos -= thumbCenterFromCorner;

	this->Attach(thumb, newPos);

	keepHeight = middleHeight - (thumb->atlasSection.h/2);

	defaultTargetNumber = *assignedNumber;
	targetNumber = assignedNumber;

	thumb->draggable = true;
}

UI_Slider::~UI_Slider()
{
}

bool UI_Slider::Update()
{
	UpdateAttached();
	CheckThumbPosition();
	Draw();

	return true;
}

void UI_Slider::Draw()
{
	App->render->Blit(uiAtlas, App->render->ScreenToWorld(position.x, position.y).x, App->render->ScreenToWorld(position.x, position.y).y, &atlasSection);
}

void UI_Slider::CheckThumbPosition()
{
	thumb->position.y = keepHeight;
	KeepBoundaries();

	iPoint thumbCenter = thumb->position - thumbCenterFromCorner;

	int relativeDistance = thumbCenter.x - position.x;

	percentile = (float)relativeDistance/(float)length;

	*targetNumber = defaultTargetNumber * percentile;
}

void UI_Slider::KeepBoundaries()
{
	if (thumb->position.x > thumbMaxPos)
		thumb->position.x = thumbMaxPos;

	else if (thumb->position.x < thumbMinPos)
		thumb->position.x = thumbMinPos;
}
