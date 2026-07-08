#include "menuState.h"

#include <stormengine2/xmlLoader.h>

#include "playState.h"

MenuState::MenuState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
                     bool isDebugging, AssetStore *assetStore, bool &isRunning,
                     GameStateMachine &machine)
    : renderer_{renderer}, windowWidth_{windowWidth}, windowHeight_{windowHeight},
      isDebugging_{isDebugging}, assetStore_{assetStore},
      isRunning_{isRunning}, machine_{machine}
{
    LoadTexturesFromXml("./data/conan.xml", "MENU", "./data/gfx/",
                        renderer_, assetStore_, &logger_);

    XmlLoader xml("./data/conan.xml");
    buttons_ = ButtonsFromXml(xml.GetObjects("MENU"));

    millisecondsPreviousFrame_ = SDL_GetTicks();
}

MenuState::~MenuState() { onExit(); }

bool MenuState::onEnter() {
    m_loadingComplete = true;
    return true;
}

bool MenuState::onExit() {
    m_exiting = true;
    return true;
}

void MenuState::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { isRunning_ = false; return; }
        if (event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_ESCAPE) {
            isRunning_ = false;
            return;
        }
    }

    int mx, my;
    Uint32 buttons = SDL_GetMouseState(&mx, &my);
    mouseX_    = static_cast<float>(mx);
    mouseY_    = static_cast<float>(my);
    mouseDown_ = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
}

void MenuState::update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame_);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
        SDL_Delay(timeToWait);
    millisecondsPreviousFrame_ = SDL_GetTicks();

    for (auto &b : buttons_) {
        if (!UpdateButton(b, mouseX_, mouseY_, mouseDown_)) continue;

        if (b.callbackId == 1) {        // Play
            machine_.changeState(new PlayState(
                renderer_, windowWidth_, windowHeight_, isDebugging_,
                assetStore_, isRunning_, machine_));
            return;
        }
        if (b.callbackId == 2) {        // Exit
            isRunning_ = false;
            return;
        }
    }
}

void MenuState::render() {
    SDL_SetRenderDrawColor(renderer_, 30, 60, 90, 255); // cave-night blue
    SDL_RenderClear(renderer_);

    for (const auto &b : buttons_)
        DrawButton(renderer_, assetStore_, b, mouseX_, mouseY_, mouseDown_);

    SDL_RenderPresent(renderer_);
}
