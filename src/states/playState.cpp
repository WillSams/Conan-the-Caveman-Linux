#include "playState.h"

#include <algorithm>
#include <fstream>

#include "gameOverState.h"
#include "pauseState.h"

// Spawns from the original map1.tmx object layer.
static const float PLAYER_SPAWN_X = 496.f;
static const float PLAYER_SPAWN_Y = 199.f;
static const float SNAIL_SPAWNS[][2] = {
    {718.f, 335.f},  {1753.f, 234.f}, {2203.f, 251.f},
    {3099.f, 351.f}, {4116.f, 372.f}, {4767.f, 306.f},
};

static constexpr float SNAIL_SPEED = 40.f;
static constexpr float DEATH_ANIM_SECONDS = 1.2f;

PlayState::PlayState(SDL_Renderer *renderer, int windowWidth, int windowHeight,
                     bool isDebugging, AssetStore *assetStore, bool &isRunning,
                     GameStateMachine &machine)
    : renderer_{renderer}, windowWidth_{windowWidth}, windowHeight_{windowHeight},
      isDebugging_{isDebugging}, assetStore_{assetStore},
      isRunning_{isRunning}, machine_{machine}
{
    LoadLevel();

    player_.box = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y, 30.f, 56.f};

    for (const auto &s : SNAIL_SPAWNS) {
        Snail snail;
        snail.body.box = {s[0], s[1], 30.f, 30.f};
        snails_.push_back(snail);
    }

    music_    = Mix_LoadMUS("./data/sfx/DST-Away.mp3");
    sfxJump_  = Mix_LoadWAV("./data/sfx/jump.wav");
    sfxDeath_ = Mix_LoadWAV("./data/sfx/death.wav");
    if (!music_) logger_.Err("PlayState: failed to load music — " + std::string(Mix_GetError()));

    millisecondsPreviousFrame_ = SDL_GetTicks();
}

PlayState::~PlayState() { onExit(); }

bool PlayState::onEnter() {
    if (music_) Mix_PlayMusic(music_, -1);
    m_loadingComplete = true;
    return true;
}

bool PlayState::onExit() {
    Mix_HaltMusic();
    if (music_)    { Mix_FreeMusic(music_);   music_    = nullptr; }
    if (sfxJump_)  { Mix_FreeChunk(sfxJump_); sfxJump_  = nullptr; }
    if (sfxDeath_) { Mix_FreeChunk(sfxDeath_); sfxDeath_ = nullptr; }
    m_exiting = true;
    return true;
}

void PlayState::resume() {
    // Back from the pause overlay — don't let the paused wall-clock time
    // register as one giant dt.
    millisecondsPreviousFrame_ = SDL_GetTicks();
    if (music_) Mix_ResumeMusic();
}

void PlayState::LoadLevel() {
    assetStore_->AddTexture(renderer_, "blocks1", "./data/gfx/blocks1.png");
    assetStore_->AddTexture(renderer_, "blocks2", "./data/gfx/blocks2.png");
    assetStore_->AddTexture(renderer_, "player",  "./data/gfx/player.png");
    assetStore_->AddTexture(renderer_, "snail",   "./data/gfx/snail.png");
    assetStore_->AddTexture(renderer_, "background", "./data/gfx/prehistoric.png");

    TileMapLoader loader("./data/conan.map", "", 32);
    tiles_ = loader.getMap();
    std::stable_sort(tiles_.begin(), tiles_.end(),
                     [](const Tile &a, const Tile &b) { return a.zIndex < b.zIndex; });

    std::ifstream collFile("./data/conan_colliders.map");
    if (collFile.is_open()) {
        solids_ = ParseColliderMap(collFile);
    } else {
        logger_.Err("PlayState: cannot open conan_colliders.map");
    }
    logger_.Log("Level loaded: " + std::to_string(tiles_.size()) + " tiles, " +
                std::to_string(solids_.size()) + " colliders");
}

