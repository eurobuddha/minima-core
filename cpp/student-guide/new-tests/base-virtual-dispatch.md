# Inheritance changes which method runs

Java methods are overridable unless the source says otherwise. In C++, an instance
method needs `virtual` for a call through a base reference to reach a subclass.
Matching the ordinary object's return value therefore does not prove that the
translated class behaves like Java when inherited.

## MiniByte: a stored byte and an overridden getter can differ

Imagine a subclass that stores byte 0 but overrides `getValue()` to return 1024.
The pinned `MiniByte.isEqual()` compares the two objects' **getter results**. It
does not compare their private bytes. `toString()` also calls that getter. The
old C++ implementation compared private bytes and did not declare these methods
virtual, so the subclass's behavior disappeared through a `MiniByte&` reference.

The correction makes the six source value/query methods virtual and preserves
calls between them. `isTrue()` calls `isFalse()`, which calls `isEqual(FALSE)`;
each call can reach an override. `isEqual()` evaluates its own getter before the
other object's getter. C++ does not require left-to-right evaluation of `==`
operands, so storing the first result in a local variable makes that Java order
explicit. If the first getter throws, the second must never run.

Serialization still writes the stored byte directly. Overriding `getByteValue()`
or `getValue()` must not alter those bytes: Java's writer accesses its field.
This is why mechanically replacing every field access with a getter would be
wrong even after adding `virtual`.

## MiniString: copying can call a subclass

Java's `new MiniString(other)` obtains `other.toString()`. If a subclass overrides
that method, its returned text becomes the new ordinary wrapper's stored text.
The C++ compiler-generated copy instead copied the private field, bypassing that
method. The explicit copy constructor now makes the source's virtual call.

`isEqual(text)` also calls `toString()`. The stream writer calls `getData()`, which
may be overridden independently. But the base `getData()` itself encodes the
stored string, **without calling `toString()`**. The tests deliberately give a
subclass different stored text, displayed text and byte output to distinguish all
three paths. A copied ordinary wrapper encodes its copied text; it does not keep
the original subclass's overridden byte writer.

## How the regression exposes the difference

[The Java fixture](../../support/BaseDispatchOracle.java) subclasses the actual
pinned source classes. [The C++ fixture](../../support/BaseDispatchFixture.hpp)
subclasses the actual native classes and calls them through base references.
An override mask selects which methods intercept a call; the remaining overrides
delegate to the real superclass. Separate operations explicitly enter inherited
methods to check the virtual calls they make internally.

[The case generator](../../tools/base_dispatch_cases.py) supplies expected values,
wire bytes and ordered call traces. It also chooses an override that throws, then
checks the exact exception and which calls happened before it. Values include
negative/out-of-byte-range getter results, empty text, Unicode, embedded NUL and
arbitrary overridden bytes. Each result is checked independently and compared
between Java and C++.

There are 2,328 new cases, retaining all 1,853 prior API cases. Before the correction,
1,610 cases disagreed with Java; the independent expectations all agreed with Java.
Results after the correction are recorded in
[the validation directory](../../validation/base-virtual-dispatch/README.md).

## Check your understanding

A subclass stores byte 0, overrides `getValue()` to return 1024 and inherits its
other methods. What do `toString()`, `isFalse()` and serialization produce?
They produce `"1024"`, false and the single byte 00 respectively, assuming the
ordinary unmodified FALSE object.

A string subclass stores `"raw"`, returns `"display"` from `toString()` and returns
bytes FF from `getData()`. Its writer produces `00000001FF`. A new ordinary
MiniString copied from it contains `"display"` and writes that text's UTF-8 bytes.

## What remains outside this correction

MiniString's mutable charset, nullable Java strings/arguments, protected or package
field access and the rest of the object/reference model need separate work. The
native extra assignment and byte-subset helpers are not Java APIs. These tests do
not certify full class parity or subclass behavior in other wrappers. Student
lesson coverage and file counterpart counts remain distinct from parity proof.
