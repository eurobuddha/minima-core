# Reused desktop implementation

The source catalogue was the existing `desktop/minimacore-desktop` project under
`/Users/eurobuddha/Projects/minima`, plus this port's package_macos_preview.py,
package_project_delivery.py, native_node_startup.py and verify_macos_preview.py.
The installed minimaCore.app was inspected to locate its packaged Electron runtime.

The adapted node-manager retains the EventEmitter process owner, bounded log ring,
quote-aware argv tokenization, spawn/exit state updates and bounded graceful-stop
pattern from main/node-manager.js. The process is the bundled C++ executable;
stdin `quit` replaces authenticated RPC quit, and stdin/stdout provide the console.
Java/Parlons selection, heap flags, RPC health polling and router port mapping do
not fit this native console's protocol and are omitted. A busy port is reported;
this app does not reclaim another process merely because its command line matches.

BrowserWindow sandbox/context isolation, explicit preload IPC methods, the
single-instance lock and awaited before-quit lifecycle come from main/main.js and
main/preload.js. The existing renderer params manifest and wizard were inspected;
they depend on the separate wallet/Parlons onboarding and another node version.
The smaller startup/console screen uses the native binary's parameter list instead.
The existing minima-mark.svg and MIT license are retained.

Configuration uses a distinct app identity and Application Support directory.
Only non-secret core settings persist; additional arguments are session-only.
Arguments go directly to spawn with shell disabled. No wallet data is bundled.

Signing/notarization follows the existing scripts/notarize-dmg.sh and verify-mac.sh
sequence (Developer ID, hardened runtime, notarytool, stapler and Gatekeeper).
The native runtime relocation, dependency licenses and hashes reuse the existing
port packager unchanged. Electron is the existing desktop wrapper dependency;
the Minima node remains C++ and has no JVM runtime.
