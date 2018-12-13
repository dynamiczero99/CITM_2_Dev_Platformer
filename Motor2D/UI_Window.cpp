#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "UI_Window.h"

#include "p2Log.h"

UI_Window::UI_Window(iPoint pos, j1Module* callback) : UI_Sprite(UiElemType::WINDOW, pos, callback)
{
	//READ FROM XML
	horizontalWindow.x = 3;
	horizontalWindow.y = 645;
	horizontalWindow.h = 192;
	horizontalWindow.w = 414;

	verticalWindow.x = 647;
	verticalWindow.y = 319;
	verticalWindow.h = 304;
	verticalWindow.w = 272;

	titleWindow.x = 647;
	titleWindow.y = 645;
	titleWindow.h = 60;
	titleWindow.w = 204;

	horizontalWindowSmall.x = 647;
	horizontalWindowSmall.y = 706;
	horizontalWindowSmall.h = 149;
	horizontalWindowSmall.w = 302;

}

UI_Window::~UI_Window()
{
}

void UI_Window::Draw()
{
	App->render->AddToBlitList(uiAtlas, position.x, position.y, atlasSection);
}


void UI_Window::SetArea(uint w, uint h)
{
	world_area.w = w;
	world_area.h = h;
}

void UI_Window::SetWindowType(WindowType type)
{
	switch (type)
	{
	case HORIZONTAL_WINDOW:
		atlasSection = &horizontalWindow;
		break;
	case VERTICAL_WINDOW:
		atlasSection = &verticalWindow;
		break;
	case TITLE_WINDOW:
		atlasSection = &titleWindow;
		break;
	case HORIZONTAL_WINDOW_S:
		atlasSection = &horizontalWindowSmall;
		break;
	default:
		LOG("Window Type does not exist");
		break;
	}

	SetArea(atlasSection->w, atlasSection->h);
}