#include <igloo/igloo_alt.h>

#include "../../src/ui/menuButton.h"

using namespace igloo;

Describe(MenuButtonSpec) {

    Describe(HitTesting) {
        It(should_contain_a_point_inside_the_box) {
            Assert::That(PointInBox(150.f, 150.f, 100.f, 100.f, 400.f, 100.f),
                         Equals(true));
        };
        It(should_not_contain_a_point_outside_the_box) {
            Assert::That(PointInBox(50.f, 150.f, 100.f, 100.f, 400.f, 100.f),
                         Equals(false));
        };
    };

    Describe(Frames) {
        It(should_show_the_out_frame_when_not_hovered) {
            Assert::That(ButtonFrame(false, false), Equals(0));
        };
        It(should_show_the_over_frame_when_hovered) {
            Assert::That(ButtonFrame(true, false), Equals(1));
        };
        It(should_show_the_down_frame_while_pressed) {
            Assert::That(ButtonFrame(true, true), Equals(2));
        };
    };

    Describe(Clicking) {
        It(should_fire_on_release_inside_the_button) {
            Assert::That(ButtonClicked(true, true, false), Equals(true));
        };
        It(should_not_fire_while_still_held) {
            Assert::That(ButtonClicked(true, true, true), Equals(false));
        };
        It(should_not_fire_on_release_outside_the_button) {
            Assert::That(ButtonClicked(false, true, false), Equals(false));
        };
    };
};
