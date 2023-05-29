# Conan-the-Caveman-Linux

Example project from ['SDL Game Development'][2] by Shaun Mitchell. The book explains everything from a Windows user perspective so this repo exists as an alternative for those who want to learn on Linux, Windows Subsystem for Linux, or MacOS.

![text](conan-screen.png)

## Getting Started

Install pre-requisites to get the code working under an Ubuntu-based system (i.e., Linux Mint, Windows Subsystem for Linux, etc.):

```bash
sudo apt update
sudo apt install libxext-dev build-essential automake autoconf libtool -y
sudo apt install mesa-common-dev libasound2-dev libpulse-dev -y
sudo apt install libpng-dev libjpeg-dev libwebp-dev libtiff-dev libmodplug-dev -y
sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev -y
sudo apt install libtinyxml-dev tiled  -y #for the .tmx files, Tiled Map Editor
```

Then:

```bash
make        # to build the project
make run    # to execute the built binary
```

[1]: https://gist.github.com/WillSams/e2bb2874ace22b90f90f
[2]: https://www.packtpub.com/game-development/sdl-game-development
