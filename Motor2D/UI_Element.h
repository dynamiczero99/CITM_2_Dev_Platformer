#ifndef _UI_ELEMENT_
#define _UI_ELEMENT_

#include "p2Point.h"
#include "p2List.h"
#include "j1Module.h"

enum UiElemType
{
	NONE,
	SPRITE,
	LABEL,
	DYNAMICLABEL,
	BUTTON,
	SLIDER,
};

#include "SDL\include\SDL.h"

class UI_Element
{
public:
	UI_Element(UiElemType type, iPoint pos, j1Module* callback);
	UI_Element() {};
	virtual ~UI_Element();

	virtual bool PreUpdate() { return true; };
	virtual bool Update() { return true; };
	virtual bool CleanUp() { return true; };
	void Drag();

	void UpdateAttached();

	// Attach given element to this one
	void Attach(UI_Element* toAttach, const iPoint relativePosition);

private:

	virtual void Draw() { return; };

public:

	UiElemType			type;
	SDL_Rect			world_area;
	iPoint				position;

	bool beingClicked;
	bool draggable = false;

	p2List<UI_Element*> attachedUIElements;
	UI_Element* parent = nullptr;

protected:

	iPoint	relativePos;
	bool attached;
	j1Module * callback = nullptr;
	iPoint lastMousePos;
};

#endif //_UI_ELEMENT_