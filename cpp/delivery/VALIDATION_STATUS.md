# Historical delivery validation status

Current application and cascade-fix evidence is summarized in
[CURRENT_STATUS.md](CURRENT_STATUS.md). The broad suite results below belong to the
earlier delivery; they are not a full replay of the currently installed code.

This is a tested development handover, not a complete Java/C++ parity certificate.
The shipped candidate uses the optimized C++ build with `-O2 -g`, assertions enabled,
and the recorded native H2 provider. Its C++ source tree is unchanged from e82e91f.
The delivery manifest identifies the final documentation/source checkpoint and
runtime hashes. [Machine-readable test disposition](../validation/delivery-work-in-progress/test-disposition.json)
retains individual test names, results, timings and source identity.

## Broad builds and tests

| Run | Result | Evidence and limits |
| --- | --- | --- |
| Earlier full Debug build/test set | 292 / 292 passed; 6,528.51 seconds | [Log](../validation/delivery-work-in-progress/normal-full-292.log), [JUnit](../validation/delivery-work-in-progress/normal-full-292.xml). Earlier binaries; final affected changes were checked separately. |
| Optimized full build | Passed | [Build log](../validation/delivery-work-in-progress/optimized-full-build.log). Release configuration explicitly retains assertions; no fast-math. |
| Optimized initial full test run | 264 passed, 33 failed; 1,471 seconds | [Log](../validation/delivery-work-in-progress/optimized-initial-full-297.log), [JUnit](../validation/delivery-work-in-progress/optimized-initial-full-297.xml). Restricted socket access affected fixtures. This run was not a clean pass. |
| Optimized retry of all 33 failures | 33 / 33 passed; 245 seconds | [Log](../validation/delivery-work-in-progress/optimized-socket-retry.log), [JUnit](../validation/delivery-work-in-progress/optimized-socket-retry.xml). Required socket access enabled. [Definition check](../validation/delivery-work-in-progress/optimized-socket-retry-definition.json) verifies identical resolved commands, environment, working directories and timeouts. |
| Earlier full sanitizer replay | 289 passed, 1 failed, 3 unfinished | [Partial log](../validation/delivery-work-in-progress/sanitized-interrupted-293.log), [stop record](../validation/delivery-work-in-progress/sanitized-stop-disposition.json). Stopped for the bounded handover; no complete sanitizer-suite pass. |

Every one of the 297 optimized registered tests has a passing result across the
initial run and the exact-command retry. The original 33 failures remain available.
The completed Debug and optimized Magic workloads retain the complete source loop.

The unfinished sanitizer tests are `sphincs_differential`,
`sphincs_command_differential` and `magic_differential`. The original sanitizer
failure was HORST teardown's five-second limit. Its corrected, separately recorded
full-size rerun passed all 1,438 comparisons, 1,299 independent assertions and clean
exit with a bounded sixty-second teardown allowance. No cryptographic work was
removed. All owned test processes were stopped; user nodes were not targeted.

## Final affected checks

The final normal native node build and five affected regression suites passed.
The final sanitizer node build and seven affected/source suites passed in 168.07
seconds. Each added main-method batch also passed in both normal and ASAN/UBSAN
builds; [the change report](CHANGES.md) links its logic, tests and explicit limits.
Those focused passes do not replace the incomplete broad sanitizer replay.

The 297-test inventory includes the new main-method suites and ten positive/negative
source-snapshot checks. Optional live MySQL fixture tests were not configured.
Sanitizers instrument C++ code, not the AOT H2/MySQL dependency.

## Source, student material and packaging

The inventory check reports 456 counterparts for 457 Java files. Full parity remains
`not_verified`, and `check_full_port` intentionally fails. All 457 authored lessons
pass the structural audit: 9,650 links and 2,628 source-line anchors. That audit does
not establish exhaustive teaching review or behavioral equivalence.

The e82e91f exported project passed all 5,705 payload hashes and its exported lesson
audit. Its extracted source built the actual node and passed source mapping,
snapshot-integrity and Java/native primitive checks. The final source uses the same
C++ implementation tree; final package verification separately checks its hashes,
exported source map, lessons and identity against the tested runtime. Each delivery
ZIP has a checksum and a verification record beside it.

Local recovery bundles have been verified by restoring the expected commit and
pinned Java source without network access, checking Git connectivity and comparing
checkpoint file bytes. See [recovery instructions](RECOVERY.md). These are local
backups. The source was subsequently published to the [GitHub repository](GITHUB.md);
that publication does not change the recorded test results or include private wallets.

## Live runtime observations

The e82e91f Debug package passed relocation, command hash, fresh peer agreement at
2309856, saved databases and clean exit. Immediate reopen missed the 150-second tip
target. A subsequent unchanged-package restart caught up and followed new blocks
through 2309869. That success does not erase the failure. The pinned Java live
control was inconclusive: it did not establish a fresh chain within the bound.

The optimized e540881 runtime passed fresh peer agreement at 2309904. Immediate
reopen exposed a premature verifier connection assertion while the saved tip
already matched. The verifier now waits for connection within the same 150-second
limit. A corrected repeat remained at zero established peers and height zero during
fresh startup, then exited cleanly; it never ran reopen. Its log recorded a
successful connection attempt, which is insufficient to establish a ready node.
Reliable mainnet startup/reopen remain unverified. See the [retained observations
and next action](../validation/delivery-work-in-progress/live-reopen-limit.md).

## Presentation

The editable HTML presentation has ten slides and ten speaker-note sections, with
an accompanying Markdown outline and demo script. Its structure and local links
were checked. Browser security rejected opening the local file; visual inspection
remains unverified. The prescribed presentation artifact runtime was unavailable.
No PowerPoint file or visual QA pass is claimed.
