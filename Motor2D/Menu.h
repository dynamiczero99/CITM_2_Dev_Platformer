#ifndef _MENU_
#define _MENU_

class Menu
{
public:
	Menu();
	~Menu();

	void Update();

	void Enable();
	void Disable();

private:

	bool enabled;
};

#endif //_MENU_