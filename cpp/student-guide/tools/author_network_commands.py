from lesson_writer import author,CPP

def w(name,purpose,steps,adapt,tests,exercise,limits,absent=False):
 author('src/org/minima/system/commands/network/'+name+'.md',purpose,steps,adapt,tests,exercise,limits+(' The mapped native file is absent; all validation proposed here is future work.' if absent else ' The counterpart remains partial and parity not_verified.'))
w('connect','Parses a requested peer endpoint and posts a connection request to the NIO manager.',[
 'Read host directly from the parameter object and cast it to String. Missing/null host throws the checked message No host specified; a wrong value type has a distinct cast failure.',
 'createConnectMessage splits at the first colon, trims the two pieces using Java String.trim and parses the entire remaining portion as a signed int.',
 'Missing colon or an unparsable port returns null. There is no port-range or nonempty-host validation here. Multiple-colon IPv6 text generally reaches the integer parser with extra colons.',
 'Build an NIO_CONNECT Message containing host and port, post it to Main’s NIO manager, then return an Attempting to connect message. Posting is asynchronous intent, not confirmation of a successful socket connection.'
], 'Native code reuses the Java-compatible integer parser and explicit trim boundary, returning a shared Message. A required postMessage callback supplies runtime integration. Java checked command errors are represented separately from generic standard failures.',
 lambda l:'The '+l(CPP/'support/NetworkStartupOracle.java','startup oracle')+', '+l(CPP/'support/network_startup.cpp','native harness')+' and '+l(CPP/'tools/network_startup_differential.py','driver')+' test missing/wrong-type hosts, first-colon behavior, whitespace, Unicode integer digits, int bounds, negative/out-of-range socket ports, posting failures, command-runner sequences and metadata. They compare parsed message content and posted effects, never open a peer connection. The inspected startup report has 532 matching cases across connect and Greeting work.',
 'Why does a syntactically valid port of -1 pass createConnectMessage? Where must a real connection attempt enforce the socket’s valid range?',
 'Full network success, exact default-null optional errors and asynchronous failure delivery are outside this command fixture.')
w('disconnect','Requests disconnection of one peer by UID or all peers.',[
 'Read uid directly from the parameter object; null/missing UID throws No uid specified and a wrong type fails the String cast.',
 'For exact lowercase all, post NIO_DISCONNECTALL to the NIO manager.',
 'For every other UID, call the manager’s disconnect(uid) method directly. The command does not normalize case or validate UID syntax.',
 'After that call returns, set status true and describe the attempted disconnection. The response does not demonstrate that a remote peer has already closed.'
], 'There is no mapped disconnect.cpp yet. A future counterpart can reuse Command, Message and the mapped NIO manager, but must preserve the all-versus-specific dispatch distinction and error order.',
 lambda l:'No direct native command test is claimed. Proposed validation: controlled manager callbacks should record PostMessage versus disconnect calls for all, ALL, empty, ordinary, null and non-string UIDs; inject receiver failures and compare whether a response is returned. Existing NIO tests concern lower-level components and do not substitute for this absent command.',
 'Why is disconnect uid:ALL a different branch from uid:all? What would a callback-recording test need to observe?',
 'This page explains pinned Java behavior and a future validation plan, not an implemented command.',True)
w('message','Wraps user text in MiniString and asks the NIO manager to send a generic peer message.',[
 'Read data as a raw String parameter and reject missing/null data with No data specified.',
 'Read optional uid. If null, replace it with the empty string and describe a broadcast; otherwise describe a message to the supplied UID, including an explicitly empty UID.',
 'Construct MiniString from the data, then call sendNetworkMessage(uid, MSG_GENMESSAGE, payload). UTF-8 normalization belongs to that wrapper.',
 'Return the reply after the send call. The command contains no delivery acknowledgement or retry mechanism.'
], 'No mapped native message.cpp exists. A future implementation should reuse MiniString and the manager send API rather than inventing an alternate frame format.',
 lambda l:'Proposed tests should capture UID, message type and serialized payload for omitted/empty/specific UIDs and ASCII/Unicode text; compare null/wrong-type data failures and injected send exceptions. No existing lower-level transport pass is counted as a native command test.',
 'Can omitted uid and explicitly empty uid produce the same send arguments but different reply wording? Trace the branches.',
 'Validation and delivery behavior remain unimplemented for this mapped command.',True)
