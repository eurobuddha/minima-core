#!/usr/bin/env python3
"""Create source reading notes; handwritten lessons have a separate review marker.
Does not modify existing guides unless --refresh-drafts is supplied. It is not a Java parser.
"""
import argparse,hashlib,json,os,re
from pathlib import Path
from builtin_lessons import LESSONS
GUIDE=Path(__file__).resolve().parents[1];CPP=GUIDE.parent;ROOT=CPP.parent
ROWS=json.loads((CPP/'source-map.json').read_text())['files']
SOURCE=ROOT/'upstream-study'
def link(page,target,label=None,line=None):
 return f'[{label or target.name}]({os.path.relpath(target,page.parent)}'+(f'#L{line}' if line else '')+')'
def mask(s):
 return re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',lambda m:''.join('\n' if c=='\n' else ' ' for c in m[0]),s,flags=re.S)
def methods(s):
 m=mask(s);out=[]
 rx=re.compile(r'\b(?:public|protected|private)\s+(?:(?:static|final|synchronized|abstract|native|default)\s+)*(?:[\w<>\[\],.?]+\s+)?([\w$]+)\s*\([^;{}]*\)\s*(?:throws\s+[\w.,\s]+)?\s*\{')
 for x in rx.finditer(m):
  start=m.find('{',x.start());depth=1;end=start+1
  while end<len(m) and depth:
   depth+=(m[end]=='{')-(m[end]=='}');end+=1
  out.append(dict(name=x[1],start=x.start(),body_start=start,end=end,line=s.count('\n',0,x.start())+1,body=s[start+1:end-1]))
 return out

