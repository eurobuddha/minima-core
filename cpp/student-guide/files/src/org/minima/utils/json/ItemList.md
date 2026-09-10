# ItemList — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/ItemList.java) · `src/org/minima/utils/json/ItemList.cpp` (absent)

## Purpose

ItemList is a string splitting/joining helper, separate from JSON parsing. It supports exact-substring separators and a StringTokenizer mode in which each separator character is a delimiter.

## Follow the algorithm

1. The default separator is comma. The one-string constructor splits with it. The two-string constructor unexpectedly assigns this.sp=s rather than the separator argument, although the initial split still uses the argument; later joining/addAll therefore uses the original input as separator.
2. Exact splitting finds each full separator, trims each segment, preserves empty segments, and finally appends the tail. Null input/separator does nothing. An empty separator never advances the loop, so it can run indefinitely.
3. Multi-token splitting uses StringTokenizer: separator characters act independently, adjacent delimiters do not produce empty tokens, and each returned token is trimmed.
4. add ignores null and trims nonnull strings; getItems returns the mutable raw list. Joining inserts the stored separator between elements. clear retains the separator, whereas reset restores comma.
5. getArray casts the Object[] produced by no-argument List.toArray to String[], which normally throws ClassCastException rather than returning a valid string array.

## Translation work still required

The mapped C++ counterpart is absent. A future port must preserve or explicitly resolve these source quirks; ordinary std::split-like behavior cannot be substituted without changing constructor state, empty-token behavior, null handling, and getArray failure.

## Understanding the tests

No native ItemList test is claimed. Proposed cases should compare split("a::b:", ":") with multi-token mode, inspect toString after the two-argument constructor, mutate getItems, and assert the getArray failure. The empty-separator case needs an isolated timeout test to observe nontermination safely.


This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

These are future tests, not executed validation. The helper’s name and initial comments are insufficient to infer behavior; the constructor assignment and raw-array cast are particularly instructive counterexamples.

## Check your understanding

Predict the tokens and join result for new ItemList("a:b", ":"). Why does the stored separator matter after the initial split?

Pinned Java SHA-256: `d22a52ffe4a7813871d14af71baf31d639d9c70badfb0efb045fbcb0c0aa4bac`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
