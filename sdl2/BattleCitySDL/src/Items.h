#ifndef ITEMS_H
#define ITEMS_H

#include <list>
#include "Sprites.h"
#include "Types.h"

using namespace std;

struct Item {
    int x, y;
    double frameDuration;
    bool isVisible;
    bool destroyed;
    ItemType type;
    SpriteData sprite;
};

class Items {
public:
    Items() { DestroyItem(); }
    void CreateItem();
    void DestroyItem() { currentItem.destroyed = true; }
    void Draw();
    void Update(double dt);

    Item GetCurrentItem() { return currentItem; }

private:
    // there should be no more than one item in the map at one time
    Item currentItem;
};

#endif // ITEMS_H
