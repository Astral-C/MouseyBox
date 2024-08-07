## MouseyBox

A small personal 2D game engine/framework.

## Features
 - Custom archive format '.chse' + tools for creating/extracting archives
 - Custom scripting language `Skitter`
 - Support for protracker MOD modules
 - Compatible with devkitpro/devkitarm to target for Gamecube, Switch, Wii, and really anything that supports SDL2 and SDL2_ttf

### Building the Example

For Desktop

`cmake . -Bbuild -DEXAMPLE=True`

For GameCube/Switch make sure you have the relevant devkitpro/devkitarm libraries installed (check the devkitpro pacman repos)

`make -f Makefile.(platform)`

This is mostly for personal use, but if you use it, let me know! If you release anything with it credit would also be appreciated, but isn't nessesary :3