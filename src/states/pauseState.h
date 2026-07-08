#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include <stormengine2/assetStore.h>
#include <stormengine2/gameStateMachine.h>
#include <stormengine2/logger.h>
#include <stormengine2/states/gameState.h>

#include "../ui/uiButton.h"

// Pause overlay, pushed onto the state stack over PlayState. Resume pops back
// (PlayState::resume() picks the clock back up); Main abandons the run.
class PauseState : public GameState {
public:
    PauseState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
               bool isDebugging, AssetStore *assetStore, bool &isRunning,
               GameStateMachine &machine);
    ~PauseState();

    void processInput() override;
    void update()       override;
    void render()       override;
    bool onEnter()      override;
    bool onExit()       override;
    std::string getStateID() const override { return "PAUSE"; }

private:
    SDL_Renderer     *renderer_;
    int               windowWidth_, windowHeight_;
    bool              isDebugging_;
    AssetStore       *assetStore_;
    Logger            logger_;
    bool             &isRunning_;
    GameStateMachine &machine_;

    std::vector<UiButton> buttons_;

    float mouseX_ = 0.f, mouseY_ = 0.f;
    bool  mouseDown_ = false;
    bool  wantResume_ = false;

    int millisecondsPreviousFrame_ = 0;
};
