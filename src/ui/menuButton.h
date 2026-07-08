#pragma once

// ── Pure 3-state button logic (SDL-free) ────────────────────────────────────
// The button sheets (button.png, resume.png, ...) hold three frames:
// mouse-out, mouse-over, pressed. A click fires on release inside the button.

inline bool PointInBox(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

// Which sheet frame to draw: 0 out, 1 over, 2 held down.
inline int ButtonFrame(bool over, bool down) {
    if (!over) return 0;
    return down ? 2 : 1;
}

// Click = the mouse was down over the button last frame and released inside
// it this frame.
inline bool ButtonClicked(bool over, bool wasDown, bool isDown) {
    return over && wasDown && !isDown;
}
