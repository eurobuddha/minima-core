# BIP39 — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/BIP39.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/BIP39.cpp) · [C++ header](../../../../../../src/org/minima/utils/BIP39.hpp)

## Purpose

This pinned Minima class uses the 2048-word list as a vocabulary for seed phrases. Its seed derivation is SHA3-256 of MiniString data. Its name does not authorize replacing that algorithm with a different mnemonic or password-derivation standard.

## Follow the algorithm

1. getNewWordList independently chooses 24 random indices, uppercases each selected word using the current locale, trims it, and returns the array. It does not compute a mnemonic checksum here.
2. convertWordListToString trims each supplied word and joins them with a single separating space, then trims the final string. Empty elements can leave internal repeated spaces; this is not the same operation as cleaning a phrase.
3. convertWordListToSeed hashes the raw MiniString data of that joined text. convertStringToSeed hashes its supplied text directly without cleaning, validating membership, or changing case.
4. cleanSeedPhrase trims the outer text and tokenizes only on literal spaces. Tokens shorter than three UTF-16 code units fail. Three-unit tokens require an exact word match; longer tokens select the first word starting with that token. Unknown tokens fail, and accepted words are joined and uppercased.
5. WORD_LIST is a final Java array: the array reference cannot be replaced, but its entries can be mutated. Search order, null entries, and locale therefore affect observable behavior.

## How the C++ translation works

The native fixed-length array contains optional strings to model mutable/null entries. Java trim, upper/lower case, and UTF-16 length helpers preserve the text rules. Random generation obtains 48 random bytes, taking 11 low bits of each two-byte pair as an index; the 2048-entry size avoids modulo bias. Seed creation uses MiniString data and the existing SHA3 routine. Optional values expose null failures rather than converting missing values into empty phrases.

## Understanding the tests

The dedicated driver asks both implementations to return the entire dictionary, then cleans every word, its four-letter uppercase prefix, three-letter prefix, and two-letter prefix. The result comparison includes exact errors for short/unknown words. It also varies five locales, whitespace and nulls, arbitrary string-to-seed inputs, word-list joins, and mutation of dictionary entry 0. For randomized output, the fixtures check exactly 24 words and membership in the vocabulary; they intentionally do not require the same random words. These are explicit finite checks, not a randomness proof.

- [Java phrase fixture](../../../../../../support/BIP39Oracle.java)
- [Native phrase fixture](../../../../../../support/bip39.cpp)
- [Every-word and phrase driver](../../../../../../tools/bip39_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The tests cannot prove entropy quality, collision resistance, or every locale/runtime failure. An instructive direct case compares hashing "air zoo" with hashing " AIR ZOO ": convertStringToSeed preserves those differences. Keep cleaning, joining, and hashing as distinct operations in the port.

## Check your understanding

Why can a three-letter token fail while a four-letter prefix succeeds? Why would automatically calling cleanSeedPhrase inside convertStringToSeed change wallet-derived data?

Pinned Java SHA-256: `5a548235b951101f176383fb8f6469420a8888230a17954978d682c7deccd0bb`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
