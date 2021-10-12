#include "Items.h"
#include "Game.h"

using namespace std;

void Items::Draw() {
    static int tail_size = Game::instance().TailSize();
    if (currentItem.destroyed == false) {
        Game::instance().GetRenderer()->DrawSprite(currentItem.sprite, 0, currentItem.x * tail_size, currentItem.y * tail_size, tail_size * 2, tail_size * 2);
    }
}

void Items::Update(double dt) {
    if (currentItem.destroyed == false) {
        currentItem.frameDuration += dt;
        if (currentItem.frameDuration >= currentItem.sprite.frameDuration) {
            currentItem.isVisible = !currentItem.isVisible;
            currentItem.frameDuration = 0;
        }
    }
}

void Items::CreateItem() {
    currentItem.x = rand() % 21 + 2;
    currentItem.y = rand() % 21 + 2;
    currentItem.type = ItemType(rand() % 6);
    currentItem.frameDuration = 0.0;
    currentItem.isVisible = true;
    currentItem.destroyed = false;
    switch (currentItem.type) {
    case ITEM_STAR:
        currentItem.sprite = Game::instance().GetSprites()->Get("item_star"); break;
    case ITEM_LIFE:
        currentItem.sprite = Game::instance().GetSprites()->Get("item_life"); break;
    case ITEM_INVINCIBILITY:
        currentItem.sprite = Game::instance().GetSprites()->Get("item_invincibility"); break;
    case ITEM_PROTECT:
        currentItem.sprite = Game::instance().GetSprites()->Get("item_protect"); break;
    case ITEM_TIME:
        currentItem.sprite = Game::instance().GetSprites()->Get("item_time"); break;
    case ITEM_GRANADE:
        currentItem.sprite = Game::instance().GetSprites()->Get("item_granade"); break;
    }
}
