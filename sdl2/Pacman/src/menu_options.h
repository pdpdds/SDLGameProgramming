#ifndef MENU_OPTIONS
#define MENU_OPTIONS
#include "sounds.h"
#include "menu.h"

class MenuOptions : public Menu {
	public:
		MenuOptions();
		~MenuOptions();
		int show();
	private:
		enum Entries {BACK, FULLSCREEN, SOUND};
		void setMenuSelections();
		int handleSelection();
		void updateMenuItemNames();
};

#endif
