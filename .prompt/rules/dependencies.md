---
name: Dependencies and build commands
description: Meson build and Ceedling test commands for valve.
---

# Dependencies

## Stack

- **C**: C23 (`c_std=c23`), Meson (≥ 1.4) + Ninja
- **Compiler**: GCC 14+ or Clang 18+, selected by Meson
- **Tests**: Ceedling `1.1.1` (Unity; no CMock) + gcovr
- **Changelog**: git-cliff (`cliff.toml`)

## Build

### Release

```bash
meson setup build -Dbuildtype=release
meson compile -C build
```

### Debug

```bash
meson setup build -Dbuildtype=debug
meson compile -C build
```

## Test

Meson prepares the test toolchain (compiler, gcov, gcovr) at configure time, then `meson test` runs coverage first.

```bash
meson setup build -Dtests=enabled
meson compile -C build
meson test -C build --print-errorlogs
```

Select a compiler when configuring a new build directory:

```bash
CC=gcc meson setup build-gcc -Dtests=enabled
CC=clang meson setup build-clang -Dtests=enabled
```

Generated artifacts:

- `build/ceedling-toolchain.yml` — full Ceedling tool tables for this host
- `build/valve-ceedling` — wrapper that always loads that toolchain
- `.cache/ceedling/artifacts/gcov/gcovr/valve-coverage.html` — gcovr report from `meson test`

## Direct Ceedling

After `meson setup … -Dtests=enabled`:

```bash
build/valve-ceedling test:all
build/valve-ceedling gcov:all
```

```bash
meson devenv -C build
valve-ceedling test:all
```

Do not pass raw `--mixin build/ceedling-meson.yml` paths; that file is gone. Use the wrapper (or the generated toolchain mixin) so compiler selection stays automatic.

## Changelog

```bash
git-cliff --output CHANGELOG.md
```