void PlayState::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { isRunning_ = false; return; }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: case SDLK_p: wantPause_ = true; break;
            default: break;
            }
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);
    keyLeft_  = keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A];
    keyRight_ = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D];
    keyJump_  = keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_UP] ||
                keys[SDL_SCANCODE_W];

    // Merge in the gamepad: d-pad/stick moves, A jumps, Start pauses.
    GamepadState gp = gamepad_.poll();
    keyLeft_   = keyLeft_  || gp.moveDir < 0;
    keyRight_  = keyRight_ || gp.moveDir > 0;
    keyJump_   = keyJump_  || gp.jumpHeld;
    wantPause_ = wantPause_ || gp.pausePressed;
}

void PlayState::update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame_);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
        SDL_Delay(timeToWait);

    float dt = (SDL_GetTicks() - millisecondsPreviousFrame_) / 1000.f;
    dt = std::min(dt, 0.05f);
    millisecondsPreviousFrame_ = SDL_GetTicks();

    if (wantPause_ && !dying_) {
        wantPause_ = false;
        Mix_PauseMusic();
        machine_.pushState(new PauseState(
            renderer_, windowWidth_, windowHeight_, isDebugging_,
            assetStore_, isRunning_, machine_));
        return;
    }
    wantPause_ = false;

    UpdatePlayer(dt);
    if (!dying_) UpdateSnails(dt);

    // Camera follows the caveman, clamped to the level.
    cameraX_ = player_.box.x + player_.box.w / 2.f - windowWidth_ / 2.f;
    cameraX_ = std::max(0.f, std::min(cameraX_, levelW_ - windowWidth_));
}

void PlayState::UpdatePlayer(float dt) {
    if (dying_) {
        dyingTimer_ += dt;
        frame_ = static_cast<int>((dyingTimer_ / DEATH_ANIM_SECONDS) * 9);
        if (dyingTimer_ >= DEATH_ANIM_SECONDS) {
            machine_.changeState(new GameOverState(
                renderer_, windowWidth_, windowHeight_, isDebugging_,
                assetStore_, isRunning_, machine_));
        }
        return;
    }

    float moveDir = (keyRight_ ? 1.f : 0.f) - (keyLeft_ ? 1.f : 0.f);
    bool  wasGrounded = player_.onGround;

    StepBody(player_, moveDir, keyJump_, dt, params_, solids_);

    if (keyJump_ && wasGrounded && !player_.onGround && sfxJump_)
        Mix_PlayChannel(-1, sfxJump_, 0);

    if (moveDir < 0.f) facingLeft_ = true;
    if (moveDir > 0.f) facingLeft_ = false;

    // Keep the caveman inside the level horizontally.
    player_.box.x = std::max(0.f, std::min(player_.box.x, levelW_ - player_.box.w));

    // Fell into a pit.
    if (player_.box.y > windowHeight_) { Die(); return; }

    // Snail contact: stomp kills the snail, anything else kills Conan.
    for (auto &snail : snails_) {
        if (!snail.alive || !RectsOverlap(player_.box, snail.body.box)) continue;
        if (IsStomp(player_.box, player_.vy, snail.body.box)) {
            snail.alive = false;
            player_.vy  = -params_.jumpSpeed * 0.6f; // bounce off the shell
        } else {
            Die();
            return;
        }
    }

    // Pick the animation for this frame.
    PlayerAnim next = PlayerAnim::Idle;
    if (!player_.onGround)      next = PlayerAnim::Jump;
    else if (moveDir != 0.f)    next = PlayerAnim::Run;
    if (next != anim_) { anim_ = next; frame_ = 0; frameTimer_ = 0.f; }

    frameTimer_ += dt;
    if (frameTimer_ >= 0.12f) {
        frameTimer_ -= 0.12f;
        frame_ = (frame_ + 1) % PlayerFrameCount(anim_);
    }
}

void PlayState::Die() {
    dying_      = true;
    dyingTimer_ = 0.f;
    anim_       = PlayerAnim::Die;
    frame_      = 0;
    Mix_HaltMusic();
    if (sfxDeath_) Mix_PlayChannel(-1, sfxDeath_, 0);
}

