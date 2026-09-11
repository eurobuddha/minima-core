# Reading and testing the MMR demonstrations

MMR.main adds the same leaf data object ten times, then prints the sum tree.
MegaMMR.main creates two batches of ten coins and prints six stages around scans
and pruning. Native code calls the existing MMR and coin methods; it does not
replace the tree with a drawing or a precomputed string.

## Why compare invisible output?

The Java printer allocates 2,048 characters for each row but initializes only the
first 512 to spaces. The rest are NUL characters. A terminal can hide that
mistake, while a file or downstream parser can observe it. The new test compares
the entire output string, preserving every NUL and newline.

A node's horizontal position comes from its row and entry index. Java casts a
large power of two to a saturated int and then wraps int multiplication. C++
uses unsigned arithmetic for those wrapping steps and checks the final copy
range before writing. That preserves the source failure without native undefined
behavior. Tests include the exact right edge, beyond the edge, a negative index,
a fractional decimal index, large indexes that wrap, and high rows.

## What is exercised

The [Java wrapper](../../support/MMRMainsOracle.java) and
[native wrapper](../../support/mmr_mains.cpp) call the real mains and printers.
The [driver](../../tools/mmr_mains_differential.py) compares 30 scenarios and makes
112 assertions per build. Both normal and sanitizer runs passed in the
[recorded evidence](../../validation/mmr-mains-work-in-progress/README.md).

For successful direct printer calls, serialization before and after must match.
Printing should not modify the stored tree. MegaMMR runs with pruning on and off.
Its random addresses and coin IDs are still generated, even when the source
immediately overwrites an address. These random identities are not printed, so
output equality does not prove a random generator's distribution.

## Boundaries of the result

The null-tree cases compare exception class and partial output. Java's helpful
null message and native diagnostic prose are recorded separately. This test does
not establish full MMR API, concurrency, lifetime or random-provider parity.

## Exercise

Why would trimming whitespace or removing NULs conceal a porting error? Why can a
32-bit overflow be well-defined in Java but require explicit wrapping in C++?
