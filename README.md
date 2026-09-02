# valve

*A command line should feel designed, not decoded.*

[![CI](https://github.com/relicloops/valve/actions/workflows/ci.yml/badge.svg)](https://github.com/relicloops/valve/actions/workflows/ci.yml)
[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![Status](https://img.shields.io/badge/status-experimental-orange.svg)](#valve)
[![Version](https://img.shields.io/badge/version-1.0.1--000-lightgrey.svg)](meson.build)
[![Linux](https://img.shields.io/badge/Linux-supported-FCC624?logo=linux&logoColor=black)](.github/workflows/ci.yml)
[![macOS](https://img.shields.io/badge/macOS-supported-000000?logo=apple&logoColor=white)](.github/workflows/ci.yml)

## Introduction

A command line is where a program first meets its users. `valve` helps C programs make that meeting predictable: describe the interface as a schema, then let the library handle the repetitive, failure-prone work of parsing it.

Declare accepted options and, when needed, verbs and sub-verbs. `vl_parse` validates `argc` / `argv`, converts values to their declared types, and can fill caller-owned config structs. Built-in help and version banners, terminal-aware color, structured parse errors, and target teardown keep the whole lifecycle in one small library with no third-party runtime dependencies.

Documentation lives at [valve.relicloops.org](https://valve.relicloops.org), including a quick start, the parsing concepts, current status, and a verified source download.

## Status and limitations

Valve `1.x` is experimental. Minor releases may change the public source or ABI contract, so consumers should pin an exact version tag. Valve will announce when the experimental status is lifted; from that release onward, breaking public API or ABI changes require a major-version bump, backward-compatible additions require a minor-version bump, and fixes require a patch-version bump. The final `BUILD` component identifies the published build.

Operands are not implemented. `VL_BEHAVIOR_ACCEPT_OPERANDS` is reserved and currently has no effect, so ordinary positional arguments are rejected. A schema may instead declare one `VL_OPTION_VALUE_COMMAND` in its active verb chain to capture the opaque tail after `--`; without that declaration, Valve preserves the legacy behavior of discarding the tail. Do not rely on operand storage or accessors until they are added to the public header.

For the current metadata contract and other parser boundaries, see the [reference guide](docs/reference/README.md).

## Quick start

Create a small project with three files:

```text
valve-demo/
├── main.c
├── meson.build
└── subprojects/
    └── valve.wrap
```

### `main.c`

Declare the accepted options, give the schema to Valve, and parse:

```c
#include "valve/valve.h"

#include <stdio.h>

int main(int argc, char **argv) {
  char *name = NULL;

  /* Describe --name=<value> and let Valve populate name. */
  const vl_option_t name_option = {
      .name = "name",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_STRING,
      .data = &name,
      .target = VL_TARGET_STRING,
  };
  const vl_option_t *const options[] = {&name_option, NULL};

  /* Schemas use NULL-terminated option tables. */
  const vl_executable_t settings = {
      .program_name = "demo",
      .program_version = "0.1.0",
      .options = options,
  };
  valve_t *v = vl_create(&settings);

  if (!v)
    return 1;

  /* Parse, print structured errors, then release parser state. */
  if (vl_parse(v, argc, argv) != 0) {
    vl_errors_print(v, stderr);
    vl_destroy(v);
    vl_targets_clear(&settings);
    return 2;
  }

  /* Help and version are printed by Valve; only run the app otherwise. */
  if (!vl_reserved_fired(v))
    printf("Hello, %s\n", name ? name : "world");

  vl_destroy(v);
  vl_targets_clear(&settings);
  return 0;
}
```

### `subprojects/valve.wrap`

Valve consumers pin a version tag rather than tracking `main`:

```ini
[wrap-git]
directory = valve
url = https://github.com/relicloops/valve.git
revision = v1.0.1-000
```

`v1.0.1-000` is the current published tag. When upgrading, change `revision` to the latest published Valve version tag.

### `meson.build`

```meson
project(
  'valve-demo',
  'c',
  default_options: ['c_std=c23'],
)

valve_proj = subproject('valve')
valve_dep = valve_proj.get_variable('valve_dep')

executable('demo', 'main.c', dependencies: valve_dep)
```

Meson resolves Valve from the pinned Wrap and exposes its `valve_dep` dependency.

### Command tails

Declare a command option when the program needs to forward everything after `--` unchanged:

```c
vl_command_t command = {0};
const vl_option_t command_option = {
    .name = "command",
    .value = VL_OPTION_VALUE_COMMAND,
    .required = true,
    .data = &command,
    .target = VL_TARGET_COMMAND,
};
```

For `demo -- generator cron list`, `command.argv` points at `generator` and `command.argc` is `3`. The tail is borrowed from the main-style, NULL-terminated `argv`, remains suitable for `execvp`, and is not copied or freed by Valve. Tokens such as `--help` inside the tail belong to the forwarded command. Call `vl_targets_clear()` after the last use to zero the command view.

### Build and run

```bash
meson setup build
meson compile -C build

./build/demo --help
./build/demo --version
./build/demo --name=Valve
```

The final command prints `Hello, Valve`. See the [examples](docs/example/README.md) for complete programs and the [reference guide](docs/reference/README.md) for parser behavior, relationships, terminal capabilities, errors, and ownership.

## Install

Install Valve from a source checkout to a prefix:

```bash
meson setup build --buildtype=release --prefix="$HOME/.local"
meson compile -C build
meson install -C build
```

This installs `libvalve`, the public headers under `include/valve`, and `valve.pc`. Meson can then consume the installed Valve directly:

```meson
valve_dep = dependency('valve')
```

You can also link Valve through pkg-config:

```bash
cc -std=c23 $(pkg-config --cflags valve) main.c \
  $(pkg-config --libs valve) -o demo
```

## Development

To work on Valve itself, clone the repository:

```bash
git clone https://github.com/relicloops/valve.git
cd valve
```

### Testing with Ceedling

Ceedling requires Ruby, Meson 1.4 or newer, and a compiler that can build C23 (GCC 14+ or Clang 18+). Install Ruby, Meson, Ninja, and the compiler with your system package manager, then install the pinned test runner and coverage reporter:

```bash
gem install ceedling -v 1.1.1
python3 -m pip install gcovr
```

Meson is the front door for both the library build and the test system. At configure time it selects the C compiler (and the matching gcov/gcovr tools for the host OS and architecture), writes a Ceedling toolchain mixin, and installs a thin wrapper. The first automated run is coverage through Meson:

```bash
meson setup build -Dtests=enabled
meson compile -C build
meson test -C build --print-errorlogs
```

That `meson test` step runs Ceedling `gcov:all` and produces `.cache/ceedling/artifacts/gcov/gcovr/valve-coverage.html`.

Meson uses the platform default C compiler unless you set `CC` for a new build directory, for example `CC=gcc meson setup build-gcc -Dtests=enabled` or `CC=clang meson setup build-clang -Dtests=enabled`. You do not pass compiler paths into Ceedling by hand.

After configure, use the generated wrapper for direct Ceedling work with the same toolchain:

```bash
build/valve-ceedling test:all
build/valve-ceedling gcov:all
```

Or open a devenv so `valve-ceedling` is on `PATH`:

```bash
meson devenv -C build
valve-ceedling test:all
```

See [Contributing](CONTRIBUTING.md) for prerequisites, test organization, and CI details.

> [!NOTE]
> If clangd or another LSP reports missing `unity.h`, `test_support.h`, or `TEST_SOURCE_FILE` errors under `test/`, generate Ceedling's compilation database:
>
> ```bash
> meson setup build -Dtests=enabled
> meson test -C build --print-errorlogs
> ```
>
> This creates `.cache/ceedling/artifacts/compile_commands.json`. The project `.clangd` uses that database for Ceedling tests while retaining Meson's `build/compile_commands.json` for library sources. Restart the language server after generating the database.

## License

Valve is licensed under Apache-2.0. See the [license](LICENSE).
