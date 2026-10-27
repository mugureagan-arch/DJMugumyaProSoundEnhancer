# DJ Mugumya Pro Sound Enhancer - Compatibility & Installation Guide
## Targeting VirtualDJ 2021 through VirtualDJ 2025 (and VirtualDJ 8.x)

## 1. Version Compatibility Matrix & SDK Architecture
The VirtualDJ Plugin SDK v8 (vdjPlugin8.h, vdjDsp8.h) is Atomix Productions' official binary interface standard across modern 64-bit VirtualDJ builds.

- VirtualDJ 2025 (Build 8000+): Official SDK v8, Master FX, Deck FX, Stem FX
- VirtualDJ 2024 (Build 7800+): Official SDK v8, Master FX, Deck FX, Stem FX
- VirtualDJ 2023 (Build 7400+): Official SDK v8, Master FX, Deck FX, Stem FX
- VirtualDJ 2021 (Build 6064+): Official SDK v8, Master FX, Deck FX, Stem FX
- VirtualDJ 8.x (Build 2000+ Min): Backward compatible, Master FX, Deck FX

Verification Policy: The plugin is architected strictly to the official Atomix C++ SDK v8 interfaces. Do not claim final certification on any specific build until you have compiled the native DLL and tested it inside your local installation of VirtualDJ.

## 2. Windows Installation Directories
Copy DJMugumyaProSoundEnhancer64.dll to either:
1. Modern Default (VirtualDJ 2021 - 2025):
   %LOCALAPPDATA%\VirtualDJ\Plugins64\SoundEffect\
2. Classic Documents (VirtualDJ 8.x / 2020):
   %USERPROFILE%\Documents\VirtualDJ\Plugins64\SoundEffect\

## 3. How to Compile
1. Open DJMugumyaProSoundEnhancer.sln in Visual Studio 2022.
2. Select Release | x64 and press Ctrl + Shift + B.
3. Output DLL is at bin\x64\Release\DJMugumyaProSoundEnhancer64.dll.