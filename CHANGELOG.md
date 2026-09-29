# Changelog

All notable changes to this project are documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and this project uses [Conventional Commits](https://www.conventionalcommits.org/).

## [2.0.2-015] - 2026-09-29

### Fixed

- **tu**: Include the declaring header in two prototype-less units

### Refactored

- **parse**: Split the parser into per-function units
- **c23**: Use nullptr and drop stdbool.h

### Documentation

- **rules**: Add C formatting and vertical spacing
- **include**: Add Doxygen comments to color.h and valve.h

### Build

- **meson**: Version the shared library as libvalve.so.2
- **meson**: Drop _GNU_SOURCE from the library c_args
## [2.0.0-011] - 2026-09-11

### Fixed

- **parse**: Make key/value literals strict and predictable

### Documentation

- **prompt**: Drop GEMINI.md from the symlink note
## [1.2.0-008] - 2026-09-11

### Added

- **parse**: Add executable actions

### Documentation

- **plans**: Record the command-option and worker docs releases
- **road-map**: Mark 1.1 release published

### Build

- **version**: Make VERSION authoritative and bump to 1.1.0-002
## [1.1.0-001] - 2026-09-02

### Added

- **parser**: Capture schema-owned command tails

### Documentation

- **road-map**: Record 1.1 release gates
## [1.0.1-000] - 2026-08-08

### Fixed

- **help**: Resolve dotted option names under a verb or sub-verb

### Documentation

- **plans**: Update completed plan location
- Regenerate CHANGELOG for the docs-site entries
- **road-map**: Reconcile the status surface with the shipped site
- Point the public docs at valve.relicloops.org
- Add GitHub community standards files
## [1.0.0-000] - 2026-07-28

### Documentation

- Generate CHANGELOG from conventional history

### Initial

- Experimental Valve 1.0.0-000 public baseline

