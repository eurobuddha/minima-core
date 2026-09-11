# Running the local executable preview

The frozen Apple Silicon preview is in
`preview-builds/MinimaCoreC++-6c28109-port21001-macos-arm64/`.
Double-click **Start MinimaCoreC++.command** in that folder to open Terminal.
The ZIP beside it contains the same package, without a wallet or chain database.
The prepared local folder includes the separate validation wallet and chain,
which reopened and matched the peer at block 2,309,602.

This is a macOS 26+ arm64 console build, with its native libraries included. It does
not need Java, Homebrew or the development build directory at runtime. Keep the
whole folder together. It is a fixed snapshot; rebuilding the port does not change
this executable.

The launcher connects to **eurobuddha.com:9001** and uses **port 21001**, RPC disabled,
and its own `data/` directory inside the preview folder. My development node runs
separately on port 16001. Start one instance of a preview folder at a time. Initial
wallet creation and synchronization take time; later starts reuse the saved data.

Try these commands, one at a time:

```text
help
status
network
ping host:eurobuddha.com:9001
hash data:hello
help command:txpow
quit
```

Use `quit` and wait for the shutdown-completed message before closing Terminal.
This is the minima-core command console; it does not include the MiniDapp/MDS web
interface. Full parity is still unfinished. Use the separate preview wallet for
experiments; keep existing wallet data separate.

## Rebuilding another frozen preview

Build the existing node target first, and checkpoint changes to its sources. The
packager refuses an existing destination, so it cannot replace a preview or its
wallet while that preview is being used.

```sh
cmake --build cpp/build --target MinimaCoreCpp -j4
python3 cpp/tools/package_macos_preview.py --output cpp/preview-builds/MinimaCoreC++-next-macos-arm64
```

The packager follows all non-system dynamic-library dependencies, bundles them,
changes references to paths relative to the executable/library, removes build
RPATHs and applies/verifies local ad-hoc signatures. It includes licenses, source
identity, dependency information, a per-file SHA-256 manifest and a ZIP checksum.
The H2/MySQL database dependency is compiled from Java to a native library with
GraalVM; the node itself is C++ and there is no JVM fallback.

`verify_macos_preview.py` extracts the actual ZIP into a different folder with
spaces in its name, checks every manifest file and executable permission, and uses
a system-only PATH. It exercises the actual launcher on port 21001, checks its
hash command and live peer height/hash, quits, then reopens the saved mainnet chain.
Run that verification only when your preview is not using port 21001:

```sh
python3 cpp/tools/verify_macos_preview.py cpp/preview-builds/MinimaCoreC++-next-macos-arm64.zip --report /tmp/minima-preview-verification.json
```

The package is for local testing on this Mac, not a notarized public release.
The packaging test covers distribution and startup/restart behavior; it does not
replace the original Java/C++ parity tests.
