"""Reviewed orchestration lessons; preserve source navigation and pinned hashes."""
from lesson_writer import author,CPP
PREFIX='src/org/minima/system/commands/send/wallet/'
LESSONS={
'createfrom':('Compose an unsigned transaction from a source address using the existing transaction commands.',[
 'Parse source and recipient addresses, amount, token and burn. Reject positive token burns before reading script.',
 'Generate a 32-byte random transaction ID and issue txncreate. Preserve the literal space after fromaddress: in the txnaddamount text.',
 'Read the funding reply status by Boolean cast/unboxing. On false, delete the row and throw the child error, which may be null.',
 'Call txnscript, txnmmr, txnexport showtxn:true, then txndelete. These child statuses are not checked.',
 'Return export.response. Exceptions before the final delete can leave a row; no finally block repairs that source behavior.'
 ],'Why can an empty exported transaction have status:true in the pinned offline experiment?'),
'constructfrom':('Compose an unsigned transaction from a comma-separated list of coin IDs and explicit recipient/change amounts.',[
 'Parse coinlist and script, recipient address/amount, then changeamount before changeaddress. Parse tokenid as a string.',
 'Generate a 32-byte ID and create the row. StringTokenizer discards empty comma tokens; trim each remaining token with Java’s <= U+0020 rule.',
 'Issue one txninput per token. A false input status deletes then throws; a missing or wrongly typed status throws before cleanup.',
 'Issue the recipient output and add change only for a positive change amount. Output statuses are ignored.',
 'Add script, request proofs, export with showtxn:true, delete and return export.response. Empty coinlist is rejected by Command; commas-only input reaches this body with zero tokens.'
 ],'How do an empty string, three commas, and a comma-separated whitespace token differ?'),
'postfrom':('Import a serialized custom transaction, optionally add proofs, post it and delete the temporary row.',[
 'Fetch the custom database before parsing data. Decode a TxnRow and optionally override its ID; id is absent from the public valid-parameter list.',
 'Parse mmr before dereferencing the decoded row and inserting it. A wrong parameter type here leaves the database unchanged.',
 'Insert the row, replacing an existing matching ID, then parse mine. A failure at this later stage leaves the imported row.',
 'Optionally run txnmmr, then txnpost with the requested mine value, then txndelete. Child status values are not checked.',
 'Return the posting child’s response, including null. A successful outer status is not independent proof that posting succeeded.'
 ],'Which of mmr and mine is parsed before insertion, and why does that change cleanup evidence?'),
'signfrom':('Import a custom transaction, sign with a supplied key, optionally post, export and delete it.',[
 'Fetch the database, decode data, optionally replace ID and insert the row before parsing privatekey, keyuses and post.',
 'Invoke txnsign publickey:custom. With post:true append txnpostauto:true and txnpostmine:true exactly as the source does.',
 'Export without showtxn:true, then delete. Neither signing nor export status is checked before these next steps.',
 'Cast export.response to an object and add post after deletion. Missing or wrongly typed export responses fail at this point.',
 'For post:true, try to copy txpow from the signing response. Only exceptions inside this small extraction block are logged and swallowed.'
 ],'Why can a key parsing failure retain the row while an export response cast failure happens after deletion?')}
for name,(purpose,steps,exercise) in LESSONS.items():
 author(PREFIX+name+'.md',purpose,steps,
  'The native wrapper reuses Command, CommandRunner, MiniData/MiniNumber and the actual TxnDB/TxnRow and transaction commands. Its public string overload remains virtual for source-like subclassing. CommandResult shares the exact first-result selection, typed Boolean unboxing and nullable rejection logic. Java’s checked exceptions have no C++ compile-time equivalent. The method sequence and intentionally unescaped command strings remain source-specific.',
  lambda link:link(CPP/'tools/wallet_wrappers_differential.py','Read the Java/native comparison')+' and '+link(CPP/'student-guide/new-tests/wallet-command-orchestration.md','the student lab')+' compare metadata, parameters, exact child-call text/order, database snapshots, cleanup and errors. Controlled child results isolate orchestration; real children also construct/export rows, exercise ignored failures, sign and cryptographically verify a decoded signature. Random IDs are checked and consistently renamed; only verified serialized ID bytes are normalized. Both normal and ASAN/UBSAN builds pass 659 programs and 2,060 child calls (14.29s/58.31s). All five selected checks pass per build, including the command, runner, runner-error and source-mapping regressions.',
  exercise,
  'The counterpart is implemented_partial; parity remains not_verified. No live node or durable SQL runs in this fixture. Actual posting success and confirmation, funded chains, arbitrary Java null/reference APIs and all concurrent interleavings remain unfinished or separately scoped. NPE/CCE runtime prose is normalized; explicit errors and observed logs are compared.')
