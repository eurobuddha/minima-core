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
