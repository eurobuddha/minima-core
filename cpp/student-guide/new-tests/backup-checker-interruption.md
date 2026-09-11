# Interruption in validation and backup work

This lab extends existing experiments; final scoped results are recorded below.
Seven remaining mapped sleep defaults now use NativeThread. Vault ignores its
key-stop interruption after setting STOP_NEWKEYS. Archive ignores interruption
between request retries, then continues with its source attempt counter. Other
archive/resync/MySQL/MegaMMR polling waits retain propagation to their callers.

TimedChecker starts its actual TxPoWChecker worker and polls in 50 ms steps.
It now retains the NativeThread descriptor and interrupts the current worker
when polling finishes. If the polling caller is interrupted first, its catch
logs and clears valid; the later worker-interrupt statement is skipped. Worker
inputs remain owned until the worker exits; interruption is not forced termination.

The existing timed-checker error fixture holds a real worker at a logger callback.
A reused checker already has its finished flag set, so it can return while the
second worker is held, without shortening the source's 120-second deadline.
The worker then records whether it was interrupted. A second case interrupts the
polling caller and checks that the worker receives no signal. The logger fixture
must release its own monitor before waiting, allowing caller error logs to proceed.
Only concurrent log ordering in this new interrupted-caller case is canonicalized.

The existing runtime-sleep fixture gains vault waiting/already-ready paths,
archive request retry/no-retry paths with a null start number, and positive/zero/
negative MySQL setup delays. The archive cases fail during actual request encoding,
before a socket is opened; they test retry control flow, not successful networking.
MySQL uses the existing recording driver and synthetic credentials. Positive delay
consumes interruption before connection; zero skips sleep; negative rejects the
delay without clearing interruption. Preferences are assigned before the delay.
No preview, wallet database or mainnet data is touched by these added cases.

Raw-sleep inventory removal is not complete parity. Direct interruption during
restore/MegaMMR state changes, I/O, executor waits and general Java thread lifecycle
remain separate work. Existing long deadline and full archive tests remain intact.

## Results and evidence

Both normal and address/undefined sanitizer builds pass the actual node build,
26 runtime comparisons with 326 assertions, and 106 checker cases. This adds seven
runtime cases and one checker case, and strengthens the existing held-worker case
with an observed interrupt flag. Original cases were retained. Seven distinct focused
CTests pass per configuration: runtime sleeps, checker errors, checker checks,
archive commands, MySQL commands, restore-sync and MegaMMR commands. The storage
regression group took 224.90 seconds normally and 270.85 seconds under sanitizers.
The original 120-second deadline tests remain unchanged and were not rerun here.

The first runtime comparison found a fixture setup difference: native NotifyManager
reads the default webhook preference, while constructor-free Java setup did not.
Calling the actual Java getter aligned starting state; the full preference output
remains compared, with explicit expected MySQL assignments. Both initial failed runs
are retained alongside final [evidence](../../validation/backup-checker-interruption-work-in-progress/progress.json).

The raw-sleep scan now has zero matches under cpp/src/org. This is an inventory
observation, not a parity certificate. The [next-work inventory](../../validation/backup-checker-interruption-work-in-progress/remaining-thread-work.json)
records executor waiting, TimerProcessor's separate condition-variable behavior,
a missing standalone NIOServer demonstration entry point, and the absent JNI file.
Students should distinguish a missing implementation from a missing test and from
a correct implementation with only limited evidence.
