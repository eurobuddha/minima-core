# Porting the server's standalone entry point

NIOServer.main is a small program inside the server class. It creates a processor
named hello which logs incoming messages, starts a server on port 9002, waits two
seconds, stops it, waits two seconds, and repeats with a new server and processor.
It then calls System.exit(0). Its arguments are unused, so even null is accepted.

The C++ method reuses actual NIOServer and MessageProcessor implementations. The
anonymous Java subclass becomes a small private native subclass. Its factory
starts the worker after shared ownership exists. Server threads retain their own
server objects and use NativeThread for uncaught exceptions and source thread names.
If a sleep throws, those servers are not silently stopped by a local destructor:
Java leaves an already running server alive when main unwinds. The normal path uses
std::exit, which runs the existing native shutdown hooks; fatal bind still halts.

The source demo does not create MinimaDB. The hello worker first logs SERVERSTARTED,
then its finally block attempts database lock cleanup and throws NullPointerException.
That failure is observable and must be preserved. The native demo resolves the actual
MinimaDB singleton at cleanup and raises the same exception class when it is absent.
No database or wallet needs to be invented to make the demo appear cleaner.

The Java/C++ experiment launches separate child processes on the source port and
keeps all four 2000-ms waits. It records the main log sequence, actual hello worker
messages/failures, exit code and a registered shutdown hook. Four additional runs
interrupt the caller at each actual logger boundary and verify that sleep consumes
the pending flag. The child pauses after catching the exception while the parent
observes its listening sockets with lsof, without making a network connection.
After start interruption the server remains listening; after stop interruption it
closes. The fixture then halts its own child to release surviving threads/sockets.
An occupied-port case confirms fatal bind bypasses shutdown hooks.

A file lock serializes this fixed-port fixture across builds. A preflight bind fails
if another listener owns port 9002; it never shuts down an existing node. Elapsed-time
checks guard against accidentally shortening the original waits. Logger timestamp,
memory prefixes, exception prose and stack text are excluded; source message strings,
exception classes, flags, exit behavior and selected listener states are checked.

Normal and address/undefined sanitizer builds each pass six scenarios / 103
assertions. All three selected NIO CTests pass: the new main experiment, actual
loopback lifecycle, and 63 error-boundary cases. Both node executable builds pass.
The full suite was not rerun. [Saved results](../../validation/nio-server-main-work-in-progress/progress.json).
[Java fixture](../../support/NIOServerMainOracle.java),
[native fixture](../../support/nio_server_main.cpp), and
[comparison driver](../../tools/nio_server_main_differential.py).

Full NIOServer API, all startup races, lifecycle and
arbitrary thread interruption parity remain outside this finite experiment.

Check your understanding: why should an exception in the caller not automatically
close a separately running server? How does System.exit differ from Runtime.halt for
a registered shutdown hook, and which one occurs after a bind failure?
