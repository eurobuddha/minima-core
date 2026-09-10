# MinimaCoreC++ checkpoints

The local project name and working branch are MinimaCoreC++. The full 457-file
native port remains the goal; a commit is a recovery point, not a parity claim.

- Commit each coherent implementation with its tests, lesson, source map and
  validation evidence after the relevant checks pass.
- Before an interruption, save unfinished work in a clearly labelled WIP commit
  with remaining steps in CONTINUATION.md. Keep verified checkpoints separate.
- Preserve the existing Java history. Do not force-push, rewrite published
  checkpoints or move the pinned source without a deliberate source review.
- Keep builds, temporary databases, local machine files and graph caches ignored.
  Store compact passing reports under validation when making a verified checkpoint.
- Use GRAPHIFY_SKIP_HOOK=1 for checkpoint commits. This is the installed hook's
  documented opt-out; it prevents an incidental background rebuild from changing
  study files while they are being saved. The hooks themselves remain installed.
- Record upstream-study through .gitmodules and its pinned gitlink. Existing
  linked-worktree storage remains intact. A fresh online clone can initialize it
  with git submodule update --init upstream-study.
- Create timestamped bundles under local-backups (ignored), including all refs
  and the pinned upstream tag. Verify each bundle and clone it into a temporary
  directory to check recovery. A bundle on this disk is an extra recovery copy,
  not an off-machine backup.

Verified native baseline: e2c0d06, 357 mapped counterparts, all 457 lessons,
both full builds passing. The 356 phase passed 35 selected tests per build; the
357 phase passed ten selected tests per build. Finite coverage and full-node
limitations are recorded in REVIEW.md and validation/checkpoint-357.

GitHub backup destination is awaiting the user's choice between their two
existing fork remotes. Local commits do not imply that a GitHub push occurred.
