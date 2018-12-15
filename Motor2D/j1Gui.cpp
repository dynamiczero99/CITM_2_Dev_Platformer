#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Scene.h"
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

	title_pos.x = conf.child("title_coords").attribute("x").as_int();
	title_pos.y = conf.child("title_coords").attribute("y").as_int();

	title_Rect.x = conf.child("title_rect").attribute("x").as_int();
	title_Rect.y = conf.child("title_rect").attribute("y").as_int();
	title_Rect.w = conf.child("title_rect").attribute("w").as_int();
	title_Rect.h = conf.child("title_rect").attribute("h").as_int();
	//Large button
	L_Button_Section.x = conf.child("L_Button_Section").attribute("x").as_int();
	L_Button_Section.y = conf.child("L_Button_Section").attribute("y").as_int();
	L_Button_Section.w = conf.child("L_Button_Section").attribute("w").as_int();
	L_Button_Section.h = conf.child("L_Button_Section").attribute("h").as_int();

	L_Button_Hover.x = conf.child("L_Button_Hover").attribute("x").as_int();
	L_Button_Hover.y = conf.child("L_Button_Hover").attribute("y").as_int();
	L_Button_Hover.w = conf.child("L_Button_Hover").attribute("w").as_int();
	L_Button_Hover.h = conf.child("L_Button_Hover").attribute("h").as_int();

	L_Button_Clicked.x = conf.child("L_Button_Clicked").attribute("x").as_int();
	L_Button_Clicked.y = conf.child("L_Button_Clicked").attribute("y").as_int();
	L_Button_Clicked.w = conf.child("L_Button_Clicked").attribute("w").as_int();
	L_Button_Clicked.h = conf.child("L_Button_Clicked").attribute("h").as_int();
	//Needs to find coords
	L_Button_Disabled.x = conf.child("L_Button_Disabled").attribute("x").as_int();
	L_Button_Disabled.y = conf.child("L_Button_Disabled").attribute("y").as_int();
	L_Button_Disabled.w = conf.child("L_Button_Disabled").attribute("w").as_int();
	L_Button_Disabled.h = conf.child("L_Button_Disabled").attribute("h").as_int();

	//---------------SMALL BUTTON-------------------
	S_Button_Section.x = conf.child("S_Button_Section").attribute("x").as_int();
	S_Button_Section.y = conf.child("S_Button_Section").attribute("y").as_int();
	S_Button_Section.w = conf.child("S_Button_Section").attribute("w").as_int();
	S_Button_Section.h = conf.child("S_Button_Section").attribute("h").as_int();

	S_Button_Hover.x = conf.child("S_Button_Hover").attribute("x").as_int();
	S_Button_Hover.y = conf.child("S_Button_Hover").attribute("y").as_int();
	S_Button_Hover.w = conf.child("S_Button_Hover").attribute("w").as_int();
	S_Button_Hover.h = conf.child("S_Button_Hover").attribute("h").as_int();

	S_Button_Clicked.x = conf.child("S_Button_Clicked").attribute("x").as_int();
	S_Button_Clicked.y = conf.child("S_Button_Clicked").attribute("y").as_int();
	S_Button_Clicked.w = conf.child("S_Button_Clicked").attribute("w").as_int();
	S_Button_Clicked.h = conf.child("S_Button_Clicked").attribute("h").as_int();

	S_Button_Disabled.x = conf.child("S_Button_Disabled").attribute("x").as_int();
	S_Button_Disabled.y = conf.child("S_Button_Disabled").attribute("y").as_int();
	S_Button_Disabled.w = conf.child("S_Button_Disabled").attribute("w").as_int();
	S_Button_Disabled.h = conf.child("S_Button_Disabled").attribute("h").as_int();
	//--------------EXIT BUTTON-----------------
	X_Button_Section.x = conf.child("X_Button_Section").attribute("x").as_int();
	X_Button_Section.y = conf.child("X_Button_Section").attribute("y").as_int();
	X_Button_Section.w = conf.child("X_Button_Section").attribute("w").as_int();
	X_Button_Section.h = conf.child("X_Button_Section").attribute("h").as_int();

	X_Button_Hover.x = conf.child("X_Button_Hover").attribute("x").as_int();
	X_Button_Hover.y = conf.child("X_Button_Hover").attribute("y").as_int();
	X_Button_Hover.w = conf.child("X_Button_Hover").attribute("w").as_int();
	X_Button_Hover.h = conf.child("X_Button_Hover").attribute("h").as_int();

	X_Button_Clicked.x = conf.child("X_Button_Clicked").attribute("x").as_int();
	X_Button_Clicked.y = conf.child("X_Button_Clicked").attribute("y").as_int();
	X_Button_Clicked.w = conf.child("X_Button_Clicked").attribute("w").as_int();
	X_Button_Clicked.h = conf.child("X_Button_Clicked").attribute("h").as_int();
	//----------------WEB/GITHUB BUTTON--------------
	W_Button_Section.x = conf.child("W_Button_Section").attribute("x").as_int();
	W_Button_Section.y = conf.child("W_Button_Section").attribute("y").as_int();
	W_Button_Section.w = conf.child("W_Button_Section").attribute("w").as_int();
	W_Button_Section.h = conf.child("W_Button_Section").attribute("h").as_int();

	W_Button_Hover.x = conf.child("W_Button_Hover").attribute("x").as_int();
	W_Button_Hover.y = conf.child("W_Button_Hover").attribute("y").as_int();
	W_Button_Hover.w = conf.child("W_Button_Hover").attribute("w").as_int();
	W_Button_Hover.h = conf.child("W_Button_Hover").attribute("h").as_int();

	W_Button_Clicked.x = conf.child("W_Button_Clicked").attribute("x").as_int();
	W_Button_Clicked.y = conf.child("W_Button_Clicked").attribute("y").as_int();
	W_Button_Clicked.w = conf.child("W_Button_Clicked").attribute("w").as_int();
	W_Button_Clicked.h = conf.child("W_Button_Clicked").attribute("h").as_int();

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->LoadTexture(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

bool j1Gui::Update(float dt)
{

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	/*if (App->scene->pauseGame)
	{
		App->render->DrawQuad({ 0, 0, (int)App->win->width, (int)App->win->height }, 0, 0, 0, 150, true, false);
	}*/

	for (p2List_item<UI_Element*>* iterator = uiList.start; iterator; iterator = iterator->next)
	{
		iterator->data->Update();
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

UI_Element * j1Gui::CreateUIElement(iPoint position, j1Module * callback)
{
	UI_Element* ret = nullptr;

	ret = new UI_Element();

	if (ret != nullptr)
	uiList.add(ret);

	return ret;
}

UI_Sprite * j1Gui::CreateSprite(iPoint position, j1Module * callback, SDL_Rect texSection)
{
	UI_Element* ret = nullptr;

	ret = new UI_Sprite(SPRITE, position, callback, texSection);

	if (ret != nullptr)
		uiList.add(ret);

	return (UI_Sprite*)ret;
}



UI_Button * j1Gui::CreateButton(ButtonType type, iPoint position, j1Module * callback, SDL_Rect idleSection, SDL_Rect hoverSection, SDL_Rect disabledSection, SDL_Rect clickSection)
{
	UI_Element* ret = nullptr;

	ret = new UI_Button(type, position, callback, idleSection, hoverSection, disabledSection, clickSection);

	if (ret != nullptr)
		uiList.add(ret);

	return (UI_Button*)ret;
}

UI_Label * j1Gui::CreateLabel(iPoint position, j1Module * callback)
{
	UI_Element* ret = nullptr;

	ret = new UI_Label(position, callback);

	if (ret != nullptr)
		uiList.add(ret);

	return (UI_Label*)ret;
}

UI_DynamicLabel * j1Gui::CreateDynamicLabel(iPoint position, j1Module * callback)
{
	UI_Element* ret = nullptr;

	ret = new UI_DynamicLabel(position, callback);

	if (ret != nullptr)
		uiList.add(ret);

	return (UI_DynamicLabel*)ret;
}

bool j1Gui::DestroyUIElement(UI_Element * toDestroyElem)
{
	bool ret = true;

	if (toDestroyElem == nullptr)
		ret = false;

	p2List_item<UI_Element*>* temp = uiList.start;

	// Iterate the whole list to find the element that needs to be destroyed
	while (temp && ret)
	{
		if (temp->data == toDestroyElem)
		{
			// Call recursive function in order to delete also the attached "children" elements of toDestroyElem
			if (temp->data->attachedUIElements.start)
			{
				p2List_item<UI_Element*>* attachedIterator = temp->data->attachedUIElements.start;
				while (attachedIterator && temp)
				{
					DestroyUIElement(attachedIterator->data);
					attachedIterator = attachedIterator->next;
				}
			}
			delete toDestroyElem;
			uiList.del(temp);
			break;
		}
		temp = temp->next;
	}

	return ret;
}

void j1Gui::DestroyAllUIElements()
{
	for (p2List_item<UI_Element*>* tmp = uiList.start; tmp != nullptr; tmp = tmp->next)
	{
		uiList.del(tmp);
		RELEASE(tmp->data);
	}
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::UI_Debug()
{
}

void j1Gui::DestroyWindow()
{
	for (p2List_item<UI_Element*>* tmp = uiList.start; tmp != nullptr; tmp = tmp->next)
	{
		if (tmp->data->type == UiElemType::SPRITE && tmp->data == App->scene->window_to_close)
		{
			/*DestroyUIElement(tmp->data);*/
			DestroyAllUIElements();
			App->scene->CreateWidgets();
		}
		

	}
	//This is to close more than 1 window during the game's execution
	App->scene->active_window = false;
	App->scene->window_to_close = nullptr;

}

// class Gui ---------------------------------------------------
