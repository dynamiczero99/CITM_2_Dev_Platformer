#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2DynArray.h"

#define CURSOR_WIDTH 2

#define WHITE (SDL_Color){255,255,255}
#define BLUE (SDL_Color){0,0,255}

// TODO 1: Create your structure of classes

// maybe we need a structures of predefined elements somewhere on xml and creates the guielements using it with simple gui methods
class GUIElement
{
public:

	enum class MOUSE_STATE
	{
		M_OUT = 0,
		M_ENTER,
		M_IN,//Same as hover
		M_EXIT,
		MAX
	};

public:

	int index = -1; // stores the position of this element on dynarray elements gui
	bool visible = true;
	bool draggable = false;
	//Position
	iPoint localPos = iPoint(0, 0);
	iPoint globalPos = iPoint(0, 0);//We also store the global position to avoid recalculating it, adding the parent's one
	//Tree structure
	GUIElement* parent;
	p2List<GUIElement*> childs;
	//Hovering control
	SDL_Rect bounds = SDL_Rect(0, 0, 0, 0); // stores "general" boundaries for mouse checking
	MOUSE_STATE guiState = MOUSE_STATE::M_OUT;
	uint hoverSFX;


public:
	GUIElement(const iPoint& position);
	virtual bool PreUpdate();
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void OnMouseHover();
	bool CheckBounds(int x, int y);
};

enum class GUI_ADJUST
{
	//INVALID = -1,
	//IN_LEFT_UP,
	//IN_MIDDLE_UP,
	//IN_RIGHT_UP,
	//IN_LEFT_CENTER,
	//IN_MIDDLE_CENTER,
	//IN_RIGHT_CENTER,
	//IN_LEFT_DOWN,
	//IN_MIDDLE_DOWN,
	//IN_RIGHT_DOWN,
	//OUT_UP_LEFT,
	//OUT_UP_MIDDLE,
	//OUT_UP_RIGHT,
	//OUT_LEFT_UP,
	//OUT_LEFT_MIDDLE,
	//OUT_LEFT_DOWN,
	//OUT_RIGHT_UP,
	//OUT_RIGHT_MIDDLE,
	//OUT_RIGHT_DOWN,
	//OUT_DOWN_LEFT,
	//OUT_DOWN_MIDDLE,
	//OUT_DOWN_RIGHT
};

class GUIImage : public GUIElement // image class that supports optional text
{
public:
	SDL_Rect section;// rect of the target "atlas" texture
public:
	GUIImage(SDL_Texture* texture, const iPoint& position);
	bool PostUpdate();
};

//class GUIText : public GUIElement
//{
//public:
//
//	GUIText() {}
//	GUIText(const iPoint& position, const char* text, SDL_Color color);
//
//public:
//
//	bool PreUpdate() { return true; }
//	bool PostUpdate();
//
//private:
//	SDL_Texture* texture = nullptr;
//
//};
//
//class GUIButton : public GUIImage//public GUIelement
//{
//public:
//
//	// maybe we need more different possibilites constructors
//	//GUIButton();
//	GUIButton(SDL_Texture* click_texture, SDL_Texture* unclick_texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, GUI_ADJUST targetPos = GUI_ADJUST::CENTERED, SDL_Texture* hoverTex = nullptr);
//
//	bool PreUpdate();
//
//private:
//
//protected:
//	SDL_Texture* clicked_texture = nullptr;
//	SDL_Texture* unclicked_texture = nullptr;
//	SDL_Texture* hover_texture = nullptr;
//	//Animation on guiState::HOVER
//	// 
//
//};
//
//class GUICheckBox : public GUIButton
//{
//public:
//
//	GUICheckBox(SDL_Texture* click_texture, SDL_Texture* unclick_texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, GUI_ADJUST targetPos = GUI_ADJUST::CENTERED, SDL_Texture* hoverTex = nullptr, SDL_Texture* checkTex = nullptr);
//
//	bool PreUpdate();
//	bool PostUpdate();
//
//private:
//
//	bool active = false;
//	SDL_Texture* checkTexture = nullptr;
//};



// ---------------------------------------------------
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

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Create GUI Objects
	GUIImage* AddGUIImage(SDL_Texture* texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, GUI_ADJUST targetTextPos = GUI_ADJUST::CENTERED);
	//GUIText* AddGUIText(const iPoint& position, const char* text, SDL_Color color);
	//GUIButton* AddGUIButton(SDL_Texture* clickedTexture, SDL_Texture* unclickTexture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, GUI_ADJUST targetTextPos = GUI_ADJUST::CENTERED, SDL_Texture* onMouseTex = nullptr);
	//GUICheckBox* AddGUICheckBox(SDL_Texture* clickedTexture, SDL_Texture* unclickTexture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, GUI_ADJUST targetTextPos = GUI_ADJUST::CENTERED, SDL_Texture* onMouseTex = nullptr, SDL_Texture* checkTex = nullptr);

	const SDL_Texture* GetAtlas() const;

public:
	// TODO: maybe adds a structure to pack all needed textures for specific button type
	// general buttons textures
	SDL_Texture* buttonup_texture = nullptr;
	SDL_Texture* buttondown_texture = nullptr;
	SDL_Texture* buttonhighlight_texture = nullptr;
	// general checkboxes textures
	SDL_Texture* checkbox_up_texture = nullptr;
	SDL_Texture* checkbox_down_texture = nullptr;
	SDL_Texture* checkbox_highlight_texture = nullptr;
	SDL_Texture* checkbox_check_texture = nullptr;

private:

	SDL_Texture* atlas = nullptr;
	p2SString atlas_file_name;
	// pass a node or save strings, if we delete a document node after awake phase
	// buttons filenames
	p2SString buttonup_filename;
	p2SString buttondown_filename;
	p2SString buttonhighlight_filename;
	// checkbox filenames
	p2SString checkbox_up_filename;
	p2SString checkbox_down_filename;
	p2SString checkbox_highlight_filename;
	p2SString checkbox_check_filename;
	//p2SString checkbox_check_locked_filename;

	p2DynArray<GUIElement*> elements = NULL;
	//GUIelement* elements[10] = { nullptr };
	//p2List<GUIelement*> elements;

};

#endif // __j1GUI_H__