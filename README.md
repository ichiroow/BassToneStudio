# BassToneStudio

Low-latency bass amp simulator and multi-effects software.

The long-term goal is a modular bass rig that can reproduce artist/song-oriented tones with reusable presets while remaining suitable for realtime playing.

## Current milestone: audio foundation

Before adding amp simulation or effects, the project first establishes a stable realtime signal path:

```
Bass -> Audio Interface -> BassToneStudio -> Audio Interface -> Headphones/Speakers
```

The current application intentionally performs no amp/effect DSP. It passes input channel 1 to the outputs and displays the active sample rate, block size, input peak, and clipping warning.

Recommended first test:

- Manufacturer ASIO driver when available
- 48 kHz sample rate
- Start at 128 samples
- Move to 64 samples after 128 is confirmed stable
- Disable direct monitoring when evaluating software latency

## Stack

- C++17
- JUCE 8
- CMake 3.22+
- Windows standalone application
- Visual Studio 2022

JUCE is fetched automatically by CMake.

## Build on Windows

Requirements:

1. Visual Studio 2022 with **Desktop development with C++**
2. CMake
3. Git
4. Your audio interface manufacturer's Windows/ASIO driver

From a Developer PowerShell:

```powershell
git clone https://github.com/ichiroow/BassToneStudio.git
cd BassToneStudio

cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The first CMake configure downloads JUCE.

## Development roadmap

### Phase 1 - Realtime audio
- Stable audio input/output
- ASIO-capable device setup
- 48 kHz / 128 samples baseline
- 64 sample test
- Input/output metering
- Clipping detection
- Latency verification

### Phase 2 - Bass amp
- Input gain and headroom
- Preamp saturation
- Bass/mid/treble EQ
- Power amp stage
- Cabinet IR
- Output protection

### Phase 3 - Multi-effects
- Noise gate
- Compressor
- Drive/distortion
- Parametric EQ
- Modulation
- Delay
- Reverb
- Reorderable effect chain

### Phase 4 - Presets
- Save/load rigs
- Artist presets
- Song presets
- Instrument compensation profiles

### Phase 5
- VST3 target
- Preset browser and polished UI

## Realtime coding rule

The audio callback must remain deterministic. Avoid heap allocation, file I/O, GUI work, blocking locks, and other unpredictable work on the audio thread.
