#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>

#include <stormengine2/assetStore.h>
#include <stormengine2/gameStateMachine.h>
#include <stormengine2/logger.h>
#include <stormengine2/states/gameState.h>
#include <stormengine2/tilemapLoader.h>

#include "../input/gamepad.h"
#include "../level/colliderMap.h"
#include "../physics/platformer.h"
#include "../sprites/playerFrames.h"

// The level itself: tiles from data/conan.map (converted from the original
// TMX by tools/tmx2map.py), solid rects from data/conan_colliders.map, the
// caveman, and patrolling snails. Esc pushes PauseState onto the state stack.
class PlayState : public GameState {
public:
    PlayState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
              bool isDebugging, AssetStore *assetStore, bool &isRunning,
              GameStateMachine &machine);
    ~PlayState();

    void processInput() override;
    void update()       override;
    void render()       override;
    bool onEnter()      override;
    bool onExit()       override;
    void resume()       override;
    std::string getStateID() const override { return "PLAY"; }

private:
    struct Snail {
        Body  body;
        float dir   = -1.f;
        int   frame = 0;
        float frameTimer = 0.f;
        bool  alive = true;
    };

    void LoadLevel();
    void UpdatePlayer(float dt);
    void UpdateSnails(float dt);
    void Die();
    void RenderBackground();
    void RenderTiles();
    void RenderPlayer();
    void RenderSnails();

    SDL_Renderer     *renderer_;
    int               windowWidth_, windowHeight_;
    bool              isDebugging_;
    AssetStore       *assetStore_;
    Logger            logger_;
    bool             &isRunning_;
    GameStateMachine &machine_;

    // Level
    Map                tiles_;    // sorted by zIndex at load
    std::vector<RectF> solids_;
    float              levelW_ = 6400.f;

    // Player
    Body       player_;
    PlatformerParams params_;
    PlayerAnim anim_        = PlayerAnim::Idle;
    int        frame_       = 0;
    float      frameTimer_  = 0.f;
    bool       facingLeft_  = false;
    bool       dying_       = false;
    float      dyingTimer_  = 0.f;

    std::vector<Snail> snails_;

    float cameraX_ = 0.f;

    // Input (keyboard + gamepad merged each frame)
    Gamepad gamepad_;
    bool keyLeft_ = false, keyRight_ = false, keyJump_ = false, wantPause_ = false;

    // Audio (freed in onExit)
    Mix_Music *music_    = nullptr;
    Mix_Chunk *sfxJump_  = nullptr;
    Mix_Chunk *sfxDeath_ = nullptr;

    int millisecondsPreviousFrame_ = 0;
};
