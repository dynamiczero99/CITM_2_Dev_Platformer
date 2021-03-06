#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "UI_Element.h"
#include "UI_Sprite.h"
#include "UI_Slider.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_DynamicLabel.h"
#include "p2List.h"

#define CURSOR_WIDTH 2

class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Update
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	UI_Element* CreateUIElement(iPoint position, j1Module* callback);
	UI_Sprite* CreateSprite(iPoint position, j1Module* callback, SDL_Rect texSection);
	UI_Button* CreateButton(ButtonType type, iPoint position, j1Module* callback, SDL_Rect idleSection, SDL_Rect hoverSection, SDL_Rect clickSection, SDL_Rect disabledSection);
	UI_Slider* CreateSlider(iPoint position, j1Module* callback, SDL_Rect texSection, UI_Sprite* thumb, int* assignedNumber);
	UI_Label* CreateLabel(iPoint position, j1Module* callback);
	UI_DynamicLabel* CreateDynamicLabel(iPoint position, j1Module* callback);

	bool DestroyUIElement(UI_Element* toDestroyElem);
	//void DestroyAllUIElements();

	///*void OpenWindow();*/
	//void DestroyWindow();
	//bool visible = true;

	SDL_Texture* GetAtlas() const;
	void UI_Debug();


	iPoint title_pos;
	SDL_Rect title_Rect;
	//Big button rects
	SDL_Rect L_Button_Section;
	SDL_Rect L_Button_Hover;
	SDL_Rect L_Button_Clicked;
	SDL_Rect L_Button_Disabled;
	//Small buttons rects
	SDL_Rect S_Button_Section;
	SDL_Rect S_Button_Hover;
	SDL_Rect S_Button_Clicked;
	SDL_Rect S_Button_Disabled;
	//X button rects
	SDL_Rect X_Button_Section;
	SDL_Rect X_Button_Hover;
	SDL_Rect X_Button_Clicked;
	SDL_Rect X_Button_Disabled;

	SDL_Rect W_Button_Section;
	SDL_Rect W_Button_Hover;
	SDL_Rect W_Button_Clicked;
	SDL_Rect W_Button_Disabled;

public:

	bool dragging = false;
	p2List<UI_Element*> uiList;

private:

	bool debug = false;
	SDL_Texture* atlas;
	p2SString atlas_file_name;


};

#endif // __j1GUI_H__