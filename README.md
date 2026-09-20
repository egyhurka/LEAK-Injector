# LEAK Injector

**LEAK Injector** is a modern, lightweight Windows DLL injection utility. Designed for developers, reverse engineers, and security researchers, it provides a highly reliable and user-friendly way to inject dynamic-link libraries (DLLs) into external target processes.

## Features
*   **Multiple Injection Methods:**
    *   *Simple (Standard/LoadLibrary):* The classic, highly stable Windows API-based approach.
    *   *Thread Hijacking (WIP):* Suspends and redirects an existing target thread to bypass common API hooks.
    *   *Manual Mapping (WIP):* An advanced technique that bypasses the Windows loader, mapping the PE headers and sections manually in memory for maximum stealth.

## Build

Run `build_release.bat` to create the single-file Release build in `dist/LEAK.exe`.

Use only with software and processes you own or are authorized to modify.

Educational use only.