w('rpc','Controls RPC enablement and the additional RPC-user records exposed through UserDB.',[
 'Obtain UserDB, then give the enable parameter priority over action. If enable exists, parse its boolean value, stop RPC, sleep one second and only then update RPC_ENABLED.',
 'If password is present, store it and enable authentication; otherwise disable authentication without necessarily erasing the old password string. Start RPC when enabled, or call stopRPC again when disabled.',
 'Although ssl appears in the advertised parameters/help, this runCommand body does not read or apply it.',
 'For action adduser, require default RPC authentication, forbid exact username minima, read password, lowercase mode and require read or write. Append a user object without checking duplicate usernames.',
 'For removeuser, build a new array excluding every exact matching username and replace UserDB’s array. listusers switches output from a count to the full stored user array. Unknown action falls through to status output.',
 'The response masks the default password as ***. When listusers is selected, it includes the stored extra-user records, which contain their password fields.'
], 'The mapped rpc.cpp is absent. Reuse candidates for a future port are Command parameter parsing, GeneralParams, UserDB and NetworkManager start/stop boundaries; no new authentication backend is required to explain the pinned command.',
 lambda l:'Proposed tests should record stop/sleep/global-update/start order, enable-over-action precedence, omitted password, unused ssl, duplicate usernames, reserved username, mode validation, replacement-array aliasing and listusers output. Inject interruption or manager failure at each stage to inspect partial changes. These are planned tests, not reported runs.',
 'When enable:false is supplied, how many stopRPC calls occur on the successful path? Why is masking the default password not the same as masking all users in listusers?',
 'The source’s help text is not evidence that ssl is implemented here. This unported command needs both behavioral tests and runtime binding before use.',True)
w('webhooks','Manages notification hooks and returns the current stored hook strings.',[
 'Default action to list, obtain the NotifyManager and create the reply’s response object.',
 'For add, read optional filter and required hook, concatenate filter + # + hook, and pass that complete string to addHook.',
 'For remove, pass the supplied hook string directly; it does not reconstruct the filter prefix. Exact stored-string removal therefore matters.',
 'For clear, clear all hooks. For errorlogs, parse enable and obtain the manager again before changing WEBHOOKS_ERROR_LOGS.',
 'For list or unknown actions, perform no mutation. Every path then retrieves the hook list and returns it under response.webhooks. A missing manager can fail after some parameter parsing, depending on the branch.'
], 'Native code uses UTF-16 parameter helpers to preserve Java string behavior, including isolated surrogate data, and a required runtimeLookup callback. The callback is invoked a second time specifically in the errorlogs branch, matching the source’s receiver order.',
 lambda l:'The '+l(CPP/'support/NotifyManagerOracle.java','notification oracle')+', '+l(CPP/'support/notify_manager.cpp','native harness')+' and '+l(CPP/'tools/notify_manager_differential.py','driver')+' execute real webhook commands directly and through CommandRunner with controlled manager/HTTP receivers. Command cases cover metadata, parameter types, missing manager, exact removal with or without # prefix, unknown actions, admin caller checks, Unicode filters and case-sensitive errorlogs values. They record reply/error plus hook/database state and manager lookup order. Null/cast expression descriptions are normalized; no remote hook is contacted.',
 'After adding hook:http://fixture/a with no filter, what exact string must remove receive? Why can removing the unprefixed URL leave the hook present?',
 'The command fixture does not establish arbitrary real HTTP delivery, full Main binding or every logger/URL transport failure. NotifyManager’s separate lesson covers delivery logic.')
w('network','Exposes connection information and several network-control actions through one command.',[
 'Default action to list. Obtain all NIO connection information and append every item when uid is empty, otherwise append the first exact UID match. Although this body reads uid, the advertised valid-parameter list contains only action, so runner validation can reject a supplied uid before this branch.',
 'For list, obtain NetworkManager again and, when present, append its detailed status. Receiver lookup order is observable when the runtime changes or returns null.',
 'reset clears traffic counters; recalculateip invokes host-IP calculation and returns GeneralParams.MINIMA_HOST; restart posts MAIN_NETRESTART asynchronously.',
 'loggingon and loggingoff change NIO logging first and P2P logging second. An exception between the calls can leave only one subsystem changed.',
 'Unknown actions throw CommandException. A successful restart reply records a request, not completion of a restart.'
], 'This mapped command is absent. A future implementation should reuse Command, manager status objects and Message dispatch, preserving raw UID filtering and the parameter-validation discrepancy rather than silently broadening valid parameters.',
 lambda l:'Proposed tests should compare direct execution with CommandRunner for uid, capture list order and first-match behavior, and record manager lookups and side effects for every action. Inject a failure between the two logging calls and inspect the retained first change. No native command execution is claimed.',
 'Why can a direct call filter by UID while a normal runner rejects the same parameter?',
 'Network restart, IP discovery and manager bindings remain future native integration work.',True)
w('nodecount','Crawls peer-advertised addresses and estimates server/client totals, optionally writing progress rows.',[
 'Optionally open a progress file and write a CSV header. The source does not close the PrintWriter. Read complete, then create sets for discovered addresses and already-pinged addresses.',
 'Ping every default seed with the full-information flag. Add advertised peers from successful greetings to the discovered set, and mark seeds pinged even on failure. Seed client counts are not accumulated.',
 'Scan discovered addresses not yet pinged. For each successful greeting, parse its clients field as a double and union its advertised peers into the discovered set. Mark every attempted address pinged.',
 'Compute percentage as pinged/discovered/0.06 times 100. Unless complete is true, stop at that threshold. Complete mode uses pinged/discovered times 100 instead. A zero discovered count can yield floating NaN or infinity.',
 'After each attempt, optionally write and flush a CSV row. Recompute the pending set after a batch so newly discovered addresses can be visited.',
 'At termination, estimate servers as discovered count times 1.05 and clients as accumulated clients/pinged count times estimated servers. Complete mode uses discovered count and accumulated clients directly. The denominator includes seed attempts whose client counts were never added.'
], 'No native nodecount counterpart exists. A faithful port needs controlled ping responses, address-set identity and Java float/double conversion semantics. The source’s statistical confidence comment is not a validated property of this algorithm.',
 lambda l:'Proposed deterministic fixtures should supply a finite fake peer graph with duplicate addresses, failed seeds, absent clients, newly discovered peers and an empty graph. Compare query order only where set iteration is specified, plus final totals, early stopping and flushed CSV content. Test partial file output on parse failures and document resource lifetime. No external network crawl or native test has been run for this page.',
 'If two seed attempts are counted as pinged but only later peers contribute client totals, how can the estimated clients-per-server ratio change?',
 'This source algorithm is an estimator with observable rounding and sampling quirks; it is not evidence of the true size of a live network.',True)
