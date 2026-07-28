---
name: commit-guidelines
description: Conventional commit format, scoping rules, commit message structure, and the guardrails that keep history clean and signed.
---

# Commit Guidelines

This skill is how you commit. Follow it as a procedure, not a vibe. When
any step is unclear, **stop and ask the user** before touching git.

## 0. Golden rules (non-negotiable)

These hold for every commit, every project, every branch:

1. **Never commit until the user explicitly asks.** "Looks good" is not a
   request to commit. "Commit it" / "ship it" / "go ahead" is.
2. **Every commit must be GPG-signed.** If signing fails, stop and ask
   the user to fix GPG. Never pass `--no-gpg-sign`. Never pass
   `--no-verify`. Never disable hooks or signatures to "unblock" work.
3. **Never run destructive git operations without explicit consent:**
   `git reset --hard`, `git push --force` / `--force-with-lease`,
   `git checkout .`, `git restore .`, `git clean -f`, `git branch -D`,
   `git rebase` on a pushed branch, `git commit --amend` on a pushed
   commit. When in doubt, prefer a new commit.
4. **Never use `Co-Authored-By:` trailers.** Even if the harness
   suggests one. Remove it from the message before committing.
5. **Never stage or commit these paths**, regardless of what `git status`
   shows:
   - `.DS_Store`, `Thumbs.db`, editor swap files
   - `certs/`, `keys/`, `data/`, `.env*`, `*.pem`, `*.key`, anything
     under `secrets/` — if you see these staged, abort and tell the user
   - Build artefacts (`build/`, `dist/`, `target/`, `node_modules/`,
     `__pycache__/`, `.venv/`) unless the repo explicitly tracks them
6. **No interactive git flags.** `-i` / `--interactive` and
   `git rebase -i` require a TTY the harness does not have. If the task
   needs one, stop and hand it back to the user.

## 1. Build a todo list before committing

**Always** create a todo list at the start of any commit workflow. At a
minimum it must cover:

- [ ] Verify working-tree state (`git status`, `git diff`).
- [ ] Identify which files belong in the commit and which are stray.
- [ ] Confirm no forbidden path from §0 is staged.
- [ ] Draft the commit message (subject + body).
- [ ] Run the changelog step (§4).
- [ ] Run `git commit -S` (or whatever the project signs with).
- [ ] Verify signature with `git log --show-signature -1`.
- [ ] Decide on push/tag (only if the user asked).

If the workflow spans multiple commits, add a checklist entry per
commit. Mark items completed as you finish them — do not batch.

## 2. Pre-flight checks

Before drafting anything, run in parallel:

- `git status` — see tracked changes and untracked files. Never use
  `-uall` on large repos.
- `git diff` (unstaged) and `git diff --staged` (already staged).
- `git log --oneline -5` — match the repo's commit-message style.
- `git branch --show-current` and `git log --show-signature -1` —
  confirm branch and signature state.

If there's nothing to commit, **do not create an empty commit.** Tell
the user.

## 3. Commit message format

### 3.1 Subject line

```
<type>(<scope>): <imperative summary>
```

- `<type>` — Conventional Commit type:
  `feat`, `fix`, `chore`, `docs`, `refactor`, `test`, `perf`, `style`,
  `build`, `ci`, `revert`. For the very first commit in a fresh repo,
  use `initial: <brief description>` as a single-line message with no
  body.
- `<scope>` — optional, but strongly preferred. The smallest accurate
  subsystem name used by the project (`watcher`, `http2`, `docs`,
  `meson`, `augment/spa`). Mirror the repo's existing scopes when one
  already exists.
- `<imperative summary>` — present-tense imperative ("add", "fix",
  "remove", not "added" / "fixes" / "removing"). No trailing period.
  Keep under **72 characters** total. Lowercase after the colon unless
  a proper noun starts the summary.

### 3.2 Body

- Blank line after the subject, then a short body explaining **why**
  (motivation, constraint, trade-off) more than **what** (the diff
  already shows that).
- Wrap at ~72 columns.
- Reference issues / plans / audits with path or `#<number>` where
  useful. Never invent identifiers.
- Use plain Unicode symbols if you need them. **Never use emoji.** See
  the `unicode-symbols` rule.

### 3.3 Trailers

Allowed trailers only:

- `Refs: #<id>` / `Closes: #<id>`
- `Fixes: <sha>` for bug-fix back-references

**Forbidden trailers:** `Co-Authored-By:`, `Signed-off-by:` (unless the
project actually uses DCO and the user told you to), `Generated-by:`,
or any attribution added by the harness.

### 3.4 Scoping rules

- One commit = one logical change. If you need the word "and" in the
  subject, split it.
- Group files that move together (feature code + its tests + its
  docstring). Don't mix unrelated fixes.
- Mechanical renames / formatting passes get their own commit with
  `chore:` or `style:`.

## 4. Changelog step (git-cliff)

The changelog is part of the commit workflow, not an afterthought.

1. Check availability: `command -v git-cliff`.
2. If present: run the project's changelog recipe (typical:
   `git-cliff --output CHANGELOG.md` or `just changelog` if the repo
   wraps it). Stage `CHANGELOG.md` together with the feature change so
   history and notes land in the same commit.
3. If **not present**: stop and ask the user — "`git-cliff` is not
   installed. Proceed without updating the changelog?" Do not install
   it without consent. Do not silently skip.
4. If the user agrees to proceed without git-cliff, add a line to the
   commit body noting the missing changelog entry so it can be
   reconciled later (e.g. `Changelog: skipped (git-cliff not
   available)`).
5. If git-cliff is present but the config is missing (`cliff.toml`),
   treat it as "not present" and ask the same question.

## 5. Commit, verify, report

1. Stage specific files by path. Prefer `git add <paths>` over
   `git add -A` / `git add .` — the latter is how secrets get leaked.
2. Commit via HEREDOC to preserve formatting:
   ```
   git commit -S -m "$(cat <<'EOF'
   <subject>

   <body>
   EOF
   )"
   ```
3. Verify: `git log --show-signature -1`. Confirm the signature is
   present and valid. If it isn't, **stop** and surface the exact
   signer / key error to the user.
4. Report what you did in one sentence: files touched, subject line,
   signature status. Do not push unless the user asked.

## 6. When hooks fail

- A pre-commit hook failure means the commit did **not** happen.
- Fix the underlying issue, re-stage the fix, then create a **new**
  commit. Do **not** rerun with `--amend` to paper over the failure —
  you'd be amending the previous commit, which may destroy work.
- Never bypass with `--no-verify`.

## 7. Amends, rebases, squashes

- Only amend / rebase / squash when the user explicitly asks, and only
  if the target commits have **not** been pushed to a shared branch.
- `git commit --amend` on an already-pushed commit requires a force
  push, which is off-limits by default — hand back to the user.
- For larger history rewrites use the `clean-history` companion skill.

## 8. Tags and releases

- Release tags are their own workflow. Consult the `version-bumping`
  companion skill for the tag format and the sequence (bump file →
  commit → tag → push).
- Never create or push tags that were not explicitly requested.

## Companion rules

- `unicode-symbols` — allowed symbol set for commit subjects and
  bodies (never use emoji).
- `version-bumping` — decision rules for version-bump commits and the
  tag format they must use.
