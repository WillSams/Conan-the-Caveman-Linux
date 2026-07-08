#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include <stormengine2/assetStore.h>
#include <stormengine2/gameStateMachine.h>
#include <stormengine2/logger.h>
#include <stormengine2/states/gameState.h>

#include "../input/gamepad.h"
#include "../ui/uiButton.h"

// Game over: blinking "Game Over" text plus main-menu / restart buttons from
// the GAMEOVER section of conan.xml.
class GameOverState : public GameState {
public:
    GameOverState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
                  bool isDebugging, AssetStore *assetStore, bool &isRunning,
                  GameStateMachine &machine);
    ~GameOverState();

    void processInput() override;
    void update()       override;
    void render()       override;
    bool onEnter()      override;
    bool onExit()       override;
    std::string getStateID() const override { return "GAMEOVER"; }

private:
    SDL_Renderer     *renderer_;
    int               windowWidth_, windowHeight_;
    bool              isDebugging_;
    AssetStore       *assetStore_;
    Logger            logger_;
    bool             &isRunning_;
    GameStateMachine &machine_;

    void Activate(int callbackId);

    std::vector<UiButton> buttons_;

    // Blinking title (2-frame sheet)
    SDL_Rect titleDst_   = {200, 100, 190, 30};
    int      titleFrame_ = 0;
    float    titleTimer_ = 0.f;

    float mouseX_ = 0.f, mouseY_ = 0.f;
    bool  mouseDown_ = false;

    // Keyboard/gamepad navigation
    Gamepad gamepad_;
    int     selected_   = 0;
    bool    navUp_      = false;
    bool    navDown_    = false;
    bool    navConfirm_ = false;

    int millisecondsPreviousFrame_ = 0;
};
