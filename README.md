# LEAK Injector

**LEAK Injector** is a lightweight Windows utility for loading DLLs into software you own or are explicitly authorized to test. It is intended for local development, debugging, modding, and security research in controlled environments.

## Features

- Process selection with name and PID search
- DLL file browser and reload workflow
- Separate x86 and x64 release builds
- ImGui-based desktop interface
- Temporary DLL-copy handling for development

## Build

Run `build_release.bat` from the project root. The script builds both architectures and places the executables in `dist`:

```text
dist/LEAK-x64.exe
dist/LEAK-x86.exe
```

Use `LEAK-x64.exe` for 64-bit target processes and `LEAK-x86.exe` for 32-bit target processes.

## Usage notice

Use this software only with applications and processes you own or have explicit permission to modify. Do not use it to bypass protections, interfere with third-party software, or access systems without authorization.

Educational and research use only.
