from lesson_writer import author,CPP

def w(name,purpose,steps,adapt,detail,exercise,limits):
 def tests(l):return detail+' The '+l(CPP/'support/WalletRowsOracle.java','Java row oracle')+' uses a ResultSet proxy that logs each getter and can throw SQLException at a chosen call. The '+l(CPP/'support/wallet_rows.cpp','native row harness')+' supplies the corresponding SqlRow callbacks. The '+l(CPP/'tools/wallet_rows_differential.py','driver')+' compares direct and SQL constructors, getter results, JSON and failure order. Its inspected report has 1,710 matches across all three row types, with zero failures. SQL driver conversion and persistence are explicitly excluded.'
 author('src/org/minima/database/wallet/'+name+'.md',purpose,steps,adapt,tests,exercise,limits+' The source-map counterpart is partial and parity not_verified.')
w('SeedRow','A two-field wallet record containing a seed phrase and its seed representation.',[
 'The ResultSet constructor reads phrase first, then seed using getString. Any SQL failure stops construction at that point.',
 'The direct constructor stores the two supplied strings, including null. It does not validate the mnemonic, derive a seed or check correspondence between fields.',
 'Getters expose each value. toJSON includes both phrase and seed without redaction; that differs from KeyRow’s JSON omission of its private key.'
], 'C++ uses optional<string> for Java nullable strings and an abstract SqlRow for column access. JSON preserves null explicitly. This is a data adapter, not a BIP39 implementation or a native database driver.',
 'The seeded fixture varies null, empty, ordinary and Unicode strings through both constructors. Injected failures after the first or second SQL call verify field-read order; later configured failure numbers are not reached by this two-column constructor.',
 'Can a SeedRow contain a phrase and seed that do not correspond? Which component, outside this class, must establish that relation?',
 'This class intentionally exposes its stored fields through JSON. Persistence, mnemonic validation and application decisions about displaying seed material remain outside these row tests.')
w('KeyRow','Stores a tree key’s parameters, usage counters, modifier and private/public key strings as a wallet database row.',[
 'Read size, depth, uses and maxuses as SQL ints, then modifier, privatekey and publickey as strings. Preserve that sequence because a failing getter stops construction.',
 'The direct constructor simply stores its arguments. It does not enforce uses <= maxuses, a valid depth or consistency between private and public key.',
 'Getters include access to the private key. JSON emits size/depth/uses/maxuses/modifier/publickey; the privatekey insertion is commented out and therefore absent from JSON.',
 'Usage counters are stored facts, not an atomic allocation protocol. This record alone does not reserve a one-time signing key or persist its next use.'
], 'Native int32_t matches Java int width and optional strings represent null. The SqlRow interface delegates JDBC-equivalent conversion to the eventual backend. Native public fields remain mutable, as in Java.',
 'Inputs include null strings, integer extrema, negative counters and Unicode. Both constructor routes are compared, all seven SQL failure positions are injected, and the harness separately captures private-key getter output and public JSON, proving the deliberate omission rather than merely checking successful JSON serialization.',
 'Why does absence of privatekey from JSON not imply that a KeyRow cannot reveal its private key? Identify the other public access paths.',
 'No validity or safe key-use policy is established by this permissive record. Database transaction semantics, crash recovery and durable counter advancement require Wallet/backend review.')
w('ScriptRow','Stores a wallet script, its address, associated public key and simple/default/track flags, and derives the human-readable address for JSON.',[
 'The SQL constructor reads script/address strings, simple and defaultaddress ints, publickey string, then track int. Every nonzero integer, including a negative value, becomes true.',
 'The direct constructor stores supplied strings and booleans without checking that script and address correspond.',
 'Getters return those values. toJSON also parses address as MiniData and generates an Mx address each time it is called.',
 'Consequently a row can construct successfully with an invalid or null address and fail later during JSON conversion. Null script or public key is independently permitted and can serialize as JSON null.',
 'The JSON key for the default flag is default, whereas the SQL column name is defaultaddress. Matching those names at the correct boundary is part of the translation.'
], 'Native optional strings retain null and !=0 reproduces Java integer booleans. Mx generation reuses the mapped Address and MiniData implementations. A null address reaches optional.value failure in C++, which is not the same exception type as Java’s null path.',
 'The fixture varies invalid/empty/hex/Mx-invalid addresses, null strings and integer boolean extrema. It observes delayed JSON failures and SQL getter order, distinguishing row construction from address formatting. Generic ERROR matching does not prove exact invalid-address/null error classes.',
 'What happens for SQL simple=-1? Why does a successfully constructed ScriptRow not guarantee that calling toJSON will succeed?',
 'The record performs no script ownership or address-consistency validation. Real JDBC conversions, database storage and all exact failure messages are outside the controlled row fixture.')
