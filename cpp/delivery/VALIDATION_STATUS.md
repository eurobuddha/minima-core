# Delivery validation status — candidate in final checks

The final handover must replace active statuses with terminal results or explicit
uncompleted tests. The source and binary revisions of separate runs are retained.

- Latest normal native node build passed after the database main additions.
- Normal affected regressions passed all five suites: utility, Cascade, archive,
  on-chain DB and TxPoW SQL DB. Evidence is under delivery-work-in-progress.
- Runtime mains:18 comparisons/121 assertions per normal and sanitizer build.
- MMR mains/printers:30 comparisons/112 assertions per build.
- Crypto mains:24 runs/24 cross-decryptions/222 assertions per build.
- Database mains:18 runs/36 cross-reads/465 assertions per build.
- Full earlier normal run:291/292 passed; Magic still active.
- Full earlier sanitizer run:active. Original HORST five-second cleanup failure is
  retained; corrected targeted full-size rerun passed1,438 comparisons and clean exit.
- Current registration:297 CTests, including new snapshot-integrity checks. Optional
  live MySQL fixture tests are not configured. The AOT H2 library is not instrumented.
- Snapshot checker:ten checks pass, including altered/missing source/map rejection
  and the required failure of full-parity acceptance. Source mapping remains456/457.
- Student guides:457 lessons;108 build-directory links repaired to portable saved
  evidence. Exported guide audit is now a packaging gate. Actual package check pending.
- Final source archive and final relocated fresh/reopen runtime check:pending.

HTML presentation:ten slides and ten presenter-note sections generated from the
editable outline. Browser security policy rejected opening the local file; no
workaround was attempted. Visual inspection remains unverified. The prescribed
presentation artifact runtime and load_workspace_dependencies tool were unavailable.
The provided formats are editable HTML and Markdown; no PowerPoint or visual QA
pass is claimed.

A successful finite test suite is not full Java/C++ parity. See KNOWN_GAPS.md.
