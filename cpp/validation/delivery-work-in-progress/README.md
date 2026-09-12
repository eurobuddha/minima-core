# Delivery validation evidence

Start with the [final validation status](../../delivery/VALIDATION_STATUS.md) and
[machine-readable test disposition](test-disposition.json). They distinguish the
292-test Debug pass, optimized initial failures and successful 33-test retry,
unfinished sanitizer replay, corrected focused checks and live-network limits.
The delivery remains a development checkpoint with full parity unverified.

The records below and earlier files preserve intermediate observations. They are
historical evidence, not a replacement for the final disposition.

## Earlier observation

# Delivery validation in progress

The initial packaged runtime at `41a9c5f` passed relocation, hash-command and fresh
peer height/hash checks, but the reopen check failed its 150-second limit. It
restored height 2309788 and connected while the peer reported 2309789. This is a
recorded failed observation, not yet an identified Java/C++ divergence. The
processor source in both languages defers blocks whose transactions are missing.
The cause is under investigation using only a temporary isolated wallet on 22001.

`initial-runtime-verification.json` preserves that result. No wallet/chain database
is included here. Diagnostic logs remain in the temporary test directory until
public observations can be selected for this evidence directory.

The verifier now compares the most recent valid ping and status, rather than
searching historical responses for a match. The 150-second gate is unchanged.
Full normal and sanitizer build/CTest results will be recorded on completion.
