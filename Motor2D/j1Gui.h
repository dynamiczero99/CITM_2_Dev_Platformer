#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2DynArray.h"

#define CURSOR_WIDTH 2

#define WHITE (SDL_Color){255,255,255}
#define BLUE (SDL_Color){0,0,255}

// TODO 1: Create your structure of classes

// maybe we need a structures of predefined elements somewhere on xml and creates the guielements using it with simple gui methods
class GUIelement
{
public:

	enum MouseState
	{
		DONTCARE = 0,
		ENTER,
		HOVER,
		CLICK,
		EXIT,
		MAX
	};

	enum class MouseEvent
	{
		ENTER,
		EXIT,
		FAILED
	};

public:

	int index = -1; // stores the position of this element on dynarray elements gui
	iPoint position;
	SDL_Rect boundaries; // stores "general" boundaries for mouse checking
	MouseState guiState;

public:

	GUIelement();
	GUIelement(const iPoint& position);

	virtual bool PreUpdate();
	virtual bool PostUpdate();
	virtual bool CleanUp();

	bool CheckBoundariesXY(int x, int y);
	void SetMouseState(MouseEvent event);

};

enum class TextPos
{
	NO,
	CENTERED,
	UP,
	BOTTOM,
	LEFT,
	RIGHT
};

class GUIBanner : public GUIelement // image class that supports optional text
{
public:

	/*enum TextPos
	{
		NO,
		CENTERED,
		UP,
		BOTTOM,
		LEFT,
		RIGHT
	};*/

	// maybe we need more different possibilites constructors
	GUIBanner(SDL_Texture* texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, TextPos targetPos = TextPos::CENTERED, SDL_Texture* onMouseTex = nullptr);

public:

	bool PreUpdate();
	bool PostUpdate();

protected:

	SDL_Texture* image_texture = nullptr;
	SDL_Texture* text_texture = nullptr; // text if the image has any text to show
	SDL_Texture* onMouse_texture = nullptr; // maybe the banner can have a hover texture or animation too
	SDL_Rect section; // rect of the target "atlas" texture
	iPoint textPosition; // position of the text, if has any

private:

	iPoint ReturnTextPosition(TextPos targetPos);

};

class GUIText : public GUIelement
{
public:

	GUIText() {}
	GUIText(const iPoint& position, const char* text, SDL_Color color);

public:

	bool PreUpdate() { return true; }
	bool PostUpdate();

private:
	SDL_Texture* texture = nullptr;

};

class GUIButton : public GUIBanner//public GUIelement
{
public:

	// maybe we need more different possibilites constructors
	//GUIButton();
	GUIButton(SDL_Texture* click_texture, SDL_Texture* unclick_texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, TextPos targetPos = TextPos::CENTERED, SDL_Texture* hoverTex = nullptr);

	bool PreUpdate();

private:

protected:
	SDL_Texture* clicked_texture = nullptr;
	SDL_Texture* unclicked_texture = nullptr;
	SDL_Texture* hover_texture = nullptr;
	//Animation on guiState::HOVER
	// 

};

class GUICheckBox : public GUIButton
{
public:

	GUICheckBox(SDL_Texture* click_texture, SDL_Texture* unclick_texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, TextPos targetPos = TextPos::CENTERED, SDL_Texture* hoverTex = nullptr, SDL_Texture* checkTex = nullptr);

	bool PreUpdate();
	bool PostUpdate();

private:

	bool active = false;
	SDL_Texture* checkTexture = nullptr;
};



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

	// TODO 2: Create the factory methods
	// Gui creation functions
	GUIBanner* AddGUIBanner(SDL_Texture* texture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, TextPos targetTextPos = TextPos::CENTERED);
	GUIText* AddGUIText(const iPoint& position, const char* text, SDL_Color color);
	GUIButton* AddGUIButton(SDL_Texture* clickedTexture, SDL_Texture* unclickTexture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, TextPos targetTextPos = TextPos::CENTERED, SDL_Texture* onMouseTex = nullptr);
	GUICheckBox* AddGUICheckBox(SDL_Texture* clickedTexture, SDL_Texture* unclickTexture, const SDL_Rect& rect, const iPoint& position, const char* text = nullptr, TextPos targetTextPos = TextPos::CENTERED, SDL_Texture* onMouseTex = nullptr, SDL_Texture* checkTex = nullptr);

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

	p2DynArray<GUIelement*> elements = NULL;
	//GUIelement* elements[10] = { nullptr };
	//p2List<GUIelement*> elements;

};

#endif // __j1GUI_H__