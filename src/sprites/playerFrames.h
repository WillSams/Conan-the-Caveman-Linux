#pragma once

// ── Conan sprite sheet layout (player.png, 454×340, irregular rows) ─────────
// Frame rects measured from the sheet; the rows are not evenly pitched, so
// each animation carries explicit source rectangles.

struct FrameRect {
    int x, y, w, h;
};

enum class PlayerAnim { Idle, Run, Jump, Duck, Die };

inline int PlayerFrameCount(PlayerAnim a) {
    switch (a) {
    case PlayerAnim::Run:  return 4;
    case PlayerAnim::Jump: return 2;
    case PlayerAnim::Die:  return 9;
    default:               return 1; // Idle, Duck
    }
}

inline FrameRect PlayerFrame(PlayerAnim a, int frame) {
    switch (a) {
    case PlayerAnim::Idle: return {2, 2, 36, 58};
    case PlayerAnim::Run: {
        static const FrameRect run[4] = {
            {2, 62, 38, 58}, {44, 62, 36, 58}, {83, 62, 32, 58}, {122, 62, 38, 58}};
        return run[frame % 4];
    }
    case PlayerAnim::Jump: {
        static const FrameRect jump[2] = {{1, 127, 40, 53}, {45, 127, 34, 53}};
        return jump[frame % 2];
    }
    case PlayerAnim::Duck: return {0, 185, 37, 52};
    case PlayerAnim::Die: {
        // Bottom row: 9 disintegration frames on a ~50px pitch.
        int f = frame % 9;
        return {3 + f * 50, 245, 50, 56};
    }
    }
    return {2, 2, 36, 58};
}
