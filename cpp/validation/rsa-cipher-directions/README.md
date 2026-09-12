# RSA factory direction correction

The original utility corpus did not exercise the reverse RSA directions exposed
by GenerateKey's cipher factory. Added direct factory operations retain the
existing utility suite and expose private encryption/public recovery failures.

- before-results.json: unchanged native production code, 1,492 cases, 80 mismatch records.
- *-empty-recovery-failure.json: initial dispatch correction, twelve empty recovery failures per build.
- normal-results.json, optimized-results.json, sanitized-results.json: 1,492/1,492 pass each, twenty independent reverse plaintext recovery checks each.
- source-hashes.json: exact changed implementation, fixtures and driver hashes.
- Build and run logs retain compiler deprecation warnings and actual outcomes.

This is a correction in an existing counterpart; inventory remains 456/457.
The Java oracle compiles the pinned GenerateKey source. No JNI implementation,
whole Java Cipher API, exact provider error prose or whole-node parity is claimed.
The installed preview remains its previous validated build; this correction is
in source and the rebuilt development targets, not silently installed over it.
