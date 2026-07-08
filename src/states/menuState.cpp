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
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: isRunning_ = false; return;
            case SDLK_UP:     case SDLK_w: navUp_      = true; break;
            case SDLK_DOWN:   case SDLK_s: navDown_    = true; break;
            case SDLK_RETURN: case SDLK_SPACE: navConfirm_ = true; break;
            default: break;
            }
        }
    }

    GamepadState gp = gamepad_.poll();
    navUp_      = navUp_      || gp.upPressed;
    navDown_    = navDown_    || gp.downPressed;
    navConfirm_ = navConfirm_ || gp.confirmPressed;

    int mx, my;
    Uint32 buttons = SDL_GetMouseState(&mx, &my);
    mouseX_    = static_cast<float>(mx);
    mouseY_    = static_cast<float>(my);
    mouseDown_ = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
}

void MenuState::Activate(int callbackId) {
    if (callbackId == 1) {              // Play
        machine_.changeState(new PlayState(
            renderer_, windowWidth_, windowHeight_, isDebugging_,
            assetStore_, isRunning_, machine_));
    } else if (callbackId == 2) {       // Exit
        isRunning_ = false;
    }
}

void MenuState::update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame_);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
        SDL_Delay(timeToWait);
    millisecondsPreviousFrame_ = SDL_GetTicks();

    if (navUp_)   selected_ = WrapSelection(selected_, -1, buttons_.size());
    if (navDown_) selected_ = WrapSelection(selected_,  1, buttons_.size());
    bool confirm = navConfirm_;
    navUp_ = navDown_ = navConfirm_ = false;

    if (confirm && !buttons_.empty()) {
        Activate(buttons_[selected_].callbackId);
        return;
    }

    for (auto &b : buttons_) {
        if (!UpdateButton(b, mouseX_, mouseY_, mouseDown_)) continue;
        Activate(b.callbackId);
        return;
    }
}

void MenuState::render() {
    SDL_SetRenderDrawColor(renderer_, 30, 60, 90, 255); // cave-night blue
    SDL_RenderClear(renderer_);

    for (std::size_t i = 0; i < buttons_.size(); ++i)
        DrawButton(renderer_, assetStore_, buttons_[i], mouseX_, mouseY_,
                   mouseDown_, static_cast<int>(i) == selected_);

    SDL_RenderPresent(renderer_);
}