void PlayState::UpdateSnails(float dt) {
    for (auto &snail : snails_) {
        if (!snail.alive) continue;

        // Turn at walls and platform edges.
        if (WallAhead(snail.body.box, snail.dir, solids_) ||
            (snail.body.onGround && !GroundAhead(snail.body.box, snail.dir, solids_)))
            snail.dir = -snail.dir;

        PlatformerParams snailParams = params_;
        snailParams.moveSpeed = SNAIL_SPEED;
        StepBody(snail.body, snail.dir, false, dt, snailParams, solids_);

        snail.frameTimer += dt;
        if (snail.frameTimer >= 0.2f) {
            snail.frameTimer -= 0.2f;
            snail.frame = (snail.frame + 1) % 4;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// render
// ─────────────────────────────────────────────────────────────────────────────

void PlayState::render() {
    SDL_SetRenderDrawColor(renderer_, 120, 180, 220, 255); // sky
    SDL_RenderClear(renderer_);

    RenderBackground();
    RenderTiles();
    RenderSnails();
    RenderPlayer();

    if (isDebugging_) {
        SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 200);
        for (const auto &s : solids_) {
            SDL_Rect r = {static_cast<int>(s.x - cameraX_), static_cast<int>(s.y),
                          static_cast<int>(s.w), static_cast<int>(s.h)};
            SDL_RenderDrawRect(renderer_, &r);
        }
    }

    SDL_RenderPresent(renderer_);
}

void PlayState::RenderBackground() {
    SDL_Texture *tex = assetStore_->GetTexture("background");
    if (!tex) return;

    // Slow parallax, tiled to cover the window at any scroll position.
    int offset = static_cast<int>(cameraX_ * 0.3f) % windowWidth_;
    SDL_Rect a = {-offset, 0, windowWidth_, windowHeight_};
    SDL_Rect b = {windowWidth_ - offset, 0, windowWidth_, windowHeight_};
    SDL_RenderCopy(renderer_, tex, nullptr, &a);
    SDL_RenderCopy(renderer_, tex, nullptr, &b);
}

void PlayState::RenderTiles() {
    for (const auto &tile : tiles_) {
        float wx = tile.relativePosition.x * 32.f;
        if (wx + 32.f < cameraX_ || wx > cameraX_ + windowWidth_) continue;

        SDL_Texture *tex = assetStore_->GetTexture(tile.assetId);
        if (!tex) continue;

        SDL_Rect src = {tile.pixelSrcPosition.x, tile.pixelSrcPosition.y, 32, 32};
        SDL_Rect dst = {static_cast<int>(wx - cameraX_),
                        tile.relativePosition.y * 32, 32, 32};
        SDL_RenderCopy(renderer_, tex, &src, &dst);
    }
}

void PlayState::RenderPlayer() {
    SDL_Texture *tex = assetStore_->GetTexture("player");
    if (!tex) return;

    FrameRect fr = PlayerFrame(anim_, frame_);
    // Anchor the sprite's feet to the collision box's feet, centered on x.
    SDL_Rect src = {fr.x, fr.y, fr.w, fr.h};
    SDL_Rect dst = {
        static_cast<int>(player_.box.x + player_.box.w / 2.f - fr.w / 2.f - cameraX_),
        static_cast<int>(player_.box.y + player_.box.h - fr.h),
        fr.w, fr.h};
    SDL_RenderCopyEx(renderer_, tex, &src, &dst, 0.0, nullptr,
                     facingLeft_ ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void PlayState::RenderSnails() {
    SDL_Texture *tex = assetStore_->GetTexture("snail");
    if (!tex) return;

    for (const auto &snail : snails_) {
        if (!snail.alive) continue;
        float wx = snail.body.box.x;
        if (wx + 34.f < cameraX_ || wx > cameraX_ + windowWidth_) continue;

        SDL_Rect src = {snail.frame * 34, 0, 34, 34};
        SDL_Rect dst = {static_cast<int>(wx - cameraX_ - 2.f),
                        static_cast<int>(snail.body.box.y - 4.f), 34, 34};
        SDL_RenderCopyEx(renderer_, tex, &src, &dst, 0.0, nullptr,
                         snail.dir > 0.f ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
}
