#include <igloo/igloo_alt.h>

#include "../../src/input/gamepadLogic.h"

using namespace igloo;

Describe(GamepadLogicSpec) {

    Describe(AxisDeadzone) {
        It(should_ignore_a_resting_stick) {
            Assert::That(AxisDir(0), Equals(0));
            Assert::That(AxisDir(4000), Equals(0));
            Assert::That(AxisDir(-4000), Equals(0));
        };
        It(should_report_a_push_past_the_deadzone) {
            Assert::That(AxisDir(20000), Equals(1));
            Assert::That(AxisDir(-20000), Equals(-1));
        };
        It(should_treat_the_deadzone_edge_as_resting) {
            Assert::That(AxisDir(8000), Equals(0));
        };
    };

    Describe(MenuSelection) {
        It(should_advance_the_selection) {
            Assert::That(WrapSelection(0, 1, 3), Equals(1));
        };
        It(should_wrap_forward_past_the_end) {
            Assert::That(WrapSelection(2, 1, 3), Equals(0));
        };
        It(should_wrap_backward_past_the_start) {
            Assert::That(WrapSelection(0, -1, 3), Equals(2));
        };
        It(should_be_safe_with_no_items) {
            Assert::That(WrapSelection(0, 1, 0), Equals(0));
        };
    };
};
