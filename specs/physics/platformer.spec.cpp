#include <igloo/igloo_alt.h>

#include "../../src/physics/platformer.h"

using namespace igloo;

// A single floor tile row at y=400 and a wall at x=200 cover the core cases.
static std::vector<RectF> Floor() { return {{0.f, 400.f, 640.f, 32.f}}; }

Describe(PlatformerSpec) {

    Describe(Gravity) {
        It(should_pull_a_falling_body_downward) {
            Body b; b.box = {100.f, 100.f, 30.f, 56.f};
            StepBody(b, 0.f, false, 0.1f, {}, {});
            Assert::That(b.box.y > 100.f, Equals(true));
            Assert::That(b.vy > 0.f, Equals(true));
        };
        It(should_cap_fall_speed_at_terminal_velocity) {
            Body b; b.box = {100.f, 100.f, 30.f, 56.f};
            PlatformerParams p;
            for (int i = 0; i < 100; ++i) StepBody(b, 0.f, false, 0.05f, p, {});
            Assert::That(b.vy, EqualsWithDelta(p.maxFall, 1e-3));
        };
    };

    Describe(Landing) {
        It(should_land_on_a_floor_tile_and_stand_on_it) {
            Body b; b.box = {100.f, 340.f, 30.f, 56.f}; // 4px above the floor
            for (int i = 0; i < 10; ++i) StepBody(b, 0.f, false, 0.05f, {}, Floor());
            Assert::That(b.box.y, EqualsWithDelta(400.f - 56.f, 1e-3)); // feet on tile
            Assert::That(b.onGround, Equals(true));
            Assert::That(b.vy, Equals(0.f));
        };
    };

    Describe(Jumping) {
        It(should_jump_only_from_the_ground) {
            Body b; b.box = {100.f, 344.f, 30.f, 56.f};
            b.onGround = true;
            StepBody(b, 0.f, true, 0.016f, {}, Floor());
            Assert::That(b.vy < 0.f, Equals(true));  // moving up
            Assert::That(b.onGround, Equals(false));
        };
        It(should_ignore_jump_while_airborne) {
            Body b; b.box = {100.f, 100.f, 30.f, 56.f}; // mid-air
            StepBody(b, 0.f, true, 0.016f, {}, {});
            Assert::That(b.vy > 0.f, Equals(true)); // still falling — no double jump
        };
        It(should_reach_the_configured_jump_height) {
            // v = sqrt(2*g*h) means the 80px jump of the original game.
            Assert::That(JumpSpeedForHeight(1500.f, 80.f),
                         EqualsWithDelta(489.9f, 0.1f));
        };
    };

    Describe(Walls) {
        It(should_stop_at_a_wall_when_moving_right) {
            std::vector<RectF> solids = Floor();
            solids.push_back({200.f, 300.f, 32.f, 100.f}); // wall ahead
            Body b; b.box = {160.f, 344.f, 30.f, 56.f};
            b.onGround = true;
            for (int i = 0; i < 20; ++i) StepBody(b, 1.f, false, 0.05f, {}, solids);
            Assert::That(b.box.x, EqualsWithDelta(200.f - 30.f, 1e-3)); // flush
        };
    };

    Describe(EdgeProbes) {
        It(should_see_ground_ahead_on_a_platform) {
            RectF box{100.f, 344.f, 30.f, 56.f};
            Assert::That(GroundAhead(box, 1.f, Floor()), Equals(true));
        };
        It(should_see_the_edge_at_the_end_of_a_platform) {
            std::vector<RectF> shortFloor = {{0.f, 400.f, 128.f, 32.f}};
            RectF box{100.f, 344.f, 30.f, 56.f}; // right edge at 130, floor ends 128
            Assert::That(GroundAhead(box, 1.f, shortFloor), Equals(false));
        };
        It(should_detect_a_wall_ahead) {
            std::vector<RectF> solids = {{200.f, 300.f, 32.f, 132.f}};
            RectF box{170.f, 344.f, 30.f, 56.f};
            Assert::That(WallAhead(box, 1.f, solids), Equals(true));
            Assert::That(WallAhead(box, -1.f, solids), Equals(false));
        };
    };

    Describe(Stomping) {
        It(should_count_a_falling_hit_from_above_as_a_stomp) {
            RectF attacker{100.f, 300.f, 30.f, 56.f}; // feet at 356
            RectF victim{100.f, 366.f, 34.f, 34.f};   // midline at 383
            Assert::That(IsStomp(attacker, 200.f, victim), Equals(true));
        };
        It(should_not_count_a_side_on_hit_as_a_stomp) {
            RectF attacker{80.f, 360.f, 30.f, 56.f};  // feet at 416, below midline
            RectF victim{100.f, 366.f, 34.f, 34.f};
            Assert::That(IsStomp(attacker, 200.f, victim), Equals(false));
        };
        It(should_not_count_a_rising_hit_as_a_stomp) {
            RectF attacker{100.f, 300.f, 30.f, 56.f};
            RectF victim{100.f, 366.f, 34.f, 34.f};
            Assert::That(IsStomp(attacker, -200.f, victim), Equals(false));
        };
    };
};
