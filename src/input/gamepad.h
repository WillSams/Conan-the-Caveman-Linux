#pragma once

#include <SDL2/SDL.h>

#include "gamepadLogic.h"

// Snapshot of the pad each frame. Held values drive movement; *Pressed values
// are edge-triggered (true only on the frame the button goes down).
struct GamepadState {
    int  moveDir      = 0;     // -1 / 0 / +1 from d-pad or left stick
    bool jumpHeld     = false; // A
    bool pausePressed = false; // Start
    bool confirmPressed = false; // A, edge — menu activation
    bool upPressed      = false; // d-pad/stick up, edge — menu navigation
    bool downPressed    = false; // d-pad/stick down, edge
};

// Thin SDL_GameController wrapper. Each state owns one (SDL refcounts the
// underlying device, so multiple instances are fine); it lazily opens the
// first attached controller and survives hot-plugging.
class Gamepad {
public:
    ~Gamepad() {
        if (pad_) SDL_GameControllerClose(pad_);
    }

    GamepadState poll() {
        GamepadState out;

        if (pad_ && !SDL_GameControllerGetAttached(pad_)) {
            SDL_GameControllerClose(pad_);
            pad_   = nullptr;
            first_ = true; // re-seed the edge history on reconnect
        }
        if (!pad_) TryOpen();
        if (!pad_) { prevA_ = prevStart_ = prevUp_ = prevDown_ = false; return out; }

        bool dpadLeft  = SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        bool dpadRight = SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        bool dpadUp    = SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_UP);
        bool dpadDown  = SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_DPAD_DOWN);

        int stickX = AxisDir(SDL_GameControllerGetAxis(pad_, SDL_CONTROLLER_AXIS_LEFTX));
        int stickY = AxisDir(SDL_GameControllerGetAxis(pad_, SDL_CONTROLLER_AXIS_LEFTY));

        out.moveDir = dpadLeft ? -1 : dpadRight ? 1 : stickX;

        bool a     = SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_A);
        bool start = SDL_GameControllerGetButton(pad_, SDL_CONTROLLER_BUTTON_START);
        bool up    = dpadUp   || stickY < 0;
        bool down  = dpadDown || stickY > 0;

        // On the very first poll, seed the edge history from the current
        // buttons WITHOUT reporting edges. Each state owns its own Gamepad, so
        // the button that triggered the state change (e.g. Start opening the
        // pause menu) is still held when the new state polls — a fresh history
        // would re-fire the edge and instantly bounce back.
        if (first_) {
            first_ = false;
            prevA_ = a; prevStart_ = start; prevUp_ = up; prevDown_ = down;
            out.jumpHeld = a;
            return out;
        }

        out.jumpHeld       = a;
        out.confirmPressed = a && !prevA_;
        out.pausePressed   = start && !prevStart_;
        out.upPressed      = up && !prevUp_;
        out.downPressed    = down && !prevDown_;

        prevA_ = a; prevStart_ = start; prevUp_ = up; prevDown_ = down;
        return out;
    }

private:
    void TryOpen() {
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (!SDL_IsGameController(i)) continue;
            pad_ = SDL_GameControllerOpen(i);
            if (pad_) return;
        }
    }

    SDL_GameController *pad_ = nullptr;
    bool first_ = true; // first poll seeds edge history without firing edges
    bool prevA_ = false, prevStart_ = false, prevUp_ = false, prevDown_ = false;
};
