#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stormengine2/assetStore.h>
#include <stormengine2/gameStateMachine.h>
#include <stormengine2/logger.h>

// Engine-backed shell: the Game owns the window/renderer, the AssetStore
// (states borrow a raw pointer), and the GameStateMachine. Each screen —
// menu, play, pause, game over — is a GameState; pause rides the state
// stack via pushState/popState.
class Game {
public:
    Game();
    ~Game();

    void Initialize();
    void Run();
    void Destroy();

private:
    void ProcessInput();
    void Update();
    void Render();

    bool isRunning   = false;
    bool isDebugging = false;

    SDL_Window   *window   = nullptr;
    SDL_Renderer *renderer = nullptr;

    GameStateMachine gameStateMachine;
    Logger_Ptr       logger;
    AssetStore_Ptr   assetStore;

    int windowWidth  = 640;
    int windowHeight = 480;
};
