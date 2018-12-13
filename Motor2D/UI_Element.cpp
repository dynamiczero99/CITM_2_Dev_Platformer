#include "j1App.h"
#include "UI_Element.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "p2Log.h"

UI_Element::UI_Element(UiElemType type, iPoint pos, j1Module* callback) : type(type), position(pos), callback(callback)
{
	
}

UI_Element::~UI_Element()
{

}

void UI_Element::Drag()
{
	world_area.x = position.x;
	world_area.y = position.y;

	if (!draggable)
		return;

	p2List_item<UI_Element*>* iterator = attachedUIElements.end;

	while (iterator)
	{
		if (iterator->data->beingClicked)
			return;

		iterator = iterator->prev;
	}

	// Check if element is clicked/click has been release, and modify bool accordingly
	SDL_Point temp_mousepos_sdl;
	App->input->GetMousePosition(temp_mousepos_sdl.x, temp_mousepos_sdl.y);
	temp_mousepos_sdl.x = App->render->ScreenToWorld(temp_mousepos_sdl.x, temp_mousepos_sdl.y).x;
	temp_mousepos_sdl.y = App->render->ScreenToWorld(temp_mousepos_sdl.x, temp_mousepos_sdl.y).y;
	if (!beingClicked && SDL_PointInRect(&temp_mousepos_sdl, &world_area) && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		beingClicked = true, LOG("Clicked = TRUE");
	if (beingClicked && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
		beingClicked = false, LOG("Clicked = FALSE");
	iPoint tempMousePos;
	tempMousePos.create(temp_mousepos_sdl.x, temp_mousepos_sdl.y);

	// Move element if it's clicked and if last mouse position is different from the new one
	if (!attached && beingClicked && lastMousePos != tempMousePos && lastMousePos.x != 0 && lastMousePos.y != 0)
	{
		position += tempMousePos - lastMousePos;
	}
	if (attached && beingClicked && lastMousePos != tempMousePos && lastMousePos.x != 0 && lastMousePos.y != 0)
	{
		relativePos += tempMousePos - lastMousePos;
	}
	if (beingClicked)
		lastMousePos = tempMousePos; App->gui->dragging = true;
	if (!beingClicked)
		App->gui->dragging = false;

}

void UI_Element::UpdateAttached()
{
	p2List_item<UI_Element*>* iterator = attachedUIElements.start;

	while (iterator)
	{
		iterator->data->position = position + iterator->data->relativePos;
		iterator = iterator->next;
	}
}

void UI_Element::Attach(UI_Element * toAttach, const iPoint relativePosition)
{
	toAttach->relativePos = relativePosition;
	toAttach->attached = true;
	attachedUIElements.add(toAttach);
}
