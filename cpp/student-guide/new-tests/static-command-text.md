# Preserving text commands without changing their content

Three pinned files return documentation: `commands/tutorial.java`,
`commands/base/tutorial.java`, and `commands/whitepaper.java`. The two tutorials
have the same command name but different packages and text. Only the first is
registered by the pinned command runner. The native classes preserve that
separation and copy their own string literals, including whitespace and escapes.

A tutorial returns a string in `response`. The white-paper command instead returns
an object containing `whitepaper`. Its public `WP` constant is preserved. All three
reuse Command's metadata, parameter object, reply fields and fresh-instance logic.
Direct calls ignore parameters; the runner rejects parameters because the valid
parameter list is empty. These are different paths and both matter.

Before running the tests, we will compare the actual pinned Java and native
classes, their metadata, return shapes, serialized text and public constant. The
driver also independently decodes the source literal expressions and checks
payload lengths and hashes. It checks which tutorial the real runner selects,
ordinary/non-admin callers, invalid parameters, and parameter aliasing in replies.

This is a text-preservation test. It does not claim that every statement in the
historical white paper is implemented or that every tutorial example is current.
Changing a typo or silently combining the two tutorials would fail fidelity.

Driver: `cpp/tools/text_commands_differential.py`. Both complete builds and both suites pass: 71 cases, 428 independent assertions,
normal 2.618 seconds and sanitizer 4.137 seconds. The transcript hash is identical.
Payload sizes are 6,825 bytes for the registered tutorial, 6,733 for the older
base tutorial, and 43,554 for the white paper. Native stderr was empty.
