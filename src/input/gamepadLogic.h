#pragma once

// ── Pure gamepad/menu-navigation logic (SDL-free) ───────────────────────────

// Maps a raw analog axis value (-32768..32767) to -1/0/+1 with a deadzone, so
// a resting stick can't drift the caveman.
inline int AxisDir(int value, int deadzone = 8000) {
    if (value > deadzone)  return 1;
    if (value < -deadzone) return -1;
    return 0;
}

// Moves a menu selection by `delta`, wrapping at both ends.
inline int WrapSelection(int index, int delta, int count) {
    if (count <= 0) return 0;
    return ((index + delta) % count + count) % count;
}
