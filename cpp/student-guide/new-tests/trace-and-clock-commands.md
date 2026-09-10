# Lab: tracing switches and a millisecond clock

Written before the new tests. trace reads enable directly as a String: null or
missing throws Must specify enable, a wrong type fails the cast, and only exact
true enables it. Unlike the filter getter, enable is not trimmed. Read filter,
then update MessageProcessor's shared trace/filter BEFORE parsing network. A bad
network parameter therefore leaves the first update visible; it must not update
either NIO trace flag. The help calls filtering case-sensitive, while the existing
MessageProcessor matching code lowercases both sides. Test the actual matcher.

Reuse the Main fixture's unstarted processor as an observer of the shared state,
plus the actual NIOClient/NIOServer trace flags. Reset only fixture diagnostics at
the beginning of each program. Inspect state after successful and failed commands,
including runner gates and exchanges with network loggingon/loggingoff. No worker
or public peer needs to run to observe these state transitions.

timemilli captures the wall clock before reading optional offsets. minutesback has
priority over hoursback. MiniNumber.getAsInt truncates and wraps to signed 32 bits.
The multiplications by 60000 or 3600000 ALSO run in signed 32-bit Java arithmetic,
even though their result is assigned to long. Preserve that wrap with the native
unsigned/bit-cast pattern already used by TimerMessage, avoiding C++ signed overflow.

For successful replies, independently calculate the wrapped offset, add it back
to returned timemilli and require the reconstructed value to lie inside the
fixture's before/after clock bounds. Check the exact UTC date against that returned
millisecond value before normalizing independently sampled clocks. Cross both
parameters to prove minutes priority, plus fractions, large integers, malformed
numbers and null/non-string values. Metadata includes the source's leading tab in
getFullHelp, which differs from the usual leading newline.

Finite state/clock comparisons do not prove worker scheduling, all locales/time
zones or complete node behavior. Do not claim that returning a current timestamp
means either runtime can control or agree on an external system clock.
