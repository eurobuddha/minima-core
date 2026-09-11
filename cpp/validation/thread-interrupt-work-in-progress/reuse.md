# Reuse evidence

Read pinned upstream-study/src/org/minima/utils/messages/MessageProcessor.java and TimerProcessor.java; system/commands/txn/txnlock.java, sendpoll/SendPollManager.java, and CommandRunner.java; corresponding cpp/src implementations and headers; cpp/src/support/NativeThread.hpp/.cpp, JavaExceptions.hpp; cpp/support/message_workers.cpp, native_thread_checks.cpp, txn_lock_workers.cpp, sendpoll.cpp, uncaught_workers.cpp, UncaughtWorkersOracle.java, NIOClientOracle.java; cpp/tools/message_differential.py, minima_uncaught_differential.py, command_differential.py and sendpoll_command_differential.py.

Searched the current source/test tree and /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src for interruption/sleep. Read the sibling src/system/MessageProcessor.hpp: its queue has no interrupt state, exception classification or timeout contract and cannot supply the missing behavior.

Reused the current NativeThread ownership/uncaught boundary, TimerProcessor condition-variable pattern, txnlock one-day duration chunking and existing Java fixture/compiler/test conventions. Added the smallest shared sleep/interrupt state and connected three mapped callers. No external dependencies introduced. The graph query identified NativeThread, TimerProcessor and MessageProcessor; source inspection, not the graph, establishes the behavior.
