#include "pauseState.h"

#include <stormengine2/xmlLoader.h>

#include "menuState.h"

PauseState::PauseState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
                       bool isDebugging, AssetStore *assetStore, bool &isRunning,
                       GameStateMachine &machine)
    : renderer_{renderer}, windowWidth_{windowWidth}, windowHeight_{windowHeight},
      isDebugging_{isDebugging}, assetStore_{assetStore},
      isRunning_{isRunning}, machine_{machine}
{
    LoadTexturesFromXml("./data/conan.xml", "PAUSE", "./data/gfx/",
                        renderer_, assetStore_, &logger_);

    XmlLoader xml("./data/conan.xml");
    buttons_ = ButtonsFromXml(xml.GetObjects("PAUSE"));

    millisecondsPreviousFrame_ = SDL_GetTicks();
}

PauseState::~PauseState() { onExit(); }

bool PauseState::onEnter() {
    m_loadingComplete = true;
    return true;
}

bool PauseState::onExit() {
    m_exiting = true;
    return true;
}

void PauseState::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { isRunning_ = false; return; }
        if (event.type == SDL_KEYDOWN &&
            (event.key.keysym.sym == SDLK_ESCAPE ||
             event.key.keysym.sym == SDLK_p)) {
            wantResume_ = true;
        }
    }

    int mx, my;
    Uint32 buttons = SDL_GetMouseState(&mx, &my);
    mouseX_    = static_cast<float>(mx);
    mouseY_    = static_cast<float>(my);
    mouseDown_ = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
}

void PauseState::update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame_);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
        SDL_Delay(timeToWait);
    millisecondsPreviousFrame_ = SDL_GetTicks();

    if (wantResume_) {
        machine_.popState(); // PlayState::resume() restarts its clock
        return;
    }

    for (auto &b : buttons_) {
        if (!UpdateButton(b, mouseX_, mouseY_, mouseDown_)) continue;

        if (b.callbackId == 1) {        // Main menu — abandon the run
            // Pop this overlay first so the PlayState underneath is the one
            // changeState replaces — otherwise it stays buried on the stack.
            machine_.popState();
            machine_.changeState(new MenuState(
                renderer_, windowWidth_, windowHeight_, isDebugging_,
                assetStore_, isRunning_, machine_));
            return;
        }
        if (b.callbackId == 2) {        // Resume
            machine_.popState();
            return;
        }
    }
}

void PauseState::render() {
    // Opaque cave-dark backdrop — the machine renders only the top state.
    SDL_SetRenderDrawColor(renderer_, 18, 24, 32, 255);
    SDL_RenderClear(renderer_);

    for (const auto &b : buttons_)
        DrawButton(renderer_, assetStore_, b, mouseX_, mouseY_, mouseDown_);

    SDL_RenderPresent(renderer_);
}
