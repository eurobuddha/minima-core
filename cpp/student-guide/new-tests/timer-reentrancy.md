# A timer callback can enter the timer again

Java's synchronized monitor is recursive: the same thread can acquire it twice.
TimerProcessor calls message formatting and logging while holding its timer-list
monitor. Those calls can inspect the timer, print it again, or append another timer.
A nonrecursive C++ mutex deadlocks on the second acquisition.

Changing the lock alone is insufficient. Appending can invalidate a C++ vector
iterator, while Java's ArrayList iterator detects structural changes on next().
The port now directly reuses the tested JSONSequence array iterator and its owned
state. It returns a shared message value, so no vector reference survives a callback.
A separate recursive timer monitor still serializes complete timer operations.

The Java enhanced-for loop obtains its iterator once. A nested run may replace the
current list, but the outer iterator still belongs to the original list. The native
processor therefore owns a shared list and retains it during each traversal. A
successful scan installs a new list; an exception leaves the existing list in place.
This preserves both old-list identity and effects that happened before failure.
Wall time is now read after acquiring the timer monitor, matching source order.

The new experiment calls the actual pinned Java printAllMessages/run methods and
the actual native methods. Empty, one-entry and two-entry lists are combined with
callbacks that read size, append, throw, print again, run again, post null or append
from the logger. Internal null entries exercise different print and run behavior.
Each comparison records ordered logs, callback events, exception class, interruption,
running flag, current contents and the retained original list's identity/contents.
Independent assertions check the expected exceptions and resulting list state.

Java uses an unstarted instance initialized through the established fixture helper;
C++ uses its manual factory. The real run loop still executes Thread.sleep(1000),
with an ordinary pending interrupt bounding the iteration. No interval is changed.
A nested run gets its own pending interrupt, and the outer run is interrupted again
so its later sleep also completes. Logger time/memory prefixes and stack frame text
are excluded; actual message text and order are compared.

Normal and address/undefined sanitizer builds each pass 48 comparisons / 688
assertions. The actual node executable builds pass, as do the three selected
regressions: thread interruption (12 comparisons / 194 assertions), JSON collections
(67 programs / 606 operations), and message workers. Full suite was not rerun.
[Saved evidence](../../validation/timer-reentrancy-work-in-progress/progress.json).
[Java fixture](../../support/TimerReentrancyOracle.java),
[native fixture](../../support/timer_reentrancy.cpp), and
[comparison driver](../../tools/timer_reentrancy_differential.py).

This is finite behavioral evidence,
not a proof of all thread schedules or every inherited/subclass API.

Check your understanding: after a callback appends C while printing A, should B
print? If a nested run removes A by installing a new list, can the outer traversal
still print A and B? Explain the role of the iterator's retained list.
