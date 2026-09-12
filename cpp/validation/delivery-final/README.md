# Final delivery verification

The [archive verification](archive-verification.json) identifies the final source
checkpoint, both ZIP hashes, all 5,742 extracted payload checks, exported source
and 457-lesson audit, and byte identity with the exercised optimized runtime.
The [export check log](export-checks.log) records the actual extracted checks.

The [source checkpoint recovery](source-checkpoint-recovery.json) and
[working recovery log](working-recovery.log) record an offline restoration of the
exact shipped source commit and pinned Java tree, including all lesson links.
The bundle remains local; no off-machine backup is claimed.

These post-export receipts are committed after the shipped source checkpoint.
They do not change the shipped source or silently replace its validation limits.
See the [delivery receipt](../../delivery/DELIVERY_RECEIPT.md).