w('p2pstate','Returns the P2P manager’s detailed status as a top-level reply field.',[
 'Create the standard command reply.',
 'Resolve Main, then NetworkManager, then its P2P manager, and cast that receiver to P2PManager.',
 'Call getStatus(true) and store the result under the top-level key p2p-state. It does not use the usual response key.',
 'Return the reply. Null receivers or an incompatible manager fail before a status object can be returned.'
], 'There is no mapped native command yet. Reuse the manager’s existing status representation and preserve the unusual reply key and detailed flag.',
 lambda l:'Proposed tests should inject a status-producing receiver and assert the exact top-level key, detailed=true argument and call order. Null and wrong-type receiver tests should compare failure categories. Existing manager tests do not establish this absent command’s behavior.',
 'What would break in a client that assumes every successful command stores data under response?',
 'Runtime binding and command-specific validation remain future work.',True)
w('peers','Lists, checks, imports or publishes the P2P peer-address list.',[
 'If P2P is disabled or the node is a slave, return immediately with peers-list empty, havepeers true and a disabled message. This key contains a hyphen; ordinary list output uses peerslist.',
 'list asks getPeersList for a randomized bounded list, counts comma-separated entries and corrects an empty string to size zero. forcecheck posts PEERS_FORCEFULLCHECK, then reports the list without that empty correction, so its empty size can differ.',
 'addpeers reads a required string; a prefix beginning http triggers an HTTP GET. Tokenize by comma, discarding empty tokens. Parse each endpoint through connect.createConnectMessage; accepted endpoints become InetSocketAddress values and PEERS_CHECKPEERS messages with force=true.',
 'publish writes up to the helper’s nominal 20 peers to a base-path file using the platform default byte encoding. fetch rejects an existing local file, otherwise downloads the url, tokenizes and posts checks similarly.',
 'getPeersList obtains a copy, shuffles it, then stops only when counter > max. Consequently a nonnegative maximum N can return N+1 peers. It formats resolved IP addresses and strips the final comma on the successful path.',
 'The helper catches exceptions and returns accumulated text. If failure precedes comma cleanup, partial output may retain a trailing comma. Importing peers is incremental: a later invalid socket port can fail after earlier check messages were posted.'
], 'The mapped native command is absent. Reuse connect’s endpoint parser and existing address/message types, while retaining differences between parsing an integer port and constructing a valid socket address. Randomization should be controlled by a fixture seam for reproducible comparisons.',
 lambda l:'Proposed tests cover disabled/slave early replies, list versus forcecheck empty counts, max -1/0/1, unresolved addresses, duplicate and empty comma tokens, HTTP-empty failure, local-file rejection and partial posting before a bad port. Capture file bytes and intended check messages with fake HTTP/file/manager receivers. These tests are future work; no live peer list is contacted.',
 'Why can max:0 still publish one peer? Trace the counter comparison before incrementing it.',
 'File operations, downloading, random selection and real P2P admission are not implemented by this absent counterpart.',True)
w('ping','Sends a ping request to one parsed endpoint and formats a greeting response.',[
 'Read the required host parameter through Command, then find the first colon.',
 'If no colon is present, return null rather than an error reply. Otherwise trim the address and parse all text after that first colon as an integer port.',
 'Translate an integer-parsing failure into CommandException with Invalid port and the supplied port text. This body does not separately check socket-port bounds or support bracketed IPv6 parsing.',
 'Call sendPingMessage(address, port, false). Report host, port and whether a greeting was returned.',
 'When a greeting exists, include its version and extra data. A null greeting produces valid false without those fields.'
], 'No mapped native ping command exists. The future implementation should reuse the established Java-compatible parameter and integer parsing helpers and Greeting JSON fields, while keeping null-return behavior distinct from a command error.',
 lambda l:'Proposed tests should use a controlled ping receiver for missing colon, surrounding whitespace, integer overflow, extra colons, negative ports, absent greeting and a greeting with extra data. Assert the false full-information flag and exact response shape. They are planned command tests, not evidence supplied by the Greeting fixture.',
 'How do host:example and host:example:abc differ before any network call occurs?',
 'Native command behavior and actual socket communication remain unimplemented.',True)
