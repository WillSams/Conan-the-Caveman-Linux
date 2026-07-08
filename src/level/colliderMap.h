#pragma once

#include <istream>
#include <string>
#include <vector>

#include "../physics/platformer.h"

// Parses the collider map produced from the old TMX Collision layer:
//   collider x y scaleX scaleY w h offX offY
// (Same shape as the JRPG example's collider file; scale/offset are unused
// here but kept so the formats stay interchangeable.)
inline std::vector<RectF> ParseColliderMap(std::istream &in) {
    std::vector<RectF> rects;
    std::string token;
    while (in >> token) {
        if (token != "collider") continue;
        float x, y, sx, sy, w, h, ox, oy;
        if (!(in >> x >> y >> sx >> sy >> w >> h >> ox >> oy)) break;
        rects.push_back({x, y, w, h});
    }
    return rects;
}
