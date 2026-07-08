#include "gameOverState.h"

#include <stormengine2/xmlLoader.h>

#include "menuState.h"
#include "playState.h"

GameOverState::GameOverState(SDL_Renderer *renderer, int windowWidth,
                             int windowHeight, bool isDebugging,
                             AssetStore *assetStore, bool &isRunning,
                             GameStateMachine &machine)
    : renderer_{renderer}, windowWidth_{windowWidth}, windowHeight_{windowHeight},
      isDebugging_{isDebugging}, assetStore_{assetStore},
      isRunning_{isRunning}, machine_{machine}
{
    LoadTexturesFromXml("./data/conan.xml", "GAMEOVER", "./data/gfx/",
                        renderer_, assetStore_, &logger_);

    XmlLoader xml("./data/conan.xml");
    auto objects = xml.GetObjects("GAMEOVER");
    buttons_ = ButtonsFromXml(objects);

    for (const auto &o : objects) {
        if (o.type == "AnimatedGraphic") {
            titleDst_ = {static_cast<int>(o.x), static_cast<int>(o.y),
                         o.width, o.height};
        }
    }

    millisecondsPreviousFrame_ = SDL_GetTicks();
}

GameOverState::~GameOverState() { onExit(); }

bool GameOverState::onEnter() {
    m_loadingComplete = true;
    return true;
}

bool GameOverState::onExit() {
    m_exiting = true;
    return true;
}

void GameOverState::processInput() {
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

void GameOverState::update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame_);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
        SDL_Delay(timeToWait);

    float dt = (SDL_GetTicks() - millisecondsPreviousFrame_) / 1000.f;
    millisecondsPreviousFrame_ = SDL_GetTicks();

    titleTimer_ += dt;
    if (titleTimer_ >= 0.5f) {
        titleTimer_ -= 0.5f;
        titleFrame_ = (titleFrame_ + 1) % 2;
    }

    for (auto &b : buttons_) {
        if (!UpdateButton(b, mouseX_, mouseY_, mouseDown_)) continue;

        if (b.callbackId == 1) {        // Main menu
            machine_.changeState(new MenuState(
                renderer_, windowWidth_, windowHeight_, isDebugging_,
                assetStore_, isRunning_, machine_));
            return;
        }
        if (b.callbackId == 2) {        // Restart
            machine_.changeState(new PlayState(
                renderer_, windowWidth_, windowHeight_, isDebugging_,
                assetStore_, isRunning_, machine_));
            return;
        }
    }
}

void GameOverState::render() {
    SDL_SetRenderDrawColor(renderer_, 20, 20, 26, 255);
    SDL_RenderClear(renderer_);

    // Blinking "Game Over" (two frames side by side in the sheet).
    SDL_Texture *title = assetStore_->GetTexture("gameovertext");
    if (title) {
        int texW, texH;
        SDL_QueryTexture(title, nullptr, nullptr, &texW, &texH);
        SDL_Rect src = {titleFrame_ * (texW / 2), 0, texW / 2, texH};
        SDL_RenderCopy(renderer_, title, &src, &titleDst_);
    }

    for (const auto &b : buttons_)
        DrawButton(renderer_, assetStore_, b, mouseX_, mouseY_, mouseDown_);

    SDL_RenderPresent(renderer_);
}
