# Reuse evidence

Pinned source: upstream-study at 5564b9d78978efcaf23679190d05253961fc6e69.
Read TimerProcessor.java in full and NIOManager.java hard/normal shutdown paths.
Read cpp/src/support/FixedTaskExecutor.hpp, NativeThread.hpp/.cpp and
cpp/src/org/minima/utils/messages/TimerProcessor.hpp/.cpp, MessageStack.hpp.
Read the existing fixed_task_executor, executor_uncaught and thread_interrupt
fixtures, Java oracles and differential drivers, plus NIOManagerStateOracle,
NIOClientOracle, PeerRuntimeFixture.hpp and nio_manager_errors fixture patterns.

Local OpenJDK 26.0.1 source at
/opt/homebrew/opt/openjdk/libexec/openjdk.jdk/Contents/Home/lib/src.zip supplied
ThreadPoolExecutor.awaitTermination, AQS.ConditionObject.awaitNanos, reacquire
and acquire. These establish shortcut order and consume-before-reacquire behavior.
Sibling /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/system/MessageProcessor.hpp
and network/P2PSync.hpp do not supply a compatible interruptible executor wait.

Reuse existing executor queue, worker ownership/replacement, shutdown state and
NativeThread interrupt bit. The queue condition variable alone cannot receive a
NativeThread interrupt. The minimal addition is scoped wake registration plus a
weakly owned private wait record; no new dependency. Timer reuses nativeThreadSleep
and stores its actual worker descriptor. Tests extend the existing thread driver
and NIO fixture approach. No source sleep interval was shortened.