def overview(row,s,ms):
 name=Path(row['java']).stem
 desc=re.search(r'super\(\s*"[^"\n]+"\s*,\s*"((?:\\.|[^"\\])*)"',s)
 if desc:return 'This command exposes the following user operation: '+desc[1].replace('\\n',' ').strip()+'. Its `runCommand` body is where parameter parsing, validation, service calls and JSON results meet.'
 known={
 'Minima':'The Java process entry point parses startup options, configures the application and starts Main. A working native node must eventually implement this lifecycle, not just compile the library.',
 'Main':'The central runtime coordinates databases, network components, workers, events and shutdown. Its native counterpart is absent, so passing isolated component tests does not establish a working node.',
 'MinimaDB':'The database owner coordinates the wallet, transaction/block stores, chain tree and preferences. This layer controls ordering and lifetime; it is not itself an implementation of the H2 storage engine.',
 'Contract':'A KISS execution context combines script parsing, typed values, transaction/witness/state data, instruction limits, execution results and trace output.',
 'Transaction':'A transaction records the coins consumed and created, state variables and link information. Its validation and serialization determine what is signed and hashed.',
 'TxPoW':'A transaction proof-of-work object combines a header and optional body, derives IDs and tracks cached validation state. A getter can therefore depend on both object contents and earlier operations.',
 'TxPoWChecker':'This checker applies transaction, signature, script, MMR and block rules. A rejection can have observable logs and cache effects in addition to a Boolean result.',
 'TimedChecker':'This worker wrapper runs block checking while the caller waits up to a deadline. Worker completion, validation result and caught versus uncaught failures are distinct states.',
 'TxPoWGenerator':'This component assembles transactions and candidate blocks from chain and mempool information, including coin IDs, difficulty and superparent data.',
 'TxPoWProcessor':'This message-driven component inserts and processes blocks, updates chain selection and coordinates dependent node effects.',
 'TxPoWSearcher':'This collection of search operations locates coins, transactions and relevant chain information for callers.',
 'TxPoWMiner':'The miner searches nonce space against proof-of-work difficulty while responding to job changes and reporting results.',
 'MMR':'A Merkle Mountain Range stores hash-and-sum nodes in a forest of perfect binary trees. Appending leaves, combining peaks, producing proofs and pruning all have serialization and historical-state consequences.',
 'MMRData':'This value couples a hash with a sum. Separate leaf and parent construction rules make the bytes being hashed unambiguous.',
 'MMREntry':'An MMR entry identifies a row/entry position and its node data, supporting tree navigation and serialization.',
 'MMREntryNumber':'This arbitrary-size entry-number type supplies the arithmetic and bit-position operations used to navigate an MMR.',
 'MMRProof':'An MMR proof records the sibling steps needed to fold a leaf into a root hash and sum.',
 'MegaMMR':'The larger MMR state applies block coin changes, keeps relevant coins and serializes snapshots for synchronization.',
 'Cascade':'The cascade keeps a compact representation of older chain work by retaining selected nodes at superblock levels.',
 'CascadeNode':'A cascade node wraps one retained TxPoW with links and weight/level information used by cascade processing.',
 'TxPowTree':'The chain tree organizes competing branches and chooses a tip using accumulated work. Branch insertion and pruning affect search and notification behavior.',
 'TxPoWTreeNode':'One chain-tree node links a TxPoW to its parent, children, weight and MMR state.',
 'Coin':'A coin is a spendable output with an identifier, address, amount, token ID and state/storage metadata.',
 'CoinProof':'A coin proof couples the actual coin data with its MMR membership proof.',
 'Token':'Token metadata defines the token identity, scaling, name and script. Stored coin amounts and human token amounts are not interchangeable.',
 'Witness':'The witness supplies signatures, scripts and coin proofs needed to validate a transaction without keeping all historical data.',
 'ScriptProof':'A script proof supplies a script together with the proof data used to establish its script address.',
 'StateVariable':'A state variable binds a numbered port to typed/textual state data that contracts can read across transactions.',
 'Address':'An address derives from a locking script and supports the displayed Minima address representation and raw hash bytes.',
 'MiniData':'This foundational byte value supplies raw bytes, hexadecimal text, numeric ordering, hashing and stream conversion.',
 'MiniNumber':'This foundational decimal value models Java BigDecimal-style arithmetic, limits, rounding and binary serialization without ordinary floating-point substitution.',
 'MiniString':'This foundational text value connects Java string behavior with UTF-8 bytes and the stream format.',
 'MiniByte':'This one-byte value distinguishes byte, unsigned integer and Boolean interpretations while preserving its stream representation.',
 'Magic':'Magic carries adjustable block limits and updates them according to source-defined arithmetic.',
 'TxBody':'The body groups transaction and witness data, burn data and block transaction references.',
 'TxHeader':'The header carries the data hashed for proof-of-work, including parent information, time, difficulty and commitments.',
 'TxBlock':'This block object combines TxPoW with the coin/MMR information used for block application and transport.',
 'IBD':'Initial block download chooses chain/archive data for bringing another node into sync.',
 'Greeting':'A greeting introduces a peer’s version, extra connection information and chain summary.',
 'Pulse':'A pulse advertises recent chain identifiers in a compact network message.',
 'Signature':'A TreeKey signature groups signature proof levels and provides wire/verification input handling.',
 'SignatureProof':'One signature proof level couples a public key/signature with its MMR proof data.',
 'TreeKey':'TreeKey organizes one-time keys into a hierarchy and signs/verifies through that hierarchy, including key-use accounting.',
 'TreeKeyNode':'A TreeKey node holds the key-tree information used to derive or connect public keys and proofs.',
 'Winternitz':'This wrapper supplies Winternitz one-time signatures using Minima’s chosen hashing and parameter rules.',
 'Command':'The command base defines parameter access, conversion, validation and the common JSON reply envelope.',
 'CommandRunner':'The runner parses command strings, dispatches commands and preserves multi-command stopping and error rules.',
 'CommandException':'This exception identifies a command-level failure so command runners can produce the expected user-facing response.',
 'MinimaFunction':'The function base owns unevaluated argument expressions, checks arity/type rules and returns fresh function instances from the built-in registry.',
 'TimedRequest':'This object tracks prior request IDs. Its sendNow method only scans for a matching ID; despite the name and stored timestamp, it neither expires entries nor inserts new ones.',
 'Streamable':'This interface declares the read/write contract used by the Java wire format. It contains no node runtime by itself.',
 'P2PSwapLink':'The pinned file is wholly commented out. Its planned C++ path is an inventory assignment; activating its historical code would change the pinned program.',
 'SqlDB':'This base wraps SQL connection and lifecycle operations. The native connection interface is a boundary for a future H2-compatible provider; it is not a working database engine.',
 'Wallet':'The wallet manages seeds, keys and scripts through database operations. Record-conversion tests do not prove durable encrypted wallet storage.',
 'JsonDB':'This small persistence layer stores an ordered JSON object through file operations and exposes synchronized value access.',
 'MinimaLogger':'This logger formats messages and exceptions, prints them, and forwards notification events. Print and callback failures can change control flow.',
 'MinimaUncaughtException':'This handler decides what to log or terminate when a worker failure escapes its normal catch boundary.',
 'Message':'A message holds a type and typed payload entries used by asynchronous components.',
 'MessageStack':'A message stack queues and filters messages for later processing.',
 'MessageProcessor':'This worker consumes messages, routes them to a subclass and coordinates timer posting, error handling and shutdown.',
 'TimerMessage':'A timer message pairs a message with its intended delivery time.',
 'TimerProcessor':'The timer worker keeps delayed messages and forwards them when their scheduled time arrives.',
 'MessageListener':'This interface defines the callback used to receive a message.',
 'FastByteArrayStream':'This output stream grows a backing byte buffer in fixed chunks and exposes size/reset/write behavior.',
 'BaseConverter':'This utility converts between byte sequences and supported textual radices, including Minima address encodings.',
 'Crypto':'This shared utility performs the hashes used by serialized objects, addresses and proofs.',
 'BIP39':'This utility uses the pinned word list and Minima’s seed/mnemonic rules. Compatibility must be derived from this implementation rather than a similarly named generic library.',
 'MiniFile':'This file utility connects Streamable values, raw bytes and filesystem operations.',
 'MiniFormat':'This utility formats quantities such as byte sizes for display.',
 'MiniUtil':'This utility supplies the source’s text, collection and date conversions.',
 'Maths':'This utility supplies integer/logarithmic arithmetic used by other components; floating rounding and signed bit rules can be observable.',
 'Stack':'This simple last-in-first-out container keeps message-compatible values and exposes push/pop/inspection behavior.',
 'ParamConfigurer':'The startup parameter registry interprets arguments, environment entries and configuration files, mutating shared settings and selected runtime resources.',
 'GeneralParams':'These mutable settings control runtime behavior, including logging, networking and configuration paths.',
 'GlobalParams':'These global protocol/default parameters are shared across chain, proof and runtime components.',
 'TestParams':'These parameters select the source’s test-mode settings; initialization and assignment order matter when settings objects are shared.',
 'NIOClient':'This peer client handles connection metadata, partial frame reads/writes, send queues, traffic accounting and manager callbacks.',
 'NIOClientInfo':'This object presents the client metadata used by peer-management code.',
 'NIOManager':'This manager routes peer connection events, creates workers and clients, serializes sends and processes network management messages.',
 'NIOServer':'This listener runs the source’s nonblocking connection/event loop and manages selection registrations and shutdown.',
 'NIOMessage':'This dispatcher interprets incoming Minima protocol message types and invokes chain/database/network processing.',
 'NIOTraffic':'This counter groups transmitted and received byte totals for network reporting.',
 'RelayPolicy':'This policy decides whether transactions satisfy the relay limits used by network and consensus callers.',
 'NetworkManager':'This owner coordinates the Minima peer network, P2P management and RPC server lifecycle.',
 'HTTPServer':'This listener accepts HTTP connections and dispatches requests into worker handlers.',
 'CMDHandler':'This HTTP handler parses and authorizes command requests, invokes the command runner and builds the response.',
 'Authorizer':'This utility applies the configured RPC authorization rule, including Basic credential decoding.',
 'Server':'This interface/class defines the network server contract consumed by the HTTP/RPC lifecycle.',
 'RPCClient':'This client performs the source’s HTTP/HTTPS requests, authentication, redirects and response handling.',
 'MinimaRPCClient':'This command-line program parses RPC client arguments, sends commands and supports interactive input.',
 'NotifyManager':'This message-driven manager routes events to listeners and configured webhooks, including persistence and error handling.',
 'P2PState':'This object stores peer-link state, known peers and shared lists used by P2P routing.',
 'P2PManager':'This message-driven P2P owner manages peer links, discovery, persistence and periodic connectivity work.',
 'P2PFunctions':'These peer helpers build and send protocol messages and query local/network availability.',
 'UtilFuncs':'These peer utilities perform address/list selection and shared P2P helper operations.',
 'P2PPeersChecker':'This worker verifies candidate peers and manages verified/unverified sets and retry timers.',
 'SwapLinksFunctions':'These handlers implement source-defined link-swap admission, secret validation, disconnect and response effects.',
 'WalkLinksFuncs':'These handlers walk the peer graph and change links using route history, secrets and randomized next-hop selection.',
 'InetSocketAddressIO':'This wrapper converts socket addresses to/from the wire/JSON forms used by peer messages.',
 'P2PGreeting':'This P2P greeting carries peer-state information used during link establishment.',
 'P2PDoSwap':'This message carries the fields required to request or perform a peer link swap.',
 'P2PWalkLinks':'This message carries a walk through peer links, including its path and routing metadata.',
 'P2PDB':'This JSON-backed database stores peer addresses for later discovery.',
 'P2P2DB':'This JSON-backed database stores the peer list used by the second P2P implementation.',
 'P2P2Functions':'These helpers support the second P2P implementation’s peer selection and connection operations.',
 'P2P2Manager':'This worker manages the second P2P implementation’s state, timers and network effects.',
 'P2PParams':'These settings define peer-link limits, defaults and logging behavior.',
 'P2PTestParams':'These settings overwrite selected P2P defaults for source test mode, including shared peer-list values.',
 'SendPollManager':'This worker repeatedly submits queued send requests through the command runner and manages their lifecycle.',
 'SendPollMessage':'This record stores a queued send-poll request and its identity.',
 'RawArchiveInput':'This reader decodes a gzip archive incrementally into the serialized chain objects expected by archive import.',
 'ArchiveManager':'This SQL-backed archive manages stored blocks and archive metadata; a recording SQL receiver does not provide durable H2 storage.',
 'TxBlockDB':'This cache stores/retrieves TxBlock objects while preserving relevant shared object identity.',
 'TxPoWDB':'This owner coordinates RAM and SQL storage for transaction proof-of-work objects.',
 'TxPoWOnChainDB':'This SQL table tracks whether transaction proofs are included on the chain and prunes retained membership records.',
 'RamDB':'This in-memory store indexes TxPoW objects and exposes source-defined cache, status and cleanup behavior.',
 'RamData':'This record adds cache metadata to a stored TxPoW reference.',
 'TxPoWList':'This wrapper maintains a shared list of TxPoW data used by SQL/cache retrieval.',
 'TxPoWSqlDB':'This SQL-backed component stores and retrieves serialized TxPoW rows.',
 'CoinDB':'This cache indexes coin information used by chain processing and search.',
 'TxPoWTreeNodeAction':'This callback specifies an action to run while visiting a chain-tree node.',
 'UserDB':'This preferences store provides typed access to node/user settings through JsonDB.',
 'TxnDB':'This store manages named draft transactions and their witnesses for transaction-building commands.',
 'TxnRow':'This record holds one named draft transaction and related witness state.',
 'KeyRow':'This wallet record preserves the key table’s column values and JSON representation.',
 'ScriptRow':'This wallet record preserves the script table’s column values and JSON representation.',
 'SeedRow':'This wallet record preserves the seed table’s column values and JSON representation.',
 'GenesisCoin':'This class constructs the protocol’s genesis coin from its fixed source-defined data.',
 'GenesisMMR':'This class builds the initial MMR containing the genesis coin.',
 'GenesisTxPoW':'This class builds the genesis proof-of-work object using the genesis coin/MMR and source defaults.',
 }
 if name in known:return known[name]
 if '/utils/json/parser/' in row['java']:return 'This JSON parser component supplies '+name+' behavior. Follow its entry methods below to distinguish tokenization, parser state, callbacks and error reporting.'
 if '/utils/json/' in row['java']:return 'This JSON component supplies '+name+' behavior for construction, escaping, containers or output. Its concrete methods below show which part of the JSON pipeline it owns.'
 if '/kissvm/' in row['java']:return 'This KISS component supplies '+name+' behavior in script evaluation. Its methods below show how it connects parsing, typed values, execution or error propagation.'
 if '/utils/encrypt/' in row['java']:return 'This encryption component supplies '+name+' operations. Algorithm names alone are insufficient: follow its byte encodings, keys, parameters and failure order in the source.'
 if '/utils/sphincs/' in row['java']:return 'This experimental hash-signature component supplies '+name+' behavior. Its presence in the source inventory is not evidence that it is the active transaction-signature path.'
 return 'This file defines '+name+' in `'+'.'.join(Path(row['java']).parts[1:-1])+'`. The method walkthrough below is a source-reading draft; its full architectural explanation still needs review.'

