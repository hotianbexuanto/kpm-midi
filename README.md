# kpm-midi

A KernelPatch module that plays MIDI sounds on load.

## Features

- Plays a C major scale when the module is loaded
- Uses Linux kernel timers for sequential note playback
- Compatible with KernelPatch framework

## Building

### Prerequisites

- KernelPatch source code
- Cross-compilation toolchain for your target platform

### Build Steps

1. Set environment variables:
   ```bash
   export TARGET_COMPILE=aarch64-linux-gnu-  # or your target platform toolchain prefix
   export KP_DIR=/path/to/KernelPatch        # path to KernelPatch source
   ```

2. Compile the module:
   ```bash
   cd kpm-midi
   make
   ```

3. The compiled module will be named `midi.kpm`

## GitHub Actions Workflow

This repository includes a GitHub Actions workflow for cloud building:

1. Go to the "Actions" tab in your GitHub repository
2. Select the "Build kpm-midi" workflow
3. Click "Run workflow"
4. Choose your target platform (aarch64, armv7a, or x86_64)
5. Click "Run workflow" to start the build

The built module will be available as an artifact after the workflow completes.

## Changelog

### v1.0.1
- Fixed GitHub Actions workflow to use `actions/upload-artifact@v4` instead of deprecated `v3`

### v1.0.0
- Initial release

## Usage

Load the module using KernelPatch tools:
```bash
kptools --load midi.kpm
```

## License

GPL v2