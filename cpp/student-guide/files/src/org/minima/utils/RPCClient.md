# RPCClient — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/RPCClient.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/RPCClient.cpp) · [C++ header](../../../../../../src/org/minima/utils/RPCClient.hpp)

## Purpose

RPCClient sends synchronous HTTP requests and returns response text. It wraps Java URLConnection behavior: request headers, redirects, TLS setup, line decoding, and status handling all influence the observable result.

## Follow the algorithm

1. GET/POST convenience overloads choose HTTPS based on a case-sensitive startsWith("https") test. Individual methods construct a URL, cast its connection to HTTP or HTTPS, set a 10-second connect timeout, the verb, User-Agent, and Connection: close.
2. Basic auth is added only when password is nonempty. It Base64-encodes user:password; a null user participates in Java string concatenation, while a null password fails at equals. sendGETAuth instead prefixes the supplied token with Basic directly. Explicit SSL methods obtain a socket factory from the supplied SSLContext; sendGETSSL supplies null and therefore fails there.
3. POST sets output mode, optionally sets Content-Type, obtains an output stream, writes UTF-8 parameters, flushes/closes, and then reads the response code. Obtaining that stream can connect before a null parameter is dereferenced.
4. Only status 200 returns response content. BufferedReader.readLine removes line endings before concatenation. Non-200 GET/PUT print a diagnostic and return empty text; POST raises IOException containing the URL and parameters.
5. Redirects, TLS verification, default headers, and transport errors are partly inherited from URLConnection. Their behavior must be observed against the pinned runtime rather than guessed from the short wrapper methods.

## How the C++ translation works

C++ uses the shared libcurl transport but explicitly emulates observed Java request semantics: preserves raw targets, snapshots mutable USER_AGENT, strips URL user-info auth, controls Basic headers, follows the observed redirect status rules, changes POST to GET where required, and strips authorization when destinations change. It maps transport failures to Java-category exceptions, removes CR/LF from decoded success text, and caps redirects at 20. SSLContext is a native callback configuring the connection’s trust/key settings, not a Java provider object.

## Understanding the tests

The driver runs controlled HTTP and TLS loopback servers and compares both method results and every recorded method/path/header/body. Its cross-product covers verbs and status codes, same-host/cross-host/cross-protocol redirects, loops, dropped connections, chunked/truncated responses, malformed UTF-8, auth/null values, URL fragments, empty queries, user-info, and illegal headers. TLS cases require normal rejection or successful configured-CA verification as appropriate. Selected provider/VM diagnostic wording is normalized; request values remain compared. This is stronger than checking only the final returned string.

- [Java RPC fixture](../../../../../../support/RPCClientOracle.java)
- [Native RPC fixture](../../../../../../support/rpc_client.cpp)
- [HTTP/TLS request-trace comparison driver](../../../../../../tools/rpc_client_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Global JDK proxy/authenticator/cookie/cache settings and every SSL provider are outside the fixture. The code models a specific observed runtime surface and does not establish complete URLConnection equivalence. A useful hand-traced case is authenticated POST→302 to another host: predict verb, body, auth, and headers at the second server before inspecting the recorded trace.

## Check your understanding

Why is non-200 POST behavior different from non-200 GET? Why does preserving a null-parameter connection attempt require more than checking arguments at the start of the native method?

Pinned Java SHA-256: `2c9afeea698f229657c0f518bedc63d343a86745c999d774e35e3dec19eff436`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
