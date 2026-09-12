# Public helpers, mutable constants and retained decimal values

The method-name audit found source APIs whose underlying behavior was already
available under other native names. A Java caller could use `ReadFromStream` or
`WriteToStream`, while the C++ counterpart exposed `deserialise` or an instance
writer. The correction adds the source-named helpers to MiniByte, MiniNumber,
MiniString and MiniData, including MiniData's expected-length overload and hash
reader. Each helper delegates to the existing checked reader or writer; there is
no second wire-format implementation.

## Serialization through an interface

`MiniData.getMiniDataVersion` accepts a Streamable, invokes its virtual writer,
and wraps the resulting bytes. Java catches only IOException, logs it, and
returns null. Runtime exceptions propagate. The native method reuses
`support/MiniDataVersion.hpp`; accepting a Streamable interface ensures a derived
writer is called through the existing serialization bridge. The native optional
result represents Java's nullable return, and a null object argument raises a
NullPointerException instead of producing an empty value.

The fixture writer appends a prefix and can then complete, throw an IO exception,
or throw a runtime exception. Tests check invocation count, returned bytes/null,
logging and exception class. The IO case verifies that a partially written prefix
does not escape as a successful result. Logger timestamps/stack details are not
compared; the presence of the expected logged IO exception is checked.

## A final reference does not freeze its object

Java declares MiniByte.FALSE and TRUE as final references. Their objects still
implement an in-place stream reader. In Java, `value.isFalse()` compares value with
the current FALSE object. The native implementation instead compared with numeric
zero and made the shared constant objects const. That diverges after a stream
read changes FALSE. The correction permits those in-place reads and delegates
isFalse to the existing equality operation, as the source does.

Sixteen cases vary FALSE and the queried byte independently, including nonzero
FALSE values. For example, with FALSE set to 255, byte 255 is false and byte 0 is
true according to this class's methods. Each fixture restores FALSE afterward so
one case does not contaminate the next. This is source compatibility, not a
recommendation to change global constants in an application.

## A getter must preserve the old value after replacement

MMREntryNumber.getBigDecimal returns the Java object's immutable decimal. A later
stream read replaces the receiver's decimal but cannot change an earlier returned
BigDecimal. The C++ helper uses the existing unbounded decimal carrier by value,
the same snapshot approach already used for MiniNumber's decimal accessor. Tests
keep the old value, replace the receiver through its real stream reader, then
check both coefficient/scale/text records. Values beyond MiniNumber's monetary
limits are included; this getter must not route through the bounded constructor.
The source-named MMR stream reader also delegates to its existing decoder.

## Evidence and limits

The existing base-value API runner retains its 1,164 cases and adds 689, for 1,853
total. Added cases check independent wire bytes with existing output prefixes,
all truncated prefixes for representative byte/number/string/data values,
expected-length reads, hash bounds, polymorphic conversion, mutable FALSE and
retained unbounded decimals. The completed configuration results are recorded in
[the evidence directory](../../validation/base-stream-helpers/README.md).

This closes thirteen Java public entry-point gaps across five existing classes;
it adds no new counterpart file. It does not certify arbitrary Java stream
subclasses, custom charset providers or full inherited/polymorphic base APIs.
MiniString's mutable MINIMA_CHARSET and broader subclass overrides remain audit
work. A method-name match is only a search clue, not a parity certificate.
