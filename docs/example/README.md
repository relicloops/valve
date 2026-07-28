# Valve examples

These programs exercise Valve as real executables. They use the same schema and lifecycle so their assignment behavior can be compared directly.

## Programs

- [`inline.c`](inline.c) builds `demo-inline` with `VL_ASSIGN_INLINE`. Option values use the same argument as the flag, such as `--proxy.cpus=4`.
- [`separate.c`](separate.c) builds `demo-separate` with `VL_ASSIGN_SEPARATE`. Option values use the following argument, such as `--proxy.cpus 4`.

Both programs define global options, a `network` verb, and a nested `page add` command. Together they demonstrate typed targets, dotted options, arrays, key/value data, durations, required options, option requirements, conflicts, automatic help and version handling, structured errors, parsed-value access, and target cleanup.

## Link Valve

The local [`meson.build`](meson.build) resolves Valve with `dependency('valve')` and passes the resulting dependency to both executables. Meson reads the installed `valve.pc`, adds the public `include/valve` path, and links the platform-standard `libvalve` library name.

Install Valve from the repository root before configuring the examples:

```bash
meson setup build --buildtype=release --prefix="$HOME/.local"
meson compile -C build
meson install -C build
```

Then configure and build the example project:

```bash
meson setup docs/example/build docs/example
meson compile -C docs/example/build
```

If Valve is not installed, the dependency fallback expects a pinned `subprojects/valve.wrap` that exposes `valve_dep`. Follow the [Wrap workflow](../../README.md#quick-start) and pin a published Valve version tag rather than `main`.

## Run the examples

Inline assignment keeps each flag and value together:

```bash
docs/example/build/demo-inline network --proxy.lane=api/v1 --proxy.cpus=4 --tags=main,edge --timeout=1h30m --enable-log --verbose
```

Separate assignment places each value in the next argument:

```bash
docs/example/build/demo-separate network --proxy.lane api/v1 --proxy.cpus 4 --tags main,edge --timeout 1h30m --enable-log --verbose
```

Both executables also support `--help`, `--version`, `--help=network`, and `--help=page.add`. Use the `page add` command to see required options, requirements, and conflicts in the same schema.

## Lifecycle

Each program creates a parser with `vl_create()`, parses `argc` and `argv` with `vl_parse()`, prints structured errors or parsed targets, destroys parser state with `vl_destroy()`, and releases Valve-owned target values with `vl_targets_clear()`.

The examples use static zero-initialized target storage. Reserved names and short options remain unavailable to the schema unless `VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED` is enabled, so `--verbose` intentionally has no `-v` alias.
