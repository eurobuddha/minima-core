# Initial connection settings: a value should not outlive its purpose

The preview saved an initial peer and supplied `-connect` on every launch.
Both Java's `ParamConfigurer` and the C++ `ParamConfigurerConsumers.inc`
deliberately disable automatic P2P when that argument is present. Consequently,
a healthy direct connection could coexist with `peers` reporting P2P disabled.
The error was in the application configuration lifetime, not the command parser.

The correction reuses the preview's existing settings loader, atomic save and
start handler. The peer defaults to blank; loading old settings overrides any
stored peer with blank. Saving omits it, and a successful start clears the visible
field. An explicitly entered manual peer still applies to that single launch.
Normal mainnet starts therefore omit `-connect`; solo mode retains its existing
isolation flags. We do not change the Java-to-C++ parameter semantics.

The launcher test checks mainnet argument lists with and without an explicit
manual peer. The real-window regression begins with an old saved peer, verifies
that loading discards it, enters a fixture peer, starts a temporary solo node,
and checks that neither the saved settings nor the next settings load retain it.
It repeats the lifecycle and types an actual hash command through the console.
Solo mode keeps this GUI test independent of external peers; it does not prove
live mainnet connectivity, initial synchronization, or complete protocol parity.

See [the app source and test instructions](../../macos-app/README.md) and
[recorded results](../../validation/cascade-release/app/peer-once/window-results.json).
No mapped Java file was newly converted by this application change.
