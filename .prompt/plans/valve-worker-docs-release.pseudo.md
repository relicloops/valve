---
name: valve-worker-docs-release
description: Publish the Valve 1.1 documentation through the relicloops-edge Worker
severity: high
status: Worker deployed and accepted; selected source and commit follow-ups complete
---

# Publish Valve 1.1 documentation through the edge Worker

`valve.relicloops.org` is a static hostname served by the `relicloops-edge` Cloudflare Worker. The authored site lives in `/Volumes/a-ssd/CODE/relicloops/websites` (plural), the generated production tree is `deploy/valve.relicloops.org`, and `/Volumes/a-ssd/VM/core.os-arm-v1/scripts/90-edge-assets.sh` derives `worker/assets/valve.relicloops.org` from that tree. Deployment is a Wrangler upload from `core.os-arm-v1/worker`; it does not build or restart the retired lighttpd container and does not require a CoreOS service restart.

The live site still identifies Valve `1.0.1-000` and asset generation `1.0.1-003`. The Architecture branch already contains commit `1514dff`, which prepares Valve `1.1.0-001`, asset generation `1.1.0-004`, and the new signed source snapshot. Before deployment, follow up that commit so the source no longer describes the retired container as the publisher and so the command-tail feature has a useful public contract rather than only a status-line mention.

## Execution record — 2026-09-02

- [x] Verified signed tag `v1.1.0-001`; the regenerated allowlisted archive and website archive both hash to `0091c9ad395cff4bb548617fddfa0d8bcb718d54dd86740c04aced4209a1004d`.
- [x] Added and checked the Valve-scoped `--site valve.relicloops.org` sync path; its stale check is read-only and detects content differences.
- [x] Synchronized only `deploy/valve.relicloops.org` into the Worker asset tree; the corrected generation records 16 files, 508635 bytes, and digest `65b1ef9b7137ed80`.
- [x] Recorded Worker version 23 (`5c54ded7-27a7-4e28-8545-01a502e6a47e`) as the rollback target.
- [x] Ran Wrangler 4.107.1 dry-run with the existing bindings and routes.
- [x] Deployed Worker version 24 (`5cfe28e5-700b-4225-a174-27b0323911b0`) with message `Publish Valve v1.1.0-001 documentation`.
- [x] Required pages returned 200, missing-path behavior returned 404, release metadata reported `v1.1.0-001` and asset generation `1.1.0-004`, and the public archive passed its published SHA-256 check.
- [x] Probed every routed hostname after the shared-Worker deployment; expected 200 and 301 responses remained intact.
- [x] Corrected the active/public retired-container wording, expanded the public command-tail contract, bumped asset generation to `1.1.0-005`, and created signed Architecture commit `53d8110`.
- [x] Uploaded only the seven reviewed Valve asset changes, activated Worker version 25 (`2ab1ced1-136c-4be7-b6c0-1e1183196638`) at 100%, and retained version 24 as rollback.
- [x] Created signed CoreOS commit `7c6afa7` containing the scoped sync script, inventory, changelog, and Valve asset changes without the unrelated sysext or service dirt.

Wrangler reported eight additional uploads from already-committed asset state for DNS-only `dod.skinjo.org` and `simonedelpopolo.com`. Those hostnames are not routed through this Worker, and their public origin probes remained healthy. A dry-run did not disclose that remote asset delta. Future shared-Worker releases should use `wrangler versions upload`, inspect its asset report, and then activate the reviewed version with `wrangler versions deploy <version-id>@100% --yes`.

## Boundaries

- Keep Valve, Architecture, and CoreOS commits separate and GPG-signed.
- Preserve the existing dirty Architecture and CoreOS worktrees. Stage exact paths only; never absorb the staged `llm/` work, Lab UI work, sysext work, or service configuration dirt.
- Do not mention Key anywhere in Valve's public site, release notes, or downloadable documentation. The feature is documented generically as an opaque command tail.
- Do not hand-edit `core.os-arm-v1/worker/assets/`; it is derived by `scripts/90-edge-assets.sh`.
- Do not change DNS, Worker routes, tunnels, cron triggers, KV, email bindings, or `worker/src/index.js`. The existing `valve.relicloops.org/*` route already selects the static asset path.
- Keep credentials in the ignored CoreOS Worker credential state. Never print, stage, copy, or embed token values.

## Phase 1 — correct the Architecture source

- [ ] Re-read the current Architecture status and exact diff for `1514dff`; confirm no later task has changed the Valve site paths.
- [ ] Move the snapshot generator out of the retired `container/valve.relicloops.org/` deployment directory into a source-owned Valve site script location, then update `internal/chrome.tsx` and `manifest.tsx` comments to name the new path.
- [ ] Retire the remaining Valve lighttpd-only files and stale runbook instructions in one isolated cleanup commit, or explicitly retain them as historical material without treating their `VERSION` as the site version. `VALVE_SITE_VERSION` is the active Worker asset cache-buster.
- [ ] Keep `VALVE_VERSION = "1.1.0-001"`, `VALVE_SITE_VERSION = "1.1.0-004"`, the signed-tag release URL, the v1.1 archive, and its SHA-256 digest synchronized.
- [ ] Add a concise command-tail section to the public Concepts or Install source: `VL_OPTION_VALUE_COMMAND`, `VL_TARGET_COMMAND`, `-- <command> [args…]`, borrowed zero-copy lifetime, and the command-schema-only `argv[argc] == NULL` requirement.
- [ ] Keep operands honest: ordinary positional arguments remain unsupported and only an unowned `--` tail retains the old discard behavior.
- [ ] Audit the generated archive before publication. It must contain the public build, source, header, docs, and tests; it must exclude coordination plans and agent symlinks.
- [ ] Resolve the Architecture rule conflict before regenerating: the operational edge runbook names `bun run prod:valve.relicloops.org`, while `.prompt/rules/build-workflow.md` currently forbids Bun build commands. Obtain a maintainer override for this one build or correct the rule; do not silently bypass it.
- [ ] Regenerate `deploy/valve.relicloops.org`, then inspect the output for `v1.1.0-001`, `v=1.1.0-004`, the command-tail text, the release URL, and the expected snapshot digest.
- [ ] Run the repository changelog procedure, create narrowly scoped signed Architecture commits, verify their signatures, and push only the current `wrangler` branch after review.

