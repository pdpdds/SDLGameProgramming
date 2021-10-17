#pragma once

#include "game/Transition.h"
#include "game/layout/menu/Logo.h"
#include "game/layout/menu/MainMenuBtn.h"
#include "game/layout/menu/PieceRain.h"
#include "game/states/substates/MainMenu.h"

#include <array>
#include <memory>

class GameState;
class Music;
class SoundEffect;
class Texture;

enum class GameMode : uint8_t;


namespace SubStates {
namespace MainMenu {

class Base : public State {
public:
    Base(MainMenuState&, AppContext&);
    ~Base();
    void updateAnimationsOnly(MainMenuState&, AppContext&) final;
    void update(MainMenuState&, const std::vector<Event>&, AppContext&) final;
    void updatePositions(GraphicsContext&) final;
    void draw(MainMenuState&, GraphicsContext&) const final;

    void reloadTheme(MainMenuState&, AppContext&);

private:
    ::Rectangle screen_rect;
    std::unique_ptr<Texture> tex_background;
    std::unique_ptr<Layout::Logo> logo;
    std::array<Layout::PieceRain, 2> rains;
    ::Rectangle desc_rect;
    RGBAColor desc_panel_color;

    struct ButtonColumn {
        std::vector<Layout::MainMenuButton> buttons;
        std::vector<std::unique_ptr<Texture>> descriptions;
        size_t selected_index;

        ButtonColumn();
        void selectNext();
        void selectPrev();
        void activate();
    };
    ButtonColumn primary_buttons;
    ButtonColumn singleplayer_buttons;
    ButtonColumn multiplayer_buttons;
    ButtonColumn* current_column;
    Transition<float> column_slide_anim;

    void setColumnPosition(std::vector<Layout::MainMenuButton>&, int left_x, int center_y);
    void openSubcolumn(ButtonColumn*);
    void updateDescriptionRect();

    std::shared_ptr<Music> music;
    std::unique_ptr<Transition<uint8_t>> state_transition_alpha;

    void startGame(AppContext&, GameMode);
    void onFadeoutComplete(AppContext&, std::unique_ptr<GameState>&&);

    void reloadGameAssets(AppContext&);
    void reloadUI(MainMenuState&, AppContext&);
    void reloadMusic(AppContext&);
};

} // namespace MainMenu
} // namespace SubStates
