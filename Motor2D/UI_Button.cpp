#include "j1App.h"
#include "p2Log.h"
#include "UI_Button.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Gui.h"

#include "SDL\include\SDL.h"


UI_Button::UI_Button(iPoint pos, j1Module* callback) : UI_Element(UiElemType::BUTTON, pos, callback)
{
	
}

UI_Button::~UI_Button()
{
}

bool UI_Button::PreUpdate(float d_time)
{
	bool ret = true;

	if (!enabled)
	{
		current_rect = &disabled_rect;
		return ret;
	}

	world_area = { position.x, position.y, current_rect->w, current_rect->h };

	if (!App->gui->dragging)
	{
		if (MouseOver(world_area) && !hovering)
		{
			hovering = true;
			ChangeVisualState(MOUSE_ENTER);
		}
		if (hovering && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			ChangeVisualState(MOUSE_CLICK);
			App->audio->PlayFx(click_sfx, 0, App->audio->sfx_vol);
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

	return ret;
}


void UI_Button::Draw()
{
	App->render->AddToBlitList(App->gui->GetAtlas(), position.x, position.y, current_rect);
}


bool UI_Button::MouseOver(const SDL_Rect& button)
{
	iPoint mouse_cords;
	App->input->GetMousePosition(mouse_cords.x, mouse_cords.y);
	mouse_cords = App->render->ScreenToWorld(mouse_cords.x, mouse_cords.y);

	return (mouse_cords.x >= button.x && mouse_cords.x <= button.x + button.w) && (mouse_cords.y >= button.y && mouse_cords.y <= button.y + button.h);
}

void UI_Button::SetSection(SDL_Rect idle_sec, SDL_Rect high_sec, SDL_Rect clicked_sec, SDL_Rect disabled_sec)
{
	idle_rect.x = idle_sec.x;
	idle_rect.y = idle_sec.y;
	idle_rect.w = idle_sec.w;
	idle_rect.h = idle_sec.h;

	highlighted_rect.x = high_sec.x;
	highlighted_rect.y = high_sec.y;
	highlighted_rect.w = high_sec.w;
	highlighted_rect.h = high_sec.h;

	click_rect.x = clicked_sec.x;
	click_rect.y = clicked_sec.y;
	click_rect.w = clicked_sec.w;
	click_rect.h = clicked_sec.h;

	if (disabled_sec.h != 0 && disabled_sec.w != 0)
	{
		disabled_rect.h = disabled_sec.h;
		disabled_rect.w = disabled_sec.w;
		disabled_rect.x = disabled_sec.x;
		disabled_rect.y = disabled_sec.y;

		enabled = false;
	}
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
		current_rect = &highlighted_rect; break;
	case MOUSE_CLICK:
		current_rect = &click_rect; break;
	case MOUSE_RELEASE:
		current_rect = &highlighted_rect; break;
	case MOUSE_LEAVE:
		current_rect = &idle_rect; break;
	}

	//	SetArea(current_rect->w, current_rect->h);
}