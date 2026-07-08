#pragma once

#include <cmath>
#include <vector>

// ── Pure platformer physics (SDL-free) ──────────────────────────────────────
// Axis-separated AABB movement against a list of solid rects: X moves first
// and stops at walls, then Y applies gravity and lands on (or bonks) tiles.
// PlayState feeds input and renders; everything here is spec-able.

struct RectF {
    float x = 0.f, y = 0.f, w = 0.f, h = 0.f;
};

inline bool RectsOverlap(const RectF &a, const RectF &b) {
    return a.x < b.x + b.w && a.x + a.w > b.x &&
           a.y < b.y + b.h && a.y + a.h > b.y;
}

struct Body {
    RectF box;                 // collision box in world px
    float vx = 0.f, vy = 0.f;  // px/s
    bool  onGround = false;
};

struct PlatformerParams {
    float gravity   = 1500.f; // px/s^2
    float moveSpeed = 180.f;  // horizontal px/s (old game: 3 px/frame @ 60fps)
    float jumpSpeed = 490.f;  // sqrt(2*gravity*jumpHeight) for an 80px jump
    float maxFall   = 900.f;  // terminal velocity
};

// Initial upward speed that peaks at `height` px under `gravity`.
inline float JumpSpeedForHeight(float gravity, float height) {
    return std::sqrt(2.f * gravity * height);
}

// One physics step. `moveDir` is -1/0/+1; `wantJump` only takes effect on the
// ground. Resolves against `solids` one axis at a time.
inline void StepBody(Body &b, float moveDir, bool wantJump, float dt,
                     const PlatformerParams &p, const std::vector<RectF> &solids) {
    // Horizontal
    b.vx = moveDir * p.moveSpeed;
    b.box.x += b.vx * dt;
    for (const RectF &s : solids) {
        if (!RectsOverlap(b.box, s)) continue;
        if (b.vx > 0.f)      b.box.x = s.x - b.box.w; // hit a wall moving right
        else if (b.vx < 0.f) b.box.x = s.x + s.w;     // hit a wall moving left
        b.vx = 0.f;
    }

    // Jump only from the ground
    if (wantJump && b.onGround) {
        b.vy       = -p.jumpSpeed;
        b.onGround = false;
    }

    // Vertical
    b.vy = std::fmin(b.vy + p.gravity * dt, p.maxFall);
    b.box.y += b.vy * dt;
    b.onGround = false;
    for (const RectF &s : solids) {
        if (!RectsOverlap(b.box, s)) continue;
        if (b.vy > 0.f) {                             // landed on a tile
            b.box.y    = s.y - b.box.h;
            b.onGround = true;
        } else if (b.vy < 0.f) {                      // bonked a ceiling
            b.box.y = s.y + s.h;
        }
        b.vy = 0.f;
    }
}

// True when standing at `box` there is ground just ahead in `dir` — used by
// patrolling enemies to turn around at platform edges.
inline bool GroundAhead(const RectF &box, float dir,
                        const std::vector<RectF> &solids, float probe = 4.f) {
    RectF foot{dir > 0.f ? box.x + box.w : box.x - probe,
               box.y + box.h + 1.f, probe, probe};
    for (const RectF &s : solids)
        if (RectsOverlap(foot, s)) return true;
    return false;
}

// True when a wall blocks movement in `dir` from `box`.
inline bool WallAhead(const RectF &box, float dir,
                      const std::vector<RectF> &solids, float probe = 2.f) {
    RectF side{dir > 0.f ? box.x + box.w : box.x - probe,
               box.y + 2.f, probe, box.h - 4.f};
    for (const RectF &s : solids)
        if (RectsOverlap(side, s)) return true;
    return false;
}

// Stomp test: the attacker's feet are above the victim's midline and falling.
inline bool IsStomp(const RectF &attacker, float attackerVy, const RectF &victim) {
    return attackerVy > 0.f &&
           attacker.y + attacker.h < victim.y + victim.h * 0.5f;
}
