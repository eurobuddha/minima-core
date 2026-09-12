# MinimaCoreC++ Preview app

A configurable macOS app around the validated optimized C++ node. Settings cover
network, role, port, an optional manual initial peer, host and data folder.
The manual peer is blank by default, applies only to the explicitly requested
launch, and clears after Start. It is neither saved nor restored from older
settings. Ordinary mainnet starts omit `-connect`, keeping automatic P2P enabled.
A manual `-connect` session disables P2P by the native/Java parameter semantics. The built-in console sends
commands directly to the native process, with no shell or JVM. Core settings
persist in `~/Library/Application Support/MinimaCoreCppPreview/startup.json`;
additional arguments are session-only. Wallet data is outside the application.

This development wrapper reuses the existing Electron desktop implementation;
see REUSE.md and REUSED_DESKTOP_LICENSE. The C++ engine is the cascade crash fix
at node source checkpoint 10ed9e6. Full protocol parity remains unverified.

## Validation

Run `node --test cpp/macos-app/tests/launcher.test.cjs` from the repository root.
`tests/native-lifecycle.cjs <native executable>` exercises fresh/reopen lifecycle.
`tests/window-smoke.cjs` is an Electron test entry point, with an isolated
MINIMA_CPP_APP_TEST_ROOT and MINIMA_CPP_APP_RUNTIME pointing at a verified runtime.
It types a command into the real window, sends it through the preload/main bridge,
checks the actual hash result, stops and reopens. It also delivers an obsolete
Starting event and requires the console to remain enabled. Test hooks are not
included in the packaged app.

The terminal fix gives status snapshots monotonic revisions, ignores older
renderer updates and reads current status after the start request. The console
accepts commands as soon as its child process has an input pipe, including during
startup. The process owns queued input; no readiness or chain-tip claim is inferred
from an enabled command field. Closing the app requests `quit` and waits for node
exit, with bounded signals only for the process this app owns.

The related evidence is in ../validation/cascade-release/app. Existing Java
minimaCore/minimaDesk applications, wallets and credentials are not modified.
