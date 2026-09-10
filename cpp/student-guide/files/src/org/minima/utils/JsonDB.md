# JsonDB — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/JsonDB.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/JsonDB.cpp) · [C++ header](../../../../../../src/org/minima/utils/JsonDB.hpp)

## Node-state command comparison

getNumber first checks whether the stored value is null and returns the supplied default if so. For a non-null value, Java casts to String before constructing MiniNumber. A Boolean stored under magic_kissvm exposed a native bad_variant_access instead of ClassCastException. The counterpart now uses JSONValue.isString and the existing JavaClassCastException before the existing decimal constructor. This preserves the source’s null/default branch. See [the lab](../../../../../new-tests/node-state-commands.md) for the failing examples and final comparison evidence.

## Main runtime comparison

The [Main lab](../../../../../new-tests/main-runtime.md) adds typed exception observations through actual UserDB preferences. A wrong Boolean backup flag and wrong string credentials exposed native bad_variant_access where Java throws ClassCastException. getBoolean and both getString overloads now check types explicitly, preserving absent/null defaults. The complete Main corpus passes 336 programs and 897 operations in both builds. Other JSON/data casts and logger behavior still need their own complete error review.

## Purpose

JsonDB is a JSON-backed preferences container with stream/file persistence. Its name does not mean it is an SQL engine. Several getters intentionally change or interpret the stored representation, so treating it as an ordinary map of interchangeable values would lose behavior.

## Follow the algorithm

1. Construction creates a JSONObject. getAllData returns that object itself. exists checks whether get(name) is non-null, so a present key containing JSON null is treated like absence.
2. Boolean and string getters return their defaults for absent/null values and otherwise cast to the expected type. Number and data setters store strings; their getters reconstruct MiniNumber or MiniData from those strings. Wrong types are not silently converted.
3. Object/array setters retain the supplied reference. getJSON returns the stored object or supplied fallback. getJSONArray is different: when absent/null, it inserts a new array into the database before returning it.
4. writeDataStream serializes the entire JSON object as a MiniString. readDataStream decodes that string, parses it, and assigns a JSONObject result. A ParseException is logged and replaces data with an empty object; a wrong top-level type can fail at the cast instead. Parsing JSON null can assign null to mParams.
5. loadDB and saveDB delegate to MiniFile slow-load/direct-save operations, inheriting their IO and failure behavior.

## How the C++ translation works

A shared_ptr<JSONObject> preserves the exposed object reference and nullable state. Typed JSONValue accessors replace Java casts. Numbers remain decimal strings and binary data remains hex text. Array getters return shared containers so later mutations are visible through every alias. The stream reader catches only the native ParseException; its logger currently writes to std::clog, so Java MinimaLogger failure behavior is not reproduced.

## Understanding the tests

The preferences driver initializes UserDB, which inherits JsonDB, then exercises exists, typed getters/defaults, array creation, and wire output against starting texts including {}, null, [], primitives, malformed JSON, and varied x field types. The explicit alias operation stores the same array under a and b, mutates it through both references, and compares the final JSON. The driver compares operation results and final database state, catching a port that returns plausible defaults while modifying the wrong object.

- [Java preferences fixture](../../../../../../support/PrefsOracle.java)
- [Native preferences fixture](../../../../../../support/prefs.cpp)
- [Preferences differential driver](../../../../../../tools/prefs_differential.py)

The descriptions above come from reading the cited source and test evidence. That original authoring pass did not run tests. The later Main comparison above records its own executed evidence.

## Limits and a useful next test

The fixture reduces many exceptions to ERROR and does not establish full file-failure, logger-failure, or stream-overload parity. A direct follow-up should start with a known value, supply malformed JSON versus a valid nonobject JSON value, and compare whether the original state survives, becomes empty, or becomes null.

## Check your understanding

Why can exists("x") be false when the JSON text contains "x":null? How does merely calling getJSONArray("x") change the next serialized database?

Pinned Java SHA-256: `cd1b32a4f7c496fd18fec4905136c0a70b57454b1dacff1a7a37134971405300`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).

## RPC-array exception correction

The RPC command comparison supplies a boolean where a saved JSONArray is expected.
Java throws ClassCastException. The native getter previously allowed std::get to
throw bad_variant_access, which the command boundary labelled RuntimeException.
getJSONArray now checks the variant and throws the explicit Java category, while
retaining missing/null default creation and shared-array identity. The older
preference fixture collapsed both failures to ERROR; the new RPC test retains
exception categories and shows why more precise observations matter.


## Root ownership during concurrent reads

JsonDB now locks the root shared_ptr when reading or replacing it. Each params()
call returns an owner, so readDataStream can install a new root while an existing
reader finishes using the old root. Copying JsonDB retains the same root, as its
previous native shared_ptr copy did. Container getters return owned values, and
getJSONArray still returns the stored array itself. The separate exists/create/get
steps remain separate source operations; they are not a get-or-create transaction.
The [shared JSON lab](../../../../../new-tests/shared-json-rpc.md) explains how to
test replacements, serialization and retained objects together.
