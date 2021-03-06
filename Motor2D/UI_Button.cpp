#include "j1App.h"
#include "p2Log.h"
#include "UI_Button.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"

#include "SDL\include\SDL.h"


UI_Button::UI_Button(ButtonType type, iPoint pos, j1Module* callback, SDL_Rect idle, SDL_Rect hover, SDL_Rect disabled, SDL_Rect clicked) : UI_Sprite(UiElemType::BUTTON, pos, callback, idle)
{
	button_type = type;

	idleRect = atlasSection;
	hoverRect = hover;
	clickRect = clicked;
	disabledRect = disabled;

	currentRect = &idleRect;
}

UI_Button::~UI_Button()
{
}

bool UI_Button::Update()
{
	bool ret = true;

	if (!enabled)
	{
		currentRect = &disabledRect;
		ChangeVisualState(MOUSE_DISABLE);
		
	}

	world_area = { position.x, position.y, currentRect->w, currentRect->h };

	if (!App->gui->dragging && enabled)
	{
		if (MouseOver(world_area) && !hovering)
		{
			hovering = true;
			ChangeVisualState(MOUSE_ENTER);
		}
		if (hovering && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			ChangeVisualState(MOUSE_CLICK);
			App->audio->PlayFx(click_sfx, 0/*, App->audio->sfx_vol*/);
		}
		if (hovering && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
		{
			ChangeVisualState(MOUSE_RELEASE);
			ret = callback->OnEvent(this);
		}
		if (!MouseOver(world_area) && hovering)
		{
			hovering = false;
			ChangeVisualState(MOUSE_LEAVE);
		}
	}
	Drag();
	UpdateAttached();
	Draw();

	return ret;
}

void UI_Button::Draw()
{
		App->render->Blit(uiAtlas, App->render->ScreenToWorld(position.x, position.y).x, App->render->ScreenToWorld(position.x, position.y).y, currentRect);
}

bool UI_Button::MouseOver(const SDL_Rect& button)
{
	
	iPoint mouse_cords;
	App->input->GetMousePosition(mouse_cords.x, mouse_cords.y);
	//mouse_cords = App->render->ScreenToWorld(mouse_cords.x, mouse_cords.y);

	return (mouse_cords.x >= button.x && mouse_cords.x <= button.x + button.w) 
		&& (mouse_cords.y >= button.y && mouse_cords.y <= button.y + button.h);
}

void UI_Button::SetSection(SDL_Rect idle_sec, SDL_Rect high_sec, SDL_Rect clicked_sec, SDL_Rect disabled_sec)
{
	idleRect.x = idle_sec.x;
	idleRect.y = idle_sec.y;
	idleRect.w = idle_sec.w;
	idleRect.h = idle_sec.h;

	hoverRect.x = high_sec.x;
	hoverRect.y = high_sec.y;
	hoverRect.w = high_sec.w;
	hoverRect.h = high_sec.h;

	clickRect.x = clicked_sec.x;
	clickRect.y = clicked_sec.y;
	clickRect.w = clicked_sec.w;
	clickRect.h = clicked_sec.h;
	
	disabledRect.h = disabled_sec.h;
	disabledRect.w = disabled_sec.w;
	disabledRect.x = disabled_sec.x;
	disabledRect.y = disabled_sec.y;

	enabled = false;
	
}


void UI_Button::SetButtonType(ButtonType type)
{
	button_type = type;
}

void UI_Button::SetArea(uint w, uint h)
{
	world_area.w = w;
	world_area.h = h;
}

void UI_Button::ChangeVisualState(const int event)
{
	switch (event)
	{
	case MOUSE_ENTER:
		currentRect = &hoverRect; break;
	case MOUSE_CLICK:
		currentRect = &clickRect; break;
	case MOUSE_RELEASE:
		currentRect = &hoverRect; break;
	case MOUSE_LEAVE:
		currentRect = &idleRect; break;
	case MOUSE_DISABLE:
		currentRect = &disabledRect; break;
	}

	//SetArea(current_rect->w, current_rect->h);
}