#ifndef _UI_DYNAMICLABEL_
#define _UI_DYNAMICLABEL_

#include "p2SString.h"
#include "UI_Label.h"

#include "SDL\include\SDL.h"

class UI_DynamicLabel : public UI_Label
{
public:
	UI_DynamicLabel(iPoint pos, j1Module* callback);
	~UI_DynamicLabel();

	void ChangeContent(const char* new_content);

private:
	void SetArea(uint w, uint h);


};
#endif //_UI_DYNAMICLABEL_