# Security Policy

## Supported versions

Valve is **experimental `1.0.x`** (Apache-2.0). Only the **latest published
`1.0.x`** line is considered for security fixes. Older tags and pre-release
scratch builds are unsupported. The API may change while Valve remains
experimental.

| Version | Supported         |
|---------|-------------------|
| 1.0.x   | Yes (latest only) |
| < 1.0   | No                |

## Reporting a vulnerability

**Do not** open a public GitHub issue for security-sensitive reports.

Prefer GitHub’s private advisory flow:

1. Open a [private security advisory](https://github.com/relicloops/valve/security/advisories/new) on this repository.
2. Include Valve version as `MAJOR.MINOR.PATCH-BUILD` (e.g. `1.0.0-000` from
   `vl_version_get()` / `meson.build`), OS, compiler, and a minimal `argv` or
   schema repro when possible.
3. Allow reasonable time for triage before any public disclosure.

If private advisories are unavailable for your account, contact the maintainers
listed on the [relicloops](https://github.com/relicloops) GitHub organization,
and mark the report as security-sensitive.

## Scope notes

Valve is a CLI argument parser (schema, parse, help/reserved, color probes,
errors/targets helpers). Security issues in applications that *use* Valve belong
in those applications’ repositories unless the defect is clearly in Valve itself.
