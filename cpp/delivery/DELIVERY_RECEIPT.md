# Delivery receipt — 12 September 2026

The bounded development handover is assembled and verified. The original full
457-file behavioral port remains incomplete: 456 counterparts, one missing JNI
implementation, and full parity unverified. Mainnet startup/reopen reliability
remains an open issue. This receipt does not change those acceptance limits.

## Artifacts

Under `cpp/deliverables/` in the original working project:

- `MinimaCoreCpp-team-20260912.zip`: complete source snapshot, pinned Java, runtime,
  validation evidence, 457 lessons and HTML/Markdown presentation materials.
- `MinimaCoreCpp-team-20260912-runtime.zip`: standalone Apple Silicon runtime.
- `MinimaCoreCpp-team-20260912/runtime/MinimaCoreC++-macos-arm64/Start MinimaCoreC++.command`:
  runnable launcher; default port 21001, separate data directory, RPC disabled.

The source checkpoint inside the delivery is
`aea5d892a623271155dd7f2a2af1d06bdecab9b1`. This receipt is an administrative
post-export commit; it does not relabel the exported source as the later commit.

Full project ZIP SHA-256:
`cba1450324a72191fec2d9e740392693e320718a95b5e672ce92c9d0637bf8f4`

Standalone runtime ZIP SHA-256:
`909ce54ab10ba60b2a11ea25ce90d1ed15f29599c2f23573dd635c821540c69d`

## Verification

All 5,742 final extracted payloads match the manifest. The exported source map
passes. All 457 lessons pass their structural audit: 9,650 links and 2,628 line
anchors. The launcher runs after relocation with a system-only PATH and creates
no data for `-help`. The executable, five native libraries and launcher byte-match
the optimized package used for live workflow checks. Native source/CMake/support
are unchanged from the earlier independently built exported source.

The optimized 297-test inventory has passing results across the original run and
33-test socket-access retry; the original failures are retained. The earlier Debug
run passed 292/292. The full sanitizer replay ended with three unfinished tests;
focused sanitizer checks passed separately. See [validation status](VALIDATION_STATUS.md)
and [final archive/recovery verification](../validation/delivery-final/README.md).

The exact shipped source bundle was independently restored without network access,
including its pinned Java worktree, and all checkpoint bytes and lessons verified.
A subsequent local recovery bundle also protects this receipt commit. Bundles and
the generated delivery artifacts remain local. No push or team message was sent.

The existing frozen port-21001 preview and its wallet were left untouched. All
owned test processes exited. Presentation structure was checked; visual inspection
remains unverified because the browser rejected access to the local file.
