#include "game.h"

#include "states/menuState.h"

Game::Game() {
    assetStore = std::make_unique<AssetStore>();
    logger     = std::make_unique<Logger>();
    logger->Log("Conan the Caveman constructor called");
}

Game::~Game() { logger->Log("Conan the Caveman destructor called"); }

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        logger->Err("Error initializing SDL.");
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
        logger->Err("Error initializing SDL_mixer: " + std::string(Mix_GetError()));

    window = SDL_CreateWindow("Conan the Caveman",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight, 0);
    if (!window) { logger->Err("Error creating SDL window."); return; }

    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) { logger->Err("Error creating SDL renderer."); return; }

    gameStateMachine.changeState(
        new MenuState(renderer, windowWidth, windowHeight, isDebugging,
                      assetStore.get(), isRunning, gameStateMachine));

    isRunning = true;
}

void Game::Run() {
    Initialize();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() { gameStateMachine.processInput(); }
void Game::Update()       { gameStateMachine.update(); }
void Game::Render()       { gameStateMachine.render(); }

void Game::Destroy() {
    gameStateMachine.clean();
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
}
