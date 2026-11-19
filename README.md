# AudioArts 4100 — VST3 source project

A JUCE/C++ four-band parametric EQ inspired by the AudioArts Engineering Model 4100. This is an original implementation and is not affiliated with or endorsed by AudioArts Engineering or Wheatstone.

## Implemented

- Four overlapping peaking EQ bands, with the original band centre-frequency ranges.
- Per-band frequency, bandwidth (1/6–2 octaves), gain (±16 dB), ×10 range switch and bypass.
- Master level control (±24 dB) and global EQ bypass.
- Mono and stereo operation, host automation and parameter-state persistence.
- Custom near-black rack-style UI, four coloured plastic-style knobs per band group (three per band), blue master knob, toggle switches and exactly four corner screws.
- VST3 and Standalone formats.

## Frequency ranges

| Band | ×1 | ×10 |
|---|---:|---:|
| 1 | 22–300 Hz | 220–3,000 Hz |
| 2 | 22–300 Hz | 220–3,000 Hz |
| 3 | 100–1,000 Hz | 1,000–10,000 Hz |
| 4 | 180–2,100 Hz | 1,800–21,000 Hz |

The frequency control stores its base-range value; the ×10 switch multiplies the actual centre frequency by ten. The filter is a peaking parametric EQ. Bandwidth is converted to Q using the RBJ octave-bandwidth relationship.

## Build on Windows 10/11

1. Install **Visual Studio 2022 Community** and select the **Desktop development with C++** workload (including the MSVC toolset and Windows SDK).
2. Install **Git for Windows** and **CMake** (or use the CMake included with Visual Studio).
3. Open `x64 Native Tools Command Prompt for VS 2022` in this folder.
4. Run:

```bat
cmake -S . -B build -A x64
cmake --build build --config Release
```

CMake downloads JUCE 8.0.6 automatically on the first configure, so an internet connection is needed during the first build.

The VST3 bundle will be generated under a path similar to:

```text
build\AudioArts4100_artefacts\Release\VST3\AudioArts 4100.vst3
```

Copy the `.vst3` bundle to:

```text
C:\Program Files\Common Files\VST3\
```

Then restart Ableton Live or rescan VST3 plug-ins in Preferences → Plug-Ins.

## Cloud build option

A Windows GitHub Actions workflow is included in `.github/workflows/build-windows.yml`. If this folder is pushed to a GitHub repository, the workflow can build the Windows VST3 and publish it as a downloadable workflow artifact. This still requires a GitHub account and an initial upload of the source folder.

## Important status

This archive contains the complete source project and build configuration. It is **not a precompiled or host-tested `.vst3` binary**. The binary must be produced by the Windows build steps or the included cloud workflow and then tested in Ableton Live. AudioArts hardware specifications have been used as design guidance; this plugin is an independent recreation, not a digitally authenticated circuit model of the original analogue hardware.

## Current development notes

The editor uses custom-painted rotary controls and toggle switch styling. The source is a first complete implementation and should be compiled and validated on Windows before production use. In particular, verify the VST3 scan in Ableton Live, compare the audible response and frequency display at each ×10 setting, and check the output at extreme gain settings.
