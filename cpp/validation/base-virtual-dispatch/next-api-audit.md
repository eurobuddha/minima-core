# Further base API gaps confirmed by owning-class inspection

This is a read-only source audit, not a proof that these are the only gaps.
The earlier whole-tree lexical method-name search missed methods whose names
occurred in other classes. These observations compare the owning native header
with its complete pinned Java class. [The compiled source API](pinned-public-api.txt)
was also extracted with javap from the baseline oracle, which compiles
upstream-study/src, to preserve owning-class signatures for the next audit. Do not mark a file verified from a name match.

- MiniNumber.getNumber is present in the pinned Java class and absent from the
  native owning header. Java delegates to getAsBigDecimal; the native decimal
  carrier is already available for reuse.
- MiniData.toString is absent from its native owning header. Java delegates to
  the overridable to0xString. A native formatter with a different name does not
  by itself expose that source API.
- MiniNumber's source copy constructor obtains getAsBigDecimal then checks limits.
  The native implicit copy bypasses both steps. Java's stream reader can create
  out-of-constructor-range values, so this gap can affect ordinary objects as
  well as derived ones. Native internal value copies are also used as arithmetic
  temporaries: inspect those callers before making all C++ copies validating.
- MiniNumber.isValidMinimaValue checks isLessEqual(BILLION) before isMore(ZERO).
  The native helper reverses that order. This matters for overrides and exceptions.
- MiniNumber arithmetic and compareTo use the right operand's getAsBigDecimal;
  the native implementation uses private coefficient/scale fields and lacks
  virtual dispatch on those public methods.
- MiniNumber.getAsLong currently calls getAsBigInteger, while Java reads its
  decimal field directly. Making all getters virtual without auditing these
  internal calls would introduce a new mismatch.
- MiniData equality uses getLength on both objects and getBytes on the right;
  concat uses both objects' getters. Native direct vector operations bypass
  those overrides. Java retains the caller's input byte array; the native vector
  constructor copies it, while getBytes only exposes a borrowed span. Array
  replacement/lifetime and protected mData need an explicit compatible carrier.
- MiniNumber, MiniData and MMREntryNumber final Java constant references still
  permit in-place reads of their objects. Most native constants are const. MMR
  div2/mult2 specifically read current TWO.getBigDecimal; native code hardcodes 2.
- MiniString.MINIMA_CHARSET is mutable in Java but fixed UTF-8 natively. Charset
  conversion and null/custom-provider behavior remain independent work.

Next: reproduce MiniNumber copy and mutable MMR TWO behavior with the pinned
classes, then reuse existing decimal operations while separating Java constructor
semantics from native internal storage copies. Continue the existing full
sanitizer workloads without relaunching their unchanged executables.
