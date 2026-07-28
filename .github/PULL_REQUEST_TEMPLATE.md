## Summary

<!-- What does this change, and why? Keep Valve experimental-status honest if the API moves. -->

## Test plan

- [ ] `meson setup build` (or reconfigure) and `meson compile -C build`
- [ ] `meson test -C build --print-errorlogs` (unit tests + gcovr HTML)
- [ ] Demo smoke (if behaviour changes): help / version / happy path / error path

## Notes for reviewers

<!-- API impact, schema examples, or areas that need a careful look. -->

See [CONTRIBUTING.md](../CONTRIBUTING.md) for local setup.
