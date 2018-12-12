#include "UI_Element.h"

UI_Element::UI_Element(UiElemType type, iPoint pos, j1Module* callback) : type(type), position(pos), callback(callback)
{
	
}

UI_Element::~UI_Element()
{

}
