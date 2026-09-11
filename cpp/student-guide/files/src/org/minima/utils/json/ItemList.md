# ItemList — teaching guide

**Teaching status:** Authored explanation of the Java source, native translation, and executed comparison. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/ItemList.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/ItemList.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/ItemList.hpp)

## Purpose

ItemList is a string splitting/joining helper, separate from JSON parsing. It supports exact-substring separators and a StringTokenizer mode in which each separator character is a delimiter.

## Follow the algorithm

1. The default separator is comma. The one-string constructor splits with it. The two-string constructor unexpectedly assigns this.sp=s rather than the separator argument, although the initial split still uses the argument; later joining/addAll therefore uses the original input as separator.
2. Exact splitting finds each full separator, trims each segment, preserves empty segments, and finally appends the tail. Null input/separator does nothing. An empty separator never advances the loop, so it can run indefinitely.
3. Multi-token splitting uses StringTokenizer: separator characters act independently, adjacent delimiters do not produce empty tokens, and each returned token is trimmed.
4. add ignores null and trims nonnull strings; getItems returns the mutable raw list. Joining inserts the stored separator between elements. clear retains the separator, whereas reset restores comma.
5. getArray casts the Object[] produced by no-argument List.toArray to String[], which normally throws ClassCastException rather than returning a valid string array.

## How the C++ translation works

The native class reuses JavaList storage and the MessageObject shared-object adapter. It adds indexed insertion to that storage and a UTF-16 overload of the existing Java trim helper. String items retain their code units rather than passing through UTF-8; nonstring values retain their native type and shared object identity. get casts strings and returns null for a null item, while a nonstring raw item fails with ClassCastException.

Exact split searches UTF-16 substrings and preserves empty fields. Multi-token split recognizes Unicode code points, matching the inspected JDK StringTokenizer surrogate behavior. The constructor assignment and getArray failure are preserved. addAll takes a snapshot before appending, so adding a list to itself duplicates the original contents once. getItems returns the same shared mutable list; a retained alias can still mutate it after a caller replaces its ItemList variable.

Joining checks current size for each iteration and calls each object's toString. A local copy retains the item while its callback runs, so an object that appends to the same list cannot invalidate the currently rendering C++ value. New elements appended during rendering are visited, matching the source loop.

## Understanding the tests

The [student lab](../../../../../../new-tests/item-list.md) was written before testing. The [driver](../../../../../../../tools/item_list_differential.py) reuses the existing JSON oracle/Runner pattern and compares all public constructors and overloads against the pinned Java class. It records every string as UTF-16 hex, preserving isolated-surrogate differences that ordinary console output could hide.

Cases cover exact separators versus delimiter sets, nulls, ASCII trim versus nonbreaking space, empty tokens, indexes, separator changes, raw values, mutable aliases, clear/reset, self-add, and a retained object whose rendering changes or throws. A mutating object appends a new string during join; the result must include it. getArray must throw even for an empty list.

For an empty exact separator, a supplied test list throws after four successful adds. Both runtimes must have appended four empty strings before that exception. This safely demonstrates the non-advancing loop without changing the production loop or relying on a timeout.

Normal and ASAN/UBSAN comparisons each pass 1,493 programs and 11,690 operations. Both full builds and five affected checks pass per build; the lab links to saved evidence.

## Limits

The map remains partial. Tests cover the class's declared API and specific raw-list/object behavior, not every inherited Java List method, subclass-constructor dispatch, arbitrary Object.toString implementation, concurrency or resource-exhaustion behavior. An empty separator with an ordinary unbounded append list remains the source's non-advancing loop. Retaining that behavior is fidelity evidence, not a recommendation for new callers.

## Check your understanding

Predict the tokens and join result for new ItemList("a:b", ":"). Why does the stored separator matter after the initial split?

Pinned Java SHA-256: `d22a52ffe4a7813871d14af71baf31d639d9c70badfb0efb045fbcb0c0aa4bac`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
