#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"

#include "j1Window.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	// button textures || if we put all of them on an atlas, is needed a fast code adaptation, we must think it good
	buttonup_filename = conf.child("button_up").attribute("file").as_string("");
	buttondown_filename = conf.child("button_down").attribute("file").as_string("");
	buttonhighlight_filename = conf.child("button_hover").attribute("file").as_string("");
	// checkbox textures
	checkbox_up_filename = conf.child("checkbox_up").attribute("file").as_string("");
	checkbox_down_filename = conf.child("checkbox_down").attribute("file").as_string("");
	checkbox_highlight_filename = conf.child("checkbox_highlight").attribute("file").as_string("");
	checkbox_check_filename = conf.child("checkbox_check").attribute("file").as_string("");
	//checkbox_check_locked_filename = conf.child("checkbox_check_locked").attribute("file").as_string("");


	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->LoadTexture(atlas_file_name.GetString());
	// buttons
	buttonup_texture = App->tex->LoadTexture(buttonup_filename.GetString());
	buttondown_texture = App->tex->LoadTexture(buttondown_filename.GetString());
	buttonhighlight_texture = App->tex->LoadTexture(buttonhighlight_filename.GetString());
	// sets blend mode additive to highlight texture
	SDL_SetTextureBlendMode(buttonhighlight_texture, SDL_BLENDMODE_ADD);
	// checkbox
	checkbox_up_texture = App->tex->LoadTexture(checkbox_up_filename.GetString());
	checkbox_down_texture = App->tex->LoadTexture(checkbox_down_filename.GetString());
	checkbox_highlight_texture = App->tex->LoadTexture(checkbox_highlight_filename.GetString());
	checkbox_check_texture = App->tex->LoadTexture(checkbox_check_filename.GetString());
	SDL_SetTextureBlendMode(checkbox_highlight_texture, SDL_BLENDMODE_ADD);

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	/*p2List_item<GUIelement*>* item = elements.start; // list test
	while (item)
	{
		item->data->PreUpdate();
		item = item->next;
	}*/

	for (int i = 0; i < elements.Count(); ++i)
	{
		GUIElement* e = *elements.At(i);

		// check and updates mouse state -----------------------
		int x, y = 0;
		App->input->GetMousePosition(x, y);
		x *= App->win->GetScale(); // workaround to get correct mouse pos
		y *= App->win->GetScale();
		/*LOG("mouse pos x:%i y:%i", x, y);
		LOG("button pos x:%i y:%i", e->boundaries.x, e->boundaries.y);*/
		if (e->CheckBoundariesXY(x, y))
		{
			e->SetMouseState(GUIElement::MOUSE_EVENT::ENTER);
		}
		else
		{
			if (e->guiState != GUIElement::MOUSE_STATE::EXIT && e->guiState != GUIElement::MOUSE_STATE::CLICK)
				e->SetMouseState(GUIElement::MOUSE_EVENT::EXIT);
		}
		// ------------------------------------------------------

		// call elements preupdate ------------------------------
		e->PreUpdate();
	}

	return true;
}

