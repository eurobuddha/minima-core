# Code review

Self-review using the code-review skill; no independent reviewer is claimed.

The change preserves native parameter behavior and adapts existing configuration
load/save and UI start handling. Legacy saved peers are ignored even before the
first new save. Failed start requests retain the visible peer for retry; successful
requests clear it. Port, role, host and data folder continue to persist. Explicit
manual launches still disable P2P for that session as documented; restarting
without re-entering the peer restores normal automatic P2P startup.

No blocking findings in the reviewed diff. Five launcher tests and fresh/reopen
real-window observations pass. The first restricted test invocation could not
bind a loopback listener (EPERM); the same tests pass with loopback permission.
No C++ source changed and no live synchronization claim follows from these tests.
