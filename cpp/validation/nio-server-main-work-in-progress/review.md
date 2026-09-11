## Code Review

### Summary
Reviewed added NIOServer.main, its private logging processor, declaration, CMake
registration, both fixtures and the comparison driver. The caller owns a shared
server and the launched worker independently retains it, so an InterruptedException
cannot destroy a live target. Source waits and effect order are retained. The hello
worker performs actual singleton database cleanup rather than an empty substitute.

### Findings
No blocking finding for this scoped addition. The fixture serializes its fixed-port
demo across builds and refuses an occupied preflight port. It only halts child
processes it launched. The original bind-failure behavior remains Runtime.halt's
native _Exit path; normal main reaches std::exit and existing shutdown hooks.
The caller's interruption is consumed, and listener state after the thrown call is
observed without connecting. Full node/thread lifecycle and every allocation or
thread-start failure remain unverified. Existing detached-thread creation behavior
is reused; this change does not certify all native/JVM thread API equivalence.

### Verdict
Approve the bounded change. Both actual executable builds and all three selected
NIO tests pass. No claim of complete file or 457-file parity is made.
