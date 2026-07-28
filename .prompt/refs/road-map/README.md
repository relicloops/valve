# Valve road-map microsite

This microsite is the navigable planning and status surface for Valve. It turns roadmap content into page-level views so you can review parser architecture, implementation priorities, and public-release readiness without scanning large markdown files.

## What it means

This site answers three questions:

- What Valve already does, by subsystem and behavior surface.
- What remains to ship for the experimental public release.
- How current repository state maps to release checklist progress.

The content is operational, not marketing. It is intended for maintainers and contributors coordinating release work.

## How pages are organized

- `index.html` gives the system overview and current high-level status.
- `parsing.html`, `forms.html`, `constraints.html`, and `help.html` break down parser contracts and behavior.
- `roadmap.html` tracks implementation priorities and missing capabilities.
- `public.html` is the canonical release checklist view and readiness summary.

## How to use it during release work

1. Scan repository state and map it to release checklist status.
2. Update `public.html` and `index.html` to reflect that status.
3. Open `public.html` to validate checklist counts and visible status language.
4. Keep remaining blockers explicit: first commit/push on the clean tree, then annotated <code>v1.0.0-000</code> + Release. Multi-OS CI and public visibility are already done. Live docs site is not a blocker.
5. Use this site as the shared view while other agents modify product files.

## Scope and ownership

This directory is a documentation/status workspace under `.prompt/refs/road-map/`. It does not define library API behavior by itself; it reflects current repository state and release intent.
