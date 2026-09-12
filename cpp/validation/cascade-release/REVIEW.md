# Code Review

## Summary

Self-review of the cascade cleanup correction, its worker regression, CMake
registration and updated teaching/release documentation. The production change
is limited to CascadeNode.hpp/.cpp. Existing Cascade, IBD, NIOMessage and
FixedTaskExecutor callers were inspected; their algorithms and ownership types
are unchanged. The defect is reproduced against the unchanged code, and the
regression checks ownership and eventual release in addition to process survival.

## Findings

No unresolved critical, major or minor correctness finding in the scoped change.

The review specifically checked that no live node is detached using a racy
use_count observation; shared/weak owners retain all ancestry; the queue is
thread-local with a stack-owned lifetime and trivially destructible TLS pointer;
custom-deleter reentrancy cannot overwrite pending references; and explicit
copy/move declarations preserve behavior after introducing a destructor. The
regression checks moved-from parents as well as copy/move type traits.

The one-slot ordinary path allocates no cleanup storage. Additional pending
references from reentrant custom deleters can allocate in the overflow vector;
allocation failure terminates through the noexcept destructor. This is an explicit
limit, not a claim of allocation-failure recovery. Cyclic ownership and concurrent
mutation of the same node are also outside this fix. No change to wallet, network
message formats, cascade selection or transaction validity is introduced.

Test review checked 30,000 distinct nodes, 512 KiB worker stacks, actual
FixedTaskExecutor and createCompleteIBD use, weak witnesses for every fixture
node, shared tails, concurrent weak promotions, thread exit and replacement/move
paths. Immutable shared TxPoW payloads in the cheaper fixtures are documented;
the complete-IBD fixture uses real wire copies. Existing Java comparisons cover
the unchanged observable algorithms. Sanitizers are not presented as a race
proof or full-node parity certification.

Documentation separates corrected behavior from file conversion counts, includes
both the old-crash reproduction and current build identities, and preserves the
457-file teaching inventory. Release artifacts and saved wallet files are outside
Git staging. The source publication excludes the unrelated graphify changes.

## Verdict

Approve the scoped implementation. Publication remains conditional on all three
selected build/test runs and packaged executable verification completing
successfully; their actual results are recorded separately in this directory.
This is a self-review, not an independent second reviewer or a full-port audit.
