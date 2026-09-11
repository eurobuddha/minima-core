# Turning a build into a runnable snapshot

A compiled executable can work on its developer's computer because it refers to
libraries at absolute paths inside a build folder or Homebrew installation. Copying
only that executable would leave those dependencies behind. Rebuilding those
libraries could also change the behavior of someone's supposedly frozen snapshot.

The preview packager reuses the existing Minima entry point and native H2 provider.
It does not create another node implementation. It reads each Mach-O dependency,
recursively copies the non-system libraries, and replaces their references with
`@loader_path` paths inside the package. macOS system libraries remain supplied by
the operating system. Changing a Mach-O file invalidates its signature, so the
packager re-signs and verifies the copies after editing their load commands.

The small Terminal launcher resolves its own directory, quotes paths containing
spaces, and starts the node with a separate data folder, port 21001 and the user's
chosen initial peer, eurobuddha.com:9001. It uses a restrictive file-creation mask
for its new private data. Further development rebuilds do not replace this copy.
The ZIP is created before any node runs there, so it contains no wallet or chain.

## What the verification proves

The verifier extracts the actual ZIP into a different folder containing spaces.
It compares every file with the manifest hash and checks executable permissions.
A system-only PATH excludes development tools. The real launcher must load and
print help. Java's ParamConfigurer intentionally exits 1 after help; the first
packaging test wrongly expected 0. Reading that source corrected the test without
changing the executable or ignoring arbitrary failures.

Next, the launcher runs against the real peer. The hash command must return the
same fixed SHA3 value used in the existing startup tests. Ping supplies the peer's
tip height and hash; local status must match both, use the isolated data/port, and
show a valid peer connection. A clean quit must save the database files. Starting
the same extracted preview again must load its chain and match the moving peer.
The JSON-reply extractor is reused directly from native_node_startup.py.

These checks answer a packaging question: does the artifact actually run outside
the build directory? They also exercise one mainnet lifecycle. They do not prove
every command, thread interleaving or consensus rule. The original test corpus
remains necessary, and the source-map completion status does not change.

See [preview instructions](../../PREVIEW.md) and
[recorded results](../../validation/executable-preview-work-in-progress/progress.json).

Exercise: why does a ZIP checksum help identify the version being tested but not
prove that its implementation is correct? Why does the test compare a block hash
as well as its height?

## Results and corrections

Port 17001 attempts did not reach a chain connection within the verification
window; the cause is not established. At the user's request the launcher now
uses 21001. Its fresh start downloaded and advanced to 2,309,598 and saved cleanly.
The initial exact-tip test held one old peer sample at 2,309,596 while polling
missed that intermediate local height; this is a test-sampling failure, preserved
in the evidence. The corrected verifier refreshes peer samples every 15 seconds
and resolves macOS /var and /private/var path aliases.

The explicitly resumed restart passed, matching both height and hash at 2,309,602,
checking the fixed SHA3 result and saving cleanly. It is labelled resumed-reopen,
not reported as a second fresh-start test. The separate validation wallet/chain
was then moved into the local user preview after shutdown. The ZIP stays free
of wallet data. No mapped C++ source behavior changed in this packaging task.
