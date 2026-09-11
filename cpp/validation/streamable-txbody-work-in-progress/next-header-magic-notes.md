# Next dependencies: Magic, then TxHeader and TxPoW

During TxBody validation, read the complete pinned TxHeader.java and Magic.java,
and current cpp/src/org/minima/objects/TxHeader.hpp/.cpp and Magic.hpp/.cpp.
Neither native class currently adopts Streamable. Magic supplies a nested header
codec, so complete its common-interface and field-ownership work before TxHeader.
A complete caller/test and sibling review is still required before editing them.

Magic findings from the actual source:

- Eight public nullable reference fields; constructor current/desired fields share
  public constant objects. Native fields currently copy values.
- Reader assigns each field only after its factory finishes. Native only exposes a
  static factory, so retained receiver/partial-state behavior is not yet available.
- checkSame evaluates all four comparisons into local booleans before returning
  their conjunction. Native && currently short-circuits; a later null/throwing field
  must still be observed when an earlier comparison is false.
- Existing native weighted arithmetic follows the 16383/16384 formula, clamp range
  and capacity/work limits. Preserve that tested arithmetic while adapting ownership
  and source order; inspect null receiver/argument timing and public getters/setters.
- The pinned demonstration main performs 1728*50 iterations, restores desired KISS
  operations to 3000 every 1000 iterations, and prints every 50 iterations. The
  current mapped Magic.cpp does not contain that demonstration.

TxHeader findings:

- Public reference fields plus a nullable, replaceable superparent array.
  Constructor fills each array slot with its own empty MiniData object.
- Reader replaces scalar fields after complete factories, mutates the existing
  superparent array, then replaces Magic only after a complete Magic factory read.
- Each decoded RLE hash object is shared by every slot of its run. Native value
  slots do not preserve that exact object identity.
- Java loops against GlobalParams.MINIMA_CASCADE_LEVELS, independently of the actual
  retained array length. Native factory currently loops against its vector size.
- An oversized run fills available slots before Java throws at the first invalid
  index. The native pre-check currently rejects the whole run before those writes.
- Zero-count runs consume the count/hash and continue; EOF/malformed streams must
  be observed without adding an invented guard that changes source behavior.
- Writers append nested fields and RLE count before a later hash failure. The
  current staged serializer cannot expose all source failure prefixes.
- convertMiniDataVersion catches/logs IOException only; native currently catches
  every std::exception and silently returns null. Error/null/runtime paths need
  their own actual converter tests.

TxPoW now has an owned body/list getter, but full shared record, nullable header/
difficulty getters, in-place read behavior, header-hash catch/log/null assignment,
subclass and lifecycle semantics remain unfinished. Do not call the containing
class parity-complete based on this TxBody checkpoint.

Sibling Magic.hpp/.cpp was also read in tools/minima-core-cpp/src/objects. Its work
default is 00FF...FF instead of MAX_VAL/10000; it has pointer-only readers, an
offset-ignoring vector overload and no weighted-update algorithm, JSON or demo.
Its equality checks all eight fields, whereas source checkSame compares four current
fields eagerly. Preserve the current proven arithmetic/codecs when implementing
the remaining ownership/interface/source-order behavior.