## Phase 2 — make the CoreOS asset sync Valve-scoped

The current `./scripts/90-edge-assets.sh --check` reports unrelated `skinjo.org` output as stale. The existing mutating form rebuilds every hostname before returning that failure, so it is not an acceptable Valve-only release command.

- [ ] Add a narrow selector such as `--site valve.relicloops.org` to `scripts/90-edge-assets.sh`.
- [ ] Make the selector validate against `EDGE_SITES`, check and synchronize only the selected host, and leave every unselected `worker/assets/<host>` directory byte-identical.
- [ ] Keep `inventory/edge-assets.env` truthful: update the sync timestamp and Valve file/byte/digest records without replacing unrelated recorded host state.
- [ ] Verify `./scripts/90-edge-assets.sh --check --site valve.relicloops.org` is read-only and returns nonzero when the generated Architecture tree differs from the Worker asset tree.
- [ ] Run `./scripts/90-edge-assets.sh --site valve.relicloops.org` only after the Architecture output is final.
- [ ] Confirm the CoreOS diff is limited to `scripts/90-edge-assets.sh`, `worker/assets/valve.relicloops.org/`, and Valve's entries in `inventory/edge-assets.env`. The expected content change replaces the 1.0.1 pages with 1.1.0 pages and adds the v1.1 archive/checksum pair.
- [ ] Re-run the targeted `--check`; require success before any Wrangler command.
- [ ] Create and verify a signed CoreOS commit without touching the existing sysext, nftables, Jack, Iris, or lifetime-build dirt.

## Phase 3 — Wrangler preflight and deployment

- [ ] From `core.os-arm-v1/worker`, confirm Wrangler 4.x is available and `wrangler whoami` identifies the intended RelicLoops account without rendering credentials.
- [ ] Run `wrangler deploy --dry-run` against `worker/wrangler.toml`; inspect the static asset delta and refuse unexpected route, binding, trigger, or Worker-code changes.
- [ ] Record the currently deployed Worker version with `wrangler versions list` as the rollback target.
- [ ] Run `wrangler deploy` from `core.os-arm-v1/worker` only after source, derived assets, commits, and dry-run are approved.
- [ ] Record the returned deployment/version identifier and deployment time in the plan or handoff.

## Phase 4 — live acceptance

- [ ] Fetch `https://valve.relicloops.org/status.html` with a unique query string and require Valve `v1.1.0-001`, asset generation `v=1.1.0-004`, the command-tail capability, and the corrected unowned-tail limitation.
- [ ] Fetch `/`, `/install.html`, `/concepts.html`, `/status.html`, `/download.html`, `/search.html`, `/sitemap.xml`, and `/search-index.json`; require successful responses and consistent release metadata.
- [ ] Download `/dist/valve-1.1.0-001.tar.gz` and its checksum from the public hostname, recompute SHA-256 locally, and require an exact match with the site source.
- [ ] Confirm a missing path still returns the Worker-owned 404 page and that no origin/fallback headers appear for this static-only hostname.
- [ ] If acceptance fails, run `wrangler rollback <recorded-version-id>`, repeat the public probes, and leave the failed source/asset commits intact for diagnosis rather than rewriting history.

## Key readiness after Valve 1.1

Key's repository now declares `dependency('valve', version: '>=1.1.0', static: false)` on `main`, so its source contract is ready and shared linkage is preserved. The installed build dependency is not ready yet: both this Mac's tooling prefix and the existing `vm-relicloops` Valve build report `1.0.1-000`, and the VM does not currently expose a Valve pkg-config file on its default path.

- [ ] Sync the released Valve `v1.1.0-001` source to the build host and run the estate's `build-native valve install` path so `valve.pc`, headers, and the shared library land in the persistent development prefix.
- [ ] With the same `PKG_CONFIG_PATH` used by `build-native`, require `pkg-config --modversion valve` to return `1.1.0-001` before configuring Key.
- [ ] Increment Key's lifetime BUILD before any Key build, as required by Key's root `.rules`.
- [ ] Build Key and confirm Meson resolves Valve `1.1.0-001` with `static: false`; do not claim runtime use of `VL_OPTION_VALUE_COMMAND` until Key's separate `06-key-exec` plan implements and accepts the `exec` verb.

## Completion

This plan is complete only when the Worker deployment is live and publicly verified, the downloadable bytes match the signed Valve tag, unrelated worktree dirt remains untouched, and Key's dependency statement is described accurately as source readiness rather than installed/runtime proof.
