# kpm-midi

A KernelPatch module that would play MIDI sounds on load (simplified version).

## Features

- Logs a message when the module is loaded
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

### v1.0.3
- Simplified module implementation to remove Linux kernel timer dependencies
- Removed MIDI playback functionality due to compatibility issues with KernelPatch

### v1.0.2
- Fixed GitHub Actions workflow directory structure issue

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

## Troubleshooting

If you encounter loading errors with the module, try the following:

1. Check kernel logs for detailed error messages:
   ```bash
   dmesg | grep kpm-midi
   ```

2. Ensure KernelPatch is properly installed and compatible with your kernel version.

3. Verify that all required KernelPatch headers and libraries are available during compilation.

4. Check that the module is compiled for the correct target platform and kernel version.

5. If using the GitHub Actions workflow, ensure that the KernelPatch repository is properly cloned and accessible.