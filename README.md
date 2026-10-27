# DJ Mugumya Pro Sound Enhancer
### Native C++ Audio DSP Plugin for VirtualDJ (Targeting 2021 through 2025 & 8.x)
Created by DJ Mugumya Audio Labs.

## 📌 IMPORTANT NOTICE: Web Interface vs. Native Windows DLL
- The web interface is an interactive in-browser audio testing workbench.
- VirtualDJ requires the compiled native Windows 64-bit DLL (DJMugumyaProSoundEnhancer64.dll).
- In this Linux cloud environment, the Windows DLL has not yet been compiled.
- Follow the beginner-friendly steps below to build it on your Windows computer.

## 🎯 Version Support
- Target: VirtualDJ 2021, 2023, 2024, 2025 (and VirtualDJ 8.x backward compatible).
- Stem FX: In VirtualDJ 2021-2025, can be inserted on Master, Decks, or Stems (Vocal, Bass, Drums, Melody).
- Verification: Must be tested on your target VirtualDJ installation before claiming certification.

## 🚀 How to Build on Windows
1. Install Visual Studio 2022 Community with the "Desktop development with C++" workload.
2. Download the project ZIP from the web app (click "DOWNLOAD FULL C++ PROJECT (.ZIP)").
3. Extract the ZIP file.
4. Double-click build_msvc.bat (or open DJMugumyaProSoundEnhancer.sln, select Release | x64, and press Ctrl+Shift+B).
5. Copy bin\x64\Release\DJMugumyaProSoundEnhancer64.dll to:
   %LOCALAPPDATA%\VirtualDJ\Plugins64\SoundEffect\
   or
   %USERPROFILE%\Documents\VirtualDJ\Plugins64\SoundEffect\