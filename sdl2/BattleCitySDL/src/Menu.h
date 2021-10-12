#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "Sprites.h"

using namespace std;

class Menu {
public:
    Menu() : selectedItem(1) { }

    void Init();
    void ProcessEvents();
    void DrawMenu();
    void SelectItem(int id) { selectedItem = id; }
    void ChooseItem();

    int SelectedItem() { return selectedItem; }

private:
    int selectedItem;
    SpriteData backgroundSprite;
    SpriteData menuOptionsSprite;
    SpriteData pointerSprite;
    SpriteData logoSprite;
};

#endif // MENU_H
