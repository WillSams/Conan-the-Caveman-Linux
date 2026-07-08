#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include <stormengine2/assetStore.h>
#include <stormengine2/xmlLoader.h>

#include "menuButton.h"

// A screen button placed from a conan.xml <object> entry. The texture sheet
// holds three frames side by side (out / over / down).
struct UiButton {
    float       x = 0, y = 0, w = 0, h = 0;
    std::string textureId;
    int         callbackId = 0;
    bool        wasDown    = false;
};

inline std::vector<UiButton> ButtonsFromXml(const std::vector<XmlObjectDef> &objects) {
    std::vector<UiButton> buttons;
    for (const auto &o : objects) {
        if (o.type != "MenuButton") continue;
        UiButton b;
        b.x = o.x;
        b.y = o.y;
        b.w = static_cast<float>(o.width);
        b.h = static_cast<float>(o.height);
        b.textureId = o.textureId;
        auto it = o.attributes.find("callbackID");
        if (it != o.attributes.end()) b.callbackId = std::stoi(it->second);
        buttons.push_back(b);
    }
    return buttons;
}

// Updates one button from the current mouse state; returns true on click.
inline bool UpdateButton(UiButton &b, float mx, float my, bool mouseDown) {
    bool over    = PointInBox(mx, my, b.x, b.y, b.w, b.h);
    bool clicked = ButtonClicked(over, b.wasDown, mouseDown);
    b.wasDown    = over && mouseDown;
    return clicked;
}

// Draws the frame that matches the current mouse state. `selected` (keyboard
// or gamepad navigation) shows the hover frame without the mouse.
inline void DrawButton(SDL_Renderer *renderer, AssetStore *assetStore,
                       const UiButton &b, float mx, float my, bool mouseDown,
                       bool selected = false) {
    SDL_Texture *tex = assetStore->GetTexture(b.textureId);
    if (!tex) return;

    int texW, texH;
    SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);
    int frameW = texW / 3;

    bool over  = PointInBox(mx, my, b.x, b.y, b.w, b.h) || selected;
    int  frame = ButtonFrame(over, over && mouseDown);

    SDL_Rect src = {frame * frameW, 0, frameW, texH};
    SDL_Rect dst = {static_cast<int>(b.x), static_cast<int>(b.y),
                    static_cast<int>(b.w), static_cast<int>(b.h)};
    SDL_RenderCopy(renderer, tex, &src, &dst);
}
