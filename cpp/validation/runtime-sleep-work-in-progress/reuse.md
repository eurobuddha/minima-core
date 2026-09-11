# Reuse gate and source boundaries

Pinned source: upstream-study at 5564b9d78978efcaf23679190d05253961fc6e69.
Read the named Java bodies/callers: src/org/minima/Minima.java,
system/Main.java, system/network/minima/NIOManager.java,
system/network/NetworkManager.java, system/network/p2p/P2PManager.java,
system/commands/network/rpc.java and utils/mysql/ArchiveServer.java under that root.
Inspected the matching current cpp/src/org/minima counterparts and their headers,
plus cpp/src/support/NativeThread.hpp/.cpp and the existing lock/runtime services.

Reused NativeThread rather than a second interruption implementation. The minimum
production adaptations are six default sleep call sites and Main's three missing
source-local InterruptedException catches. Other error handling is preserved.
No new dependency, Java application runtime or replacement database was added.

Existing test candidates read/reused: cpp/support/MainFixture.hpp,
MainNetworkFixture.hpp/.cpp, MainNetworkOracle.java, PeerRuntimeFixture.hpp,
SystemCheckFixture.hpp, main_runtime.cpp, MainRuntimeOracle.java,
SqlRecordingFixture.hpp, P2PManagerFixture.hpp, p2p_manager.cpp,
P2PManagerOracle.java, archive_server.cpp and ArchiveServerOracle.java.
The new fixtures reuse their unstarted owners, actual message dispatch, memory DB
construction and SQL recorder. The old network's completion is controlled; the
replacement NetworkManager is real. NativeThread flags and sleep remain real.

Reused drivers: cpp/tools/peers_checker_differential.py (Runner, TCP Responder),
p2p_functions_differential.py (request helpers), p2p_manager_differential.py
(existing canonicalizer), and archive_server_differential.py (existing prose/time
normalization). Existing corpora were not removed or reduced. Source catch clauses
and state mutation order supply independent expected results.

Sibling candidate searched: /Users/eurobuddha/Projects/minima/tools/minima-core-cpp,
including main.cpp, P2PSync.hpp and previously inspected MessageProcessor.hpp.
No compatible Java interruption adapter was found there. The maintained current
NativeThread implementation supplies the required state and exception behavior.

The source has additional thread/I/O semantics beyond sleep. This gate and the new
finite tests do not establish complete lifecycle, monitor, executor or I/O parity.