void GUIElement::SetMouseState(MOUSE_EVENT event)
{
	switch (event)
	{
	case MOUSE_EVENT::ENTER:
		if (guiState == MOUSE_STATE::ENTER)
		{
			LOG("Mouse is hovering");
			guiState = MOUSE_STATE::HOVER;
		}
		else if (guiState == MOUSE_STATE::DONTCARE || guiState == MOUSE_STATE::EXIT)
		{
			LOG("Mouse entered on boundaries");
			guiState = MOUSE_STATE::ENTER;
		}
		break;
	case MOUSE_EVENT::EXIT:
		LOG("Mouse Exit");
		guiState = MOUSE_STATE::EXIT;
		break;
	case MOUSE_EVENT::FAILED:
		break;
	default:
		LOG("unknown mouse event");
		break;
	}
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	for (int i = 0; i < elements.Count(); ++i)
	{
		GUIElement* e = *elements.At(i);
		e->PostUpdate();
	}

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	App->tex->UnloadTexture(atlas);
	App->tex->UnloadTexture(buttonup_texture);
	App->tex->UnloadTexture(buttondown_texture);
	App->tex->UnloadTexture(buttonhighlight_texture);

	elements.Clear(); // dynarray clears itselfs when destructor

	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

bool GUIElement::CheckBounds(int x, int y)
{
	return (x > bounds.x && x < (bounds.x + bounds.w) && y > bounds.y && y < (bounds.y + bounds.h));
}

void GUIElement::OnMouseHover()
{
}

// UIelements constructions
GUIImage* j1Gui::AddGUIBanner(SDL_Texture* texture, const SDL_Rect& rect, const iPoint& position, const char* text, GUI_ADJUST targetTextPos)
{
	GUIImage* ret = nullptr;
	ret = new GUIImage(texture, rect, position, text, targetTextPos);
	//elements.add(ret); // list method
	elements.PushBack(ret);
	// adds index
	ret->index = elements.Count();

	return ret;
}

GUIText* j1Gui::AddGUIText(const iPoint& position, const char* text, SDL_Color color)
{
	GUIText* ret = nullptr;
	ret = new GUIText(position, text, color);
	elements.PushBack(ret);
	// adds index
	ret->index = elements.Count();

	return ret;
}

GUIButton* j1Gui::AddGUIButton(SDL_Texture* clickTexture, SDL_Texture* unclickTexture, const SDL_Rect& rect, const iPoint& position, const char* text, TextPos targetTextPos, SDL_Texture* onMouseTex)
{
	GUIButton* ret = nullptr;
	ret = new GUIButton(clickTexture, unclickTexture, rect, position, text, targetTextPos, onMouseTex);
	elements.PushBack(ret);
	ret->index = elements.Count();

	return ret;
}

GUICheckBox* j1Gui::AddGUICheckBox(SDL_Texture* clickTexture, SDL_Texture* unclickTexture, const SDL_Rect& rect, const iPoint& position, const char* text, TextPos targetTextPos, SDL_Texture* onMouseTex, SDL_Texture* checkTex)
{
	GUICheckBox* ret = nullptr;
	ret = new GUICheckBox(clickTexture, unclickTexture, rect, position, text, targetTextPos, onMouseTex, checkTex);
	elements.PushBack(ret);
	ret->index = elements.Count();

	return ret;
}


// class Gui ---------------------------------------------------

GUIElement::GUIElement(const iPoint& position) : localPos(position) {}

bool GUIElement::PreUpdate()
{
	return true;
}
bool GUIElement::PostUpdate()
{
	return true;
}
bool GUIElement::CleanUp()
{
	return true;
}

//Text

GUIText::GUIText(const iPoint& position, const char* text, SDL_Color color) : GUIElement(position)
{
	if (text != nullptr)
	{
		texture = App->font->Print(text, color, NULL);
	}
}

bool GUIText::PostUpdate()
{
	if (texture != nullptr)
		App->render->BlitGUIUnscaled(texture, localPos.x, localPos.y, NULL);

	return true;
}

// GUIButton relative ============================================

GUIButton::GUIButton(SDL_Texture* click_texture, SDL_Texture* unclick_texture, const SDL_Rect& rect, const iPoint& position, const char* text, GUI_ADJUST targetPos, SDL_Texture* hoverTex)
	: clicked_texture(click_texture), unclicked_texture(unclick_texture), hover_texture(hoverTex), GUIImage(unclick_texture, rect, position, text, targetPos, hoverTex) {}

bool GUIButton::PreUpdate()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && guiState == MOUSE_STATE::HOVER)
	{
		image_texture = clicked_texture;
		LOG("button clicked");
		guiState = MOUSE_STATE::CLICK;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && guiState == MOUSE_STATE::CLICK) // always unclick
	{
		image_texture = unclicked_texture;
		LOG("button unclicked");
		guiState = MOUSE_STATE::DONTCARE;
	}

	return true;
}
// ===============================================================

// CHECKBOX relative =============================================

GUICheckBox::GUICheckBox(SDL_Texture* click_texture, SDL_Texture* unclick_texture, const SDL_Rect& rect, const iPoint& position, const char* text, GUI_ADJUST targetPos, SDL_Texture* hoverTex, SDL_Texture* checkTex)
	: checkTexture(checkTex), GUIButton(click_texture, unclick_texture, rect, position, text, targetPos, hoverTex)
{

}

bool GUICheckBox::PreUpdate()
{
	GUIButton::PreUpdate(); // calls overrided preUpdate from parent too

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && guiState == MOUSE_STATE::DONTCARE)
	{
		active = !active;
	}

	return true;
}

bool GUICheckBox::PostUpdate()
{
	//GUIBanner::PostUpdate(); // GUIButton doesnt had postupdate yet, if we need it, call parent of button(guibanner) on button postupdate, and here the button
	//Instead of calling the update of the parent, make an image and use its own update

	if (active && checkTexture != nullptr)
		App->render->BlitGUIUnscaled(checkTexture, localPos.x, localPos.y, NULL);

	return true;
}

//Image

GUIImage::GUIImage(SDL_Texture * texture, const iPoint & position)
{
}

bool GUIImage::PostUpdate()
{
	return false;
}
