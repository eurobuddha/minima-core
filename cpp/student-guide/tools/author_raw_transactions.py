"""Source-specific raw transaction command lessons, using the shared lesson editor."""
from lesson_writer import author,CPP
for name in ['rawfrom','rawtxnfrom']:
 steps=[
 'Parse inputs and outputs as arrays'+(' and require the scripts array' if name=='rawtxnfrom' else '')+', then optional state as an object. Wrong types fail here; null arrays remain null until their loops.',
 'Generate a random 32-byte ID and issue txncreate before entering any input loop.',
 ('For each input object, cast coinid and script to String, then issue txninput followed immediately by txnscript. A null field becomes the literal text null during concatenation.' if name=='rawfrom' else 'For each input, cast to String and issue txninput. Complete all inputs before proceeding to outputs. Null input elements concatenate as the text null.'),
 'For each output object, read address then amount, optional tokenid (default 0x00), then optional Boolean storestate (default true). Null Boolean unboxing fails; the string true is not a Boolean. Issue txnoutput.',
 ]
 if name=='rawtxnfrom':steps+=['After all outputs, cast each script to String and issue txnscript. This differs from rawfrom’s alternating input/script sequence.']
 steps += [
 'Traverse state keys in LinkedHashMap insertion order. Parse the integer port first, convert the value as Java object-to-string concatenation, construct StateVariable, and issue txnstate with normalized port/data.',
 'Export with showtxn:true, delete, and return export.response. No child status is checked, no MMR command runs, and no signing occurs. A thrown exception can skip the final deletion.'
 ]
 author('src/org/minima/system/commands/send/wallet/'+name+'.md',
  'Assemble an unsigned custom transaction from '+('input objects carrying individual scripts' if name=='rawfrom' else 'separate input-ID and script arrays')+', output objects and optional state.',steps,
  'The native wrapper calls the existing CommandRunner through CommandResult.first. RawTxnCommands shares only the identical output and state loops. StateText is extracted unchanged from the proven send.cpp conversion: strings keep their text, doubles use Java formatting, and other JSON values use existing JSON text. JSONObject’s native insertion order matches the pinned LinkedHashMap. Typed casts preserve null-versus-wrong-type failure categories without substituting transaction construction logic.',
  lambda link:link(CPP/'tools/raw_transaction_corpus.py','Read the raw corpus')+', '+link(CPP/'tools/wallet_wrappers_differential.py','the shared Java/native driver')+' and '+link(CPP/'student-guide/new-tests/raw-transaction-commands.md','the lab')+' compare exact child-call order/text, row bytes, metadata, failures and cleanup. Real children construct and export rows, including output storestate flags and normalized state. Independent assertions distinguish alternating versus separated script order, duplicate port normalization, retained rows on throws, and ignored input failures. The final raw group passes 656 programs and 3,991 child calls in normal and ASAN/UBSAN builds (4.06s/4.55s). After the Unicode Boolean correction, the object regression passes 9,464 cases per build (25.10s/91.34s). The earlier seven-check batches also pass, including funding and wallet-wrapper regressions.',
  'Where does a null array fail relative to txncreate, and how does that differ from passing a JSON object where an array is required?',
  'The counterpart is implemented_partial and parity remains not_verified. Tests normalize only checked generated IDs and runtime-specific NPE/CCE prose; other observed bytes/replies/logs are compared. Exporting an unsigned row does not prove a funded or accepted transfer. Native Main, durable SQL, complete nullable/reference APIs and full node integration remain unfinished.')
