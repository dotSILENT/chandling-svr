# CHandling Server


This is a SA:MP server plugin allowing server control of vehicle handlings by communicating with a client-sided plugin ([chandling](https://github.com/dotSILENT/chandling))

Handlings can also be applied per-vehicle, which is not the default behaviour of GTA.

Head over to the [Wiki](https://github.com/dotSILENT/chandling-svr/Wiki) for a list of natives, descriptions and examples.


## Installation
1. Download the latest [release](https://github.com/dotSILENT/chandling-svr/releases) for your system (windows/linux)
2. Copy the plugin file (`.dll` or `.so`) to `plugins` directory in your game mode and the include to your includes dir
3. Include the plugin's include file: `#include <chandlingsvr>`

## Compiling
This plugin is set up using CMake, for a small tutorial check out [this](https://github.com/Zeex/sampgdk/wiki/Setting-up-GDK-with-CMake)

## Thanks to
* Zeex - for sampGDK and SubHook
* kurta999 - for AMX native redirection code (taken from YSF)
* Whitetiger - for GetPacketId hook (taken from SAMP ACv2)
* DK22Pac - for his [plugin sdk](https://github.com/DK22Pac/plugin-sdk) that I took some structs from