# Imported harness references are navigation evidence, never inferred execution coverage.
HARNESS=[]
for p in sorted((CPP/'support').glob('*')):
 if p.suffix in ('.java','.cpp','.hpp'):HARNESS.append((p,p.read_text(errors='replace')))
def references(row):
 stem=Path(row['java']).stem;qualified='.'.join(Path(row['java']).with_suffix('').parts[1:]);hits=[]
 for p,s in HARNESS:
  if qualified in s or re.search(r'["<][^"\n>]*\b'+re.escape(stem)+r'\.hpp[">]',s):
   ls=s.splitlines();matched=[(i+1,l.strip()) for i,l in enumerate(ls) if re.search(r'\b'+re.escape(stem)+r'\b',l) and not l.lstrip().startswith(('import ','#include'))]
   if matched:hits.append((p,matched[0]))
 return hits[:4]

def flow(page,source,method):
 out=[];startline=source.read_text().count('\n',0,method['body_start'])+1
 body=method['body'];lines=body.splitlines();events=[]
 for i,l in enumerate(lines):
  clean=l.strip()
  if clean.startswith('//') and len(clean)>5:events.append((startline+i,'Source author’s cue: “'+clean[2:].strip()+'”'))
  elif re.match(r'(if|else if|while|for|switch)\s*\(',clean):events.append((startline+i,'Decision/repetition: `'+clean[:220].replace('`','')+'`'))
  elif clean.startswith(('throw ','return ','catch','}catch','} catch')):events.append((startline+i,'Exit/error boundary: `'+clean[:220].replace('`','')+'`'))
 if not events:
  for i,l in enumerate(lines):
   if l.strip() and l.strip() not in ['{','}']:events.append((startline+i,'Operation: `'+l.strip()[:220].replace('`','')+'`'))
 # Spread long methods across their body; don't pretend the excerpts are all branches.
 if len(events)>9:events=events[:4]+events[len(events)//2:len(events)//2+2]+events[-3:]
 for n,(ln,msg) in enumerate(events,1):out.append(f'{n}. {msg} ({link(page,source,"source",ln)}).')
 return '\n'.join(out) or 'This body is empty: it adds no executable work.'

def build(row):
 source=SOURCE/row['java'];s=source.read_text();native=CPP/row['cpp'];page=GUIDE/'files'/Path(row['java']).with_suffix('.md');name=source.stem
 ms=methods(s);lesson=LESSONS.get(name) if '/kissvm/functions/' in row['java'] else None
 page.parent.mkdir(parents=True,exist_ok=True)
 lines=['# '+name+' — '+('KISS built-in lesson' if lesson else 'source reading notes'),'',
 '**Teaching status:** '+('Authored algorithm and test explanation; full API/parity review remains open.' if lesson else 'Draft walkthrough. Source locations and test references are provided; a complete teacher-level review of this file is still pending.'),'',
 '**Port status:** `'+row['status']+'`; parity `'+row['parity']+'`. '+('The mapped C++ file exists.' if row['cpp_exists'] else 'The mapped C++ file is absent. This page does not describe a completed translation.'),'',
 '| Source | Destination |','|---|---|',f'| {link(page,source,row["java"])} | '+(link(page,native,row['cpp']) if native.exists() else '`'+row['cpp']+'` (planned)')+' |','',
 '## What this file does','',lesson['purpose'] if lesson else overview(row,s,ms),'',
 '## Follow the logic','']
 if lesson:lines+=['\n'.join(f'{i}. {v}' for i,v in enumerate(lesson['logic'],1)),'']
 else:
  if not ms:
   lines+=['No method body was found by the lightweight navigation extractor. Read the linked declaration directly: interfaces, field-only classes and wholly commented files require different treatment from executable algorithms.','']
  else:
   priority={'runCommand':100000,'runFunction':100000,'processMessage':50000,'run':40000,'readDataStream':30000,'writeDataStream':29000,'checkTxPoWBasic':28000}
   chosen=sorted(ms,key=lambda x:priority.get(x['name'],0)+len(x['body']),reverse=True)[:2]
   lines+=['Start with the following entry points. The numbered items preserve selected source-order decisions and exits; they are navigation excerpts, not a claim that every branch has been explained.','']
   for m in chosen:
    lines+=['### '+m['name'],'',link(page,source,'Read the complete method',m['line'])+'. '+('This is the command’s main execution path.' if m['name']=='runCommand' else 'Track inputs, state changes, returned values and exceptions separately as you read.'),'',flow(page,source,m),'']
 lines+=['## How the translation is approached','']
 if lesson:lines+=[lesson['native'],'']
 elif native.exists():
  ns=native.read_text();header=native.with_suffix('.hpp');hdr=header.read_text() if header.exists() else ''
  adapters=[]
  if 'shared_ptr' in ns+hdr:adapters.append('Shared pointers occur in the native source/header. Review whether these preserve Java aliases and whether callbacks can extend object lifetime.')
  if 'optional' in ns+hdr:adapters.append('Optional values occur in the native source/header. Check that an absent value fails at the same operation as Java null.')
  if re.search(r'(readDataStream|writeDataStream|serialise|deserialise)',ns):adapters.append('The native code contains stream operations. Verify field order and partially read state, not just a successful round trip.')
  if re.search(r'(mutex|atomic|condition_variable)',ns+hdr):adapters.append('Native synchronization appears in the source/header. Object lifetime and memory visibility need checks independent of single-threaded output comparisons.')
  if 'Services' in ns+hdr or 'Runtime' in ns+hdr:adapters.append('The native code refers to service/runtime boundaries. A controlled test receiver can verify call order while full node wiring remains absent.')
  lines+=['Read '+link(page,native,'the actual C++ body')+(' and '+link(page,header,'its header') if header.exists() else '')+' together. '+(' '.join(adapters) if adapters else 'The method correspondence and all public overloads still need a file-specific translation explanation.'),'']
 else:lines+=['There is no native body at the mapped destination. A future implementation must preserve the source’s inputs, outputs, branch order, exceptions, shared references and external effects. Any validation suggested below is future work, not a passed C++ test.','']
 lines+=['## What the tests tell us','']
 if lesson:
  lines+=[lesson['tests'],'',
   'Existing test path: '+link(page,CPP/'tools/kissvm_differential.py','kissvm_differential.py')+' constructs the cases; '+link(page,CPP/'support/KissVMOracle.java','KissVMOracle.java')+' executes the pinned Java and '+link(page,CPP/'support/kissvm.cpp','kissvm.cpp')+' executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.','',
   'Backfill snapshot: '+link(page,CPP/'build/kissvm-differential-results.json','existing normal report')+' records4,471 comparisons passed and0 failures. This guide did not rerun them. See '+link(page,GUIDE/'evidence/kiss-builtins.md','the shared test lesson')+' for the fixture and the limits.','']
 else:
  refs=references(row)
  if refs:
   lines+=['The following harnesses contain explicit references to this class beyond their import/include lines. This is evidence of a relationship, **not proof that every method or branch is tested**. The exact assertions and fixture reach still need review before assigning file-level coverage.','']
   for p,(n,text) in refs:lines+=['- '+link(page,p,p.name,n)+': `'+text[:230].replace('`','')+'`']
   lines+=['']
  else:lines+=['No direct class-specific exercise was established by this backfill’s harness-reference scan. Indirect integration tests may exist, but they are not counted as evidence here.','']
  lines+=['A useful next validation is to run the same source-derived operation sequence in Java and C++, compare outputs and changed state, then mutate one boundary condition at a time. First identify the actual caller and dependency behavior so that a convenient mock does not hide the rule being ported.','']
 lines+=['## Dependencies and reading map','']
 imports=re.findall(r'^import\s+(org\.minima\.[\w.]+);',s,re.M)
 if imports:
  deps=[]
  for imp in imports:
   target=SOURCE/'src'/Path(*imp.split('.')).with_suffix('.java')
   if target.exists():deps.append(link(page,target,imp.rsplit('.',1)[-1]))
  lines+=['Direct Minima imports: '+', '.join(deps)+'.',''] if deps else []
 if ms:lines+=['Method navigation (declarations with bodies found in this file): '+', '.join(link(page,source,m['name'],m['line']) for m in ms)+'.','']
 lines+=['## Check your understanding','',lesson['exercise'] if lesson else 'Choose one branch above. Write down a normal input, the closest boundary input and a malformed input. Predict the return value, any state already changed and the first operation that can fail. Then compare that prediction with the complete Java body.','',
 '## Limits and traceability','',
 'A corresponding file and a passing finite suite do not establish complete1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. '+('This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.' if lesson else 'This draft needs an authored explanation of its complete algorithm, its native adaptations and the precise assertions reached by its tests.'),'',
 'Pinned Java SHA-256: `'+row['java_sha256']+'`. Mapping: '+link(page,CPP/'source-map.json','source-map.json')+'. Project limitations: '+link(page,CPP/'REVIEW.md','REVIEW.md')+'.','']
 page.write_text('\n'.join(lines))
 return dict(java=row['java'],guide=str(page.relative_to(GUIDE)),port_status=row['status'],parity=row['parity'],teaching_status='authored_builtin' if lesson else 'source_reading_draft',java_sha256=row['java_sha256'])

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--refresh-drafts',action='store_true');a=ap.parse_args();out=[]
 for row in ROWS:
  if not row['java'].startswith('src/'):continue
  page=GUIDE/'files'/Path(row['java']).with_suffix('.md')
  if page.exists():
   # Never overwrite authored pages or paths delegated for concurrent manual review.
   if not a.refresh_drafts or 'Draft walkthrough.' not in page.read_text():continue
   if '/utils/' in row['java'] or Path(row['java']).stem in {'TxPoWChecker','TimedChecker','Transaction','TxPoW','Contract'}:continue
  out.append(build(row))
 (GUIDE/'production-coverage.json').write_text(json.dumps(out,indent=2)+'\n')
 print('Wrote',len(out),'production pages;',sum(x['teaching_status']=='authored_builtin' for x in out),'authored built-in lessons.')
if __name__=='__main__':main()
