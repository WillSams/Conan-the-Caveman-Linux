# Changelog

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
