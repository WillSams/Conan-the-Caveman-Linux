# Changelog

## [2026-07-08]

### Changed

- **Rebuilt the game on Storm Engine v2** (linked as `libstormenginev2`), replacing the book-derived singleton architecture (`TheGame`, `TheTextureManager`, `TheInputHandler`, TMX `LevelParser` with base64/zlib) with the engine's `Game` + `GameStateMachine` + `AssetStore`
- Level converted offline from Tiled TMX to the engine's editor `.map` format via `tools/tmx2map.py` (873 tiles) plus a merged-run collider map (50 rects) — no XML/zlib/base64 parsing at runtime
- Menu / Pause / GameOver screens are driven by the engine's `XmlLoader` reading the existing `conan.xml` (3-frame buttons, blinking game-over text)
- Pause now rides the state stack (`pushState`/`popState`/`resume()`) instead of a bespoke flag
- Platformer physics (gravity, jump, axis-separated AABB, edge/wall probes for snail patrols, stomp detection) extracted into pure SDL-free headers with an igloo spec suite (`make test`)

### Added

- Gamepad support (Xbox/PlayStation-style, USB or Bluetooth, hot-pluggable): d-pad/left stick moves, A jumps, Start pauses; menus navigate with d-pad + A (and Up/Down + Enter on the keyboard). Deadzone and menu-wrap logic are pure and spec'd (32 specs total)
- Dropped dependencies: `tinyxml` (v1) and `zlib`; added `libstormenginev2` and `tinyxml2`

### Notes

- Requires Storm Engine v2 **v1.0.2+** — the game changes state from within states and pushes a pause overlay, which relies on the engine's deferred discarded-state deletion
- `tools/tmx2map.py` regenerates `data/conan.map` / `data/conan_colliders.map` from `data/gfx/map1.tmx` if the level changes

## [2026-05-30]

### Added

- `gameover.png` sprite sheet (380×30, 2-frame pulse animation)
- `CHANGELOG.md`

### Fixed

- `Game::clean()` was setting `m_pGameStateMachine` to null before calling `delete`, making the delete a no-op and leaking memory
- `GameStateMachine::popState()` called `resume()` on an empty state stack, causing a crash
- `LevelParser::parseObjectLayer()` used the wrong XML element when reading `animSpeed`, so animation speed was never parsed
- `GameOverState::onEnter()` used a hardcoded `assets/conan.xml` path instead of `DATA_PREFIX "conan.xml"`, causing the GameOver screen to fail to load
- `TextureManager::clearTextureMap()` and `clearFromTextureMap()` did not call `SDL_DestroyTexture`, leaking GPU memory on every state transition
- `SoundManager` was missing `Mix_Init` / `Mix_Quit` calls required by SDL_mixer 2.x for codec support; sample rate updated from 22050 to 44100
- Background music switched from OGG to MP3 to avoid SDL_mixer vorbis codec dependency
- Stray debug `std::cout` statements removed from `Player::collision()`, `ObjectLayer::update()`, and `GameOverState::onExit()`
- Redundant double-check of `m_bPressedJump` removed from `Player::handleInput()`
- Makefile had a stray `-isystem` flag with no path argument

## [2023-01-01]

### Changed

- Updated Makefile and README for Linux build
