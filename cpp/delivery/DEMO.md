# Fifteen-minute team demonstration

## Preparation

Use a fresh extracted runtime folder. Confirm that the chosen node port is free.
Keep the source commit and final test report open beside the console. Allow wallet
creation and initial synchronization to finish before a short scheduled demonstration.
Do not use the owner's established wallet or stop an unrelated node.

## Console demonstration, five minutes

1. Launch the packaged executable and explain its separate data directory.
2. Run `help` to show the supported command surface.
3. Run `hash data:hello`. Expected SHA3 result:
   `0x3338BE694F50C5F338814986CDF0686453A888B84F424D792AF4B9202398F392`.
4. Run `status` and `network`. Explain the version, height, peer count and port.
5. Run `ping host:eurobuddha.com:9001`, then `status`. Compare the reported tip
   height and hash. If they differ, explain the actual sync state rather than
   claiming tip agreement from an old height.
6. Run `quit`, wait for clean shutdown, reopen and show that the chain persists.

## Source and test demonstration, five minutes

Open the Address student lesson and compare Java main with C++ main. Explain the
Base32 substring ranges and the small-number exception. Show one fixed-byte
Java/native comparison and one real-random run whose output is internally checked.
Then show the Contract lesson: tracing goes through MinimaLogger and retains the
source notification behavior. Point out exactly what each test covers.

## Engineering discussion, five minutes

Open the final test inventory and known gaps. Distinguish counterpart count, tested
behavior and whole-file parity. Show a local checkpoint/recovery manifest and the
shareable ZIP checksum. Agree which remaining gaps prevent the team's intended
use and assign those concrete source/file pairs for follow-up.
