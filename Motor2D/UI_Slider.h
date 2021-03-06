#ifndef _UI_SLIDER_
#define _UI_SLIDER_

#include "UI_Sprite.h"
#include "UI_Button.h"
class UI_Slider : public UI_Sprite
{
public:
	UI_Slider(UiElemType type, iPoint pos, j1Module* callback, SDL_Rect texSection, UI_Sprite* _thumb, int* assignedNumber);
	~UI_Slider();

	bool Update();

	void CheckThumbPosition();

public:

	UI_Sprite* thumb;
	int thumbMaxPos, thumbMinPos, thumbMidPos, middleHeight, length;
	iPoint thumbCenterFromCorner;
	int keepHeight;
	void Draw();
	void KeepBoundaries();
	int defaultTargetNumber;
	int* targetNumber;

	float percentile;
	
};

#endif //_UI_SLIDER_