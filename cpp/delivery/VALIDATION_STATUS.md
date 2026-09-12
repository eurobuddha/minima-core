# Delivery validation status — candidate in final checks

The final handover must replace active statuses with terminal results or explicit
uncompleted tests. The source and binary revisions of separate runs are retained.

- Both final normal and sanitizer native node builds passed after the database main additions.
- Normal affected regressions passed all five suites: utility, Cascade, archive,
  on-chain DB and TxPoW SQL DB. Evidence is under delivery-work-in-progress.
- Runtime mains:18 comparisons/121 assertions per normal and sanitizer build.
- MMR mains/printers:30 comparisons/112 assertions per build.
- Crypto mains:24 runs/24 cross-decryptions/222 assertions per build.
- Database mains:18 runs/36 cross-reads/465 assertions per build.
- Full earlier normal run:all292 tests passed, including the complete Magic workload;
  elapsed6528.51seconds. This earlier binary set is distinct from the final affected
  regression and optimized candidate. See normal-full-292.log/xml.
- Full earlier sanitizer run:active. Original HORST five-second cleanup failure is
  retained; corrected targeted full-size rerun passed1,438 comparisons and clean exit.
- Current registration:297 CTests, including new snapshot-integrity checks. Optional
  live MySQL fixture tests are not configured. The AOT H2 library is not instrumented.
- Snapshot checker:ten checks pass, including altered/missing source/map rejection
  and the required failure of full-parity acceptance. Source mapping remains456/457.
- Student guides:457 lessons;108 build-directory links repaired to portable saved
  evidence. Exported guide audit is now a packaging gate. Exported candidate package checks passed as recorded below.
- Candidate e82e91f full ZIP:5,705 payload hashes verified;457 exported lessons,
  9,640 links and2,628 line anchors pass. Its relocated source builds the actual node
  and passes source mapping, snapshot-integrity and Java/native primitive tests.
- Sanitizer final affected regression:all seven selected suites pass (five affected
  class suites plus both source-map checks),168.07seconds.
- Candidate e82e91f relocated runtime:fresh start/hash command/peer agreement at
  2309856/save/clean exit pass; immediate reopen fails its150-second tip target.
  Subsequent same-package diagnostic restart follows new blocks through2309869.
  Cause remains unconfirmed; the initial failure is not replaced by a pass.
- Offline bundle recovery:working checkout and pinned Java worktree restored,
  source map and all457 lessons verified without a network connection.
- Final handover archive will be regenerated after terminal broad-test disposition;
  candidate checks above retain their exact source identity.

HTML presentation:ten slides and ten presenter-note sections generated from the
editable outline. Browser security policy rejected opening the local file; no
workaround was attempted. Visual inspection remains unverified. The prescribed
presentation artifact runtime and load_workspace_dependencies tool were unavailable.
The provided formats are editable HTML and Markdown; no PowerPoint or visual QA
pass is claimed.

A successful finite test suite is not full Java/C++ parity. See KNOWN_GAPS.md.

Optimized `-O2 -g` build completed with assertions retained. Its full297-test run
is still active; socket fixtures in the initial sandboxed run encounter permission
denials. Preserve that run and rerun its failed tests with required socket access.
The optimized runtime initially passed fresh startup at2309904, exposed a premature
connection assertion on reopen, and a corrected repeat failed fresh connection
within150seconds. No complete successful fresh/reopen pair is claimed.
