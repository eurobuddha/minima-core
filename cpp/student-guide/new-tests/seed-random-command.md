# Lab: deriving a deterministic value from a fixture seed

Written before the new tests. Read base/seedrandom.java and follow its calls into
Wallet, SeedRow, MiniString, MiniData and Crypto. This command uses no fresh random
bytes. The same seed and modifier produce the same result.

First check isBaseSeedAvailable. Only the exact seed text 0x00 means locked;
missing/null seed state can instead throw. This check precedes required modifier
parsing, so a locked wallet must report DB locked! even for a missing modifier.

Command.getParam trims Java String boundary characters at or below U+0020. Keep
the resulting UTF-16 modifier for the response. MiniString uses Java’s UTF-8
encoding, whose isolated-surrogate replacement is the byte for '?'. These two
representations can differ; the reply must not lose original UTF-16 code units.

For UTF-8 bytes M and seed bytes S, define B(X) as a four-byte big-endian length
followed by X. MiniString wire is B(M). Wrapping that wire in MiniData means the
first hash consumes B(B(M)) followed by B(DE AD DE AD). Call that SHA3-256 result
H. The second hash consumes B(H) followed by B(S). Reuse Crypto.hashObjects for
both hashes. Hashing plain seed text plus plain modifier text is incompatible.

Use only synthetic fixture seeds, through the existing recording wallet fixture.
Compare actual command replies against Java and independently compute expected
hashes with Python hashlib.sha3_256. Test repeated calls, changed modifiers,
whitespace, Unicode, embedded zero bytes, variable seed lengths, seed text aliases,
malformed/null seed fields, missing/wrong modifier types, locked-check precedence,
failed wallet initialization/update and public runner permission handling.
Compare all recorded SQL events. A successful derivation should read the in-memory
SeedRow and issue no SQL calls itself. Earlier wallet loading or updates still
produce their actual SQL effects. No real wallet or private user seed is accessed.
These finite tests establish byte compatibility for their inputs, not a new
cryptographic proof or complete runtime/concurrent-wallet parity.

Final evidence: both full builds pass without observed warnings. All 309 programs, 782 operations and 14,616 SQL observations match Java (12.30s normal, 5.50s sanitizer). All four selected regressions pass (23.31s/75.25s): wallet commands, command parameters, runner and node-state commands. The command reads synthetic fixture seeds only. Isolated-surrogate reply observations cross Java stdout’s UTF-8 replacement boundary; hashes are independently checked with those encoder replacement bytes.
