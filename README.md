# Conan-the-Caveman-Linux

A side-scrolling caveman platformer, rebuilt on [**Storm Engine v2**](https://github.com/WillSams/storm-engine-v2).

Originally the example project from ['SDL Game Development'][2] by Shaun Mitchell; the game has since been re-architected on the engine's `Game` + `GameStateMachine` + `AssetStore`, with the level converted from Tiled TMX to the engine's native map format and the platformer physics extracted into pure, unit-tested headers.

![text](conan-screen.png)

## Requirements

- **Storm Engine v2 v1.0.2+** (`libstormenginev2` + `stormengine2` headers) — pre-built `.deb` packages are on the engine's [Releases](https://github.com/WillSams/storm-engine-v2/releases) page. v1.0.2 is required: the game changes state from within states and pushes a pause overlay, which relies on the engine's deferred state deletion.
- SDL2 and extensions: `sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev`
- tinyxml2: `sudo apt install libtinyxml2-dev`
- [igloo](https://github.com/joakimkarlsson/igloo) (header-only) — only needed for `make test`

## Build, run, test

```bash
make            # builds bin/conan-game
make run
make test       # igloo specs for the pure platformer logic
```

## Controls

| Keyboard | Gamepad | Action |
|---|---|---|
| Arrows / WASD | D-pad / left stick | Move |
| Space / Up / W | A | Jump |
| Esc / P | Start | Pause (resume from the overlay) |
| Up/Down + Enter | D-pad + A | Navigate menus |

Gamepads (Xbox/PlayStation-style, USB or Bluetooth) are detected automatically, including hot-plugging mid-game. Stomp the snails; touching one any other way sends Conan to the game-over screen.

## Layout

```text
src/game.*            Engine shell: window, renderer, GameStateMachine
src/states/           Menu, Play, Pause (pushed on the state stack), GameOver
src/physics/          Pure platformer physics (gravity, jump, AABB, stomp)
src/input/            Gamepad wrapper + pure deadzone/menu-nav logic
src/level/            Collider-map parser
src/ui/               3-frame button logic + XML-driven button screens
src/sprites/          Player sheet frame table (irregular rows)
specs/                igloo specs for the pure headers (make test)
tools/tmx2map.py      Converts data/gfx/map1.tmx -> data/conan.map + colliders
data/conan.xml        Menu/Pause/GameOver screens (engine XmlLoader format)
```

The original Tiled level (`data/gfx/map1.tmx`) is still the source of truth for the map — edit it in [Tiled][3] and re-run `python3 tools/tmx2map.py` to regenerate the engine-format files.

[2]: https://www.packtpub.com/game-development/sdl-game-development
[3]: https://www.mapeditor.org/
