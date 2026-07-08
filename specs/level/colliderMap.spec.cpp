#include <igloo/igloo_alt.h>
#include <sstream>

#include "../../src/level/colliderMap.h"

using namespace igloo;

Describe(ColliderMapSpec) {

    It(should_parse_a_collider_line_into_a_rect) {
        std::istringstream in("collider 256 96 1 1 128 32 0 0");
        auto rects = ParseColliderMap(in);
        Assert::That(rects.size(), Equals(1u));
        Assert::That(rects[0].x, Equals(256.f));
        Assert::That(rects[0].y, Equals(96.f));
        Assert::That(rects[0].w, Equals(128.f));
        Assert::That(rects[0].h, Equals(32.f));
    };

    It(should_parse_multiple_lines) {
        std::istringstream in("collider 0 0 1 1 256 32 0 0\n"
                              "collider 5952 0 1 1 448 32 0 0\n");
        Assert::That(ParseColliderMap(in).size(), Equals(2u));
    };

    It(should_skip_unknown_tokens) {
        std::istringstream in("junk collider 0 0 1 1 32 32 0 0");
        Assert::That(ParseColliderMap(in).size(), Equals(1u));
    };

    It(should_return_empty_for_an_empty_stream) {
        std::istringstream in("");
        Assert::That(ParseColliderMap(in).empty(), Equals(true));
    };
};
