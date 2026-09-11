# Reuse evidence

Read pinned/native Maths, BaseConverter, MiniFormat, POW and REPLACE code, their
headers, and complete demonstration bodies. MiniFormat's 9954-byte embedded literal
is decoded directly from the pinned Java string constants, copied intact and compared
back to source by the test. MiniData's String constructor was inspected: it calls
decode16, so BaseConverter.main must retain its Mx1 NumberFormatException. MiniNumber
pow/checkLimits establishes the full demonstration's zero results at scale 44.

Read existing utility and small-utilities fixtures/drivers, POW original test fixture,
KISS replacement cases, JSONObject/MinimaLogger interfaces and JavaStringTrim helper.
Reuse current arithmetic, conversion, replacement, parser, formatter and logger.
Search of /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src found no
compareVersions/JSONPretty/encode32/decode32/safeReplaceAll alternative. Existing
AesUtil main and NIOServer main supply output/entry-point conventions. No new library
or substitute arithmetic/JSON implementation was introduced.
