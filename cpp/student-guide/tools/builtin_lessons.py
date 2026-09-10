"""Manually authored after reading the pinned Java/native built-ins and KISS harnesses."""
# Each entry: purpose; algorithm; native adaptation; focused existing cases; student exercise.
LESSONS = {}
def add(name,purpose,logic,native,tests,exercise):
 LESSONS[name]=dict(purpose=purpose,logic=logic,native=native,tests=tests,exercise=exercise)
add('ASCII','Turn a hexadecimal byte sequence into an ASCII string.',
 ['Require one argument and ask Contract to evaluate it as HEX.','Decode each byte using Java ASCII: bytes outside the 0–127 range become the replacement character.','Wrap the decoded text in StringValue; its string-size and encoding rules still apply.'],
 'The C++ loop appends the byte as a UTF-16 code unit below 128 and U+FFFD otherwise. It then reuses StringValue.',
 'The driver supplies 0x00, 0x80, 0xFFFF, empty/string/number/boolean inputs, and wrong argument counts. The harness compares value type and text as well as instruction count and errors.',
 'Predict how ASCII(0xC3A9) differs from UTF8(0xC3A9), and trace where replacement occurs.')
add('UTF8','Decode hexadecimal bytes as UTF-8 text.',
 ['Require one HEX argument.','Decode its raw bytes with the UTF-8 decoder, including replacement of malformed sequences.','Construct a StringValue from the decoded text.'],
 'C++ reuses MiniString’s byte-span decoder rather than interpreting a byte as a character. UTF-16 storage is supplied by StringValue.',
 'The shared unary-function cases include 0x80, 0xFFFF, large HEX integers and wrong types/counts. These selected cases check decoder interaction; they are not an exhaustive proof of every malformed UTF-8 sequence.',
 'Explain why one visible emoji occupies four UTF-8 bytes but two UTF-16 code units.')
add('BOOL','Convert any supported KISS value into a Boolean.',
 ['Evaluate the single expression before deciding its type.','Booleans keep their value; numbers and hexadecimal integers are false only when numerically zero.','Strings are false only when their exact text is FALSE. In particular an empty string and lowercase false are true.','Reject unsupported value types and return a BooleanValue.'],
 'A C++ switch replaces Java instanceof-style casts while retaining the four separate conversion rules.',
 'The driver explicitly includes TRUE/FALSE, 0, nonzero numbers, [FALSE], [false], [], 0x00 and 0xFFFF, then compares typed results and failures.',
 'Why would using C++ string.empty() as this conversion produce a consensus difference?')
add('HEX','Convert a Boolean, number, string or HEX value into bytes.',
 ['Require one argument, evaluate it and branch on its KISS type.','Map booleans to the single byte 00 or01; preserve HEX bytes.','For numbers, reject fractions and negative values before converting the whole nonnegative integer.','For strings, use the string’s encoded bytes; return HexValue.'],
 'C++ keeps MiniData/BigInt conversion and MiniString encoding instead of using a machine-width integer or a display string for all types.',
 'The unary matrix includes fractions, negatives, values beyond 64 bits, booleans, strings and HEX. It also tests missing and excess arguments.',
 'Predict HEX([0xFF]) and explain why it differs from HEX(0xFF).')
add('NUMBER','Convert a supported KISS value into Minima decimal arithmetic.',
 ['Evaluate one expression.','Map Boolean true/false to 1/0 and interpret HEX as an unsigned integer.','Parse string contents through NumberValue’s decimal parser, or copy an existing number.','Reject other type tags.'],
 'The native result wraps MiniNumber; it does not use double, so the existing decimal limits and wire representation remain relevant.',
 'The unary matrix includes nonnumeric strings, very large HEX, fractions, TRUE/FALSE and wrong arities; the harness also compares serialized MiniNumber bytes.',
 'Why is numeric equality alone weaker than comparing the decimal wire bytes?')
add('STRING','Obtain the KISS textual representation of a value.',
 ['Require one argument.','Evaluate the expression and call its textual representation.','Construct a StringValue around that representation; this is distinct from decoding HEX bytes.'],
 'C++ copies an existing StringValue directly and otherwise constructs one from toString().',
 'The shared unary matrix includes every value category and Unicode string [🐼]. Wrong arity and unknown-variable paths are compared too.',
 'Contrast STRING(0x4142) with ASCII(0x4142).')
add('ADDRESS','Calculate the address associated with a script string.',
 ['Require exactly one string argument.','Construct the shared Address object from that script text.','Return its address bytes as HEX.'],
 'Both versions delegate address derivation to Address. The C++ built-in is a thin wrapper and does not introduce a second hashing algorithm.',
 'The unary matrix supplies strings, Unicode, empty strings and incorrect types. Output values are compared with Java through the real expression evaluator.',
 'Where would you inspect the serialization and hash domain if two address outputs differed?')
for name,missing in [('GET','throw an execution exception naming the missing key'),('EXISTS','return FALSE')]:
 add(name,'Look up an array-style variable using a list of numeric indices.',
 ['Require at least one parameter and evaluate the indices in their original order.','Require every index to be NUMBER; append its trimmed textual form followed by a comma to a single key.','Look up that key in Contract’s variable table. If absent, '+missing+'.','If present, '+('return the stored Value itself.' if name=='GET' else 'return TRUE.')],
 'C++ builds the same comma-terminated key and reuses Contract’s variable table. GET retains the shared result value; EXISTS returns a Boolean.',
 'The driver writes LET (1 2)=7 and reads GET(1 2), checks EXISTS on missing indices, compares1.0 versus 1 key spelling, replaces a prior value, and supplies wrong index types.',
 'Write out the exact key for indices 1 and2. Why is evaluating index expressions left-to-right observable?')
add('FUNCTION','Execute a supplied script after textual argument substitution.',
 ['Require at least the script string. Count dollar-delimited nonempty tokens and enforce the source replacement bound.','Evaluate extra arguments sequentially and replace $1, $2, … in order. String arguments receive square brackets; repeat the replacement bound check each time.','Remove the old returnvalue variable, tokenize/parse the expanded script at the current stack depth and run it in the same Contract.','Rethrow execution errors, wrap other caught failures as execution errors, and return returnvalue if the executed script defined it; otherwise return TRUE.'],
 'The native implementation reuses the tokenizer, statement parser and REPLACE helper. A small loop reproduces StringTokenizer’s nonempty-token count; this is not a simple count of dollar characters.',
 'The driver tests argument replacement, string-valued parameters, missing returnvalue, shared variable mutation, scripts containing RETURN and malformed nested scripts. Contract state, instruction counts and traces are compared.',
 'Why can replacement of $1 affect a later $2 replacement? Trace a supplied string containing a dollar placeholder.')
add('BITCOUNT','Count the set bits in a byte sequence.',
 ['Evaluate exactly one HEX argument.','Visit every byte and count its one bits. Java masks with 0xFF before looking up a256-entry table.','Sum the counts and return a NumberValue.'],
 'C++ uses std::popcount on uint8_t values and preserves the signed32-bit total through an explicit bit conversion.',
 'The unary matrix includes 0x00, 0x80 and 0xFFFF (zero, one and sixteen set bits), plus invalid types/counts.',
 'Why must Java mask a signed byte before using it as an array index?')
for name in ['BITGET','BITSET']:
 add(name,('Read' if name=='BITGET' else 'Change')+' one bit using Java BitSet byte ordering.',
 ['Require '+('two' if name=='BITGET' else 'three')+' arguments. Evaluate HEX data and convert the index to a Java-style int.','Reject an index below 0 or above byteLength×8−1. Bit 0 is the least significant bit of the first byte.',('Return a Boolean indicating whether that bit is set.' if name=='BITGET' else 'Evaluate the Boolean setting only after index validation, then set or clear the bit. Remove trailing zero bytes when converting the BitSet back to HEX.')],
 'C++ uses byte index bit/8 and mask 1<<(bit%8). '+('The return value is BooleanValue.' if name=='BITGET' else 'A loop removes zero bytes at the back to match BitSet.toByteArray; retaining the original length would be wrong.'),
 'The driver combines several byte orders (0x0001 versus 0x0100), empty HEX, indices −1/0/7/8/15/16/4294967296, and, for BITSET, TRUE/FALSE/invalid numeric flags. Values, errors and traces are compared.',
 'Calculate '+('BITGET(0x0001 8).' if name=='BITGET' else 'BITSET(0x0100 0 FALSE). Why may the resulting length shrink to zero?'))
add('CONCAT','Join two or more HEX values into one byte sequence.',
 ['Require at least two arguments.','Evaluate and type-check each expression in order while adding its byte length to the total. Reject a total above the contract data limit.','Copy the sequences in the same order and return HEX.'],
 'Java first collects arrays then copies into a final array. C++ appends into a vector while applying the same per-argument type and size checks. Allocation/resource-failure equivalence is not established.',
 'Explicit cases join0xFF/0x00/0x11, reject strings, and reject a single argument. Other size cases exercise related HEX limits but are not claimed as exhaustive CONCAT allocation tests.',
 'Why does CONCAT require at least two arguments rather than accepting an empty list? Find the source rule.')
add('LEN','Measure HEX byte length or string UTF-16 length.',
 ['Require one argument and evaluate it.','Return byte-array length for HEX.','Return Java String.length() for STRING; reject numbers and booleans.'],
 'C++ uses the byte vector size or StringValue.getUTF16().size(), preserving code-unit counting instead of counting UTF-8 bytes or visible characters.',
 'The unary matrix includes empty and emoji strings. Explicit SUBSTR/LEN cases split surrogate pairs, and LEN(SETLEN(...)) checks byte lengths at size boundaries.',
 'Predict LEN([a🐼b]) and explain each counted unit.')
add('OVERWRITE','Copy a range of source bytes into a copy of a destination.',
 ['Evaluate source bytes, source position, destination bytes, destination position and length in that order.','Copy the destination, so its original value is not changed.','Check destination end, then source end, then negative length, using Java int addition. Finally let array-copy range checks reject remaining invalid positions.','Return the modified copy.'],
 'The native code copies vectors, uses explicit modulo32-bit addition for the source checks and performs bounds checks before std::copy_n to avoid undefined memory access.',
 'Explicit driver cases cover a valid copy, negative source position and too-long copies. Generic runtime diagnostics from range failures are normalized; matching those cases does not establish exact JDK exception text.',
 'Why is checking negative length first a behavioral change when another invalid parameter is also present?')
add('REV','Reverse the order of bytes in HEX.',
 ['Require one HEX argument.','For each output position i, take input[length −i−1].','Return a new HEX value; this reverses bytes, not the bits inside a byte.'],
 'C++ copies the byte vector then uses std::reverse.',
 'The unary matrix includes0x00,0x80,0xFFFF, longer HEX values and wrong types; comparison runs through actual expression evaluation.',
 'Predict REV(0x0102) and explain why REV(0x80) remains0x80.')
add('SETLEN','Resize HEX while keeping the least-significant/rightmost bytes.',
 ['Read the requested length first and reject values below 1 or above MAX_DATA_SIZE.','Evaluate the HEX argument only after that check.','For shrinking, keep the rightmost newLength bytes. For growing, left-pad with zero bytes.','Return the resized HEX value.'],
 'C++ allocates a zero-initialized vector and copies min(oldLength,newLength) bytes from the end of each array.',
 'LEN(SETLEN(...)) is exercised at −1,0,1,2,4,65536 and65537; the driver compares success or source error as well as instruction state.',
 'Give the outputs of SETLEN(2 0xAABBCC) and SETLEN(4 0xAABBCC).')
add('SUBSET','Extract a half-open range of HEX bytes.',
 ['Evaluate start and end, then calculate end−start with Java int semantics.','Reject negative or excessive result length before evaluating the HEX array.','Check both endpoints against the array bounds and copy [start,end). An empty range is permitted.'],
 'C++ explicitly models32-bit subtraction before using checked vector iterators.',
 'The driver takes a product of negative, zero, normal and extreme32-bit endpoints against 0xAABBCC. It compares successful values and ordered failures.',
 'Explain why SUBSET(1 1 0xAABBCC) is valid even though SETLEN(0 ...) is not.')
for name,operation,example in [('ABS','absolute value','ABS(-3.7) is3.7'),('CEIL','ceiling (smallest integer at least the input)','CEIL(-3.7) is−3'),('FLOOR','floor (largest integer at most the input)','FLOOR(-3.7) is−4'),('INC','addition of one','INC(-3.7) is−2.7'),('DEC','subtraction of one','DEC(-3.7) is−4.7'),('SQRT','square root under MiniNumber arithmetic','SQRT(0.25) is0.5')]:
 add(name,'Apply '+operation+' to a Minima number.', ['Require exactly one argument.','Evaluate it as NumberValue, delegating type checks to Contract.','Call the corresponding MiniNumber operation and wrap its result. '+example+'.'],
 'The C++ wrapper calls the existing MiniNumber method; it does not replace decimal arithmetic with a platform floating-point function.',
 'The numeric-function loop includes0,1,−1,±0.25, variables, HEX/string/Boolean wrong types, a large integer, and zero/two arguments. Numeric results include serialized decimal bytes in the comparison.',
 'Which errors arise in this wrapper and which arise inside MiniNumber? Trace one wrong type and one arithmetic edge case.')
for name,comp in [('MIN','smallest'),('MAX','largest')]:
 add(name,'Select the '+comp+' of at least two numeric expressions.',
 ['Require at least two arguments.','Evaluate expressions left-to-right and reject any non-number.','Keep the first value as the initial candidate; replace it only when a later number is strictly '+('less' if name=='MIN' else 'greater')+'.','Return the selected Value object. Equal values preserve the earlier candidate.'],
 'C++ keeps a shared Value pointer as the candidate, retaining the Java aliasing choice rather than synthesizing an unrelated numerical result.',
 'The driver checks empty/one/two/many arguments, a mixed Boolean list and a missing variable. Trace comparison can detect an incorrectly skipped expression.',
 'If the current candidate already seems unbeatable, may evaluation skip the remaining arguments? Explain the observable error case.')
add('POW','Raise a number to an integer exponent; the exponent is the first argument.',
 ['Require two numeric arguments: exponent first, base second.','Reject a fractional exponent and any exponent whose absolute value is at least 1024.','Convert the exponent to int and delegate exponentiation to MiniNumber. Negative integral exponents reach that arithmetic implementation.'],
 'The C++ code preserves parameter order and both guards; it uses MiniNumber.pow instead of std::pow.',
 'The driver varies first arguments −1024/−2/0/0.5/1/2/64/1023/1024/4294967296 against several bases, exercising boundary and fractional exponents.',
 'Predict POW(3 2). Why might a reader expecting pow(base,exponent) port this incorrectly?')
add('SIGDIG','Round a number to a specified count of significant digits.',
 ['Require precision first and number second.','Reject fractional precision, then reject negative precision. Zero passes these guards.','Convert precision to int and call MiniNumber.setSignificantDigits.'],
 'Native code delegates decimal rounding to MiniNumber and preserves the two validation steps.',
 'The POW/SIGDIG argument product includes fractional, zero, negative and very large precision values. The oracle compares the serialized decimal result too.',
 'Why should a port preserve zero passing these guards even if the error wording says positive?')
for name,hashname in [('SHA2','SHA-256'),('SHA3','the shared Crypto SHA3 hash')]:
 add(name,'Hash raw HEX bytes or encoded string bytes using '+hashname+'.',
 ['Require one expression and accept only HEX or STRING.','Choose raw bytes for HEX, or the string encoding for STRING.','Hash those bytes through Crypto and return HEX. No length prefix is added by this wrapper.'],
 'C++ reuses MiniData’s '+name.lower()+' implementation over the selected byte vector.',
 'The unary matrix compares empty/string/HEX inputs, malformed types and wrong argument counts. The result is compared as HEX text; this exercises the common hash implementation indirectly.',
 'Would hashing a serialized MiniData value instead of its raw bytes produce the same digest? Explain why not.')
add('PROOF','Verify an MMR membership proof against an expected hash and sum.',
 ['Require data, leaf sum, root hash, root sum and serialized proof. Data must be HEX or STRING.','Build the domain-separated MMR leaf using the matching Streamable type and supplied sum.','Decode the proof; an IOException becomes the source-specific execution error.','Fold the leaf through the proof and compare both hash and sum with the expected root.'],
 'C++ reuses MMRData and MMRProof. Its current decoder catch accepts std::exception broadly; the KISS harness normalizes generic runtime failures, so full exception-category parity is still open.',
 'KissVMOracle builds real3-leaf trees for both HEX and STRING leaves. It emits a valid proof and an altered leaf-sum case for each; the driver also supplies a malformed proof.',
 'Why is a matching root hash alone insufficient for an MMRData equality check?')
add('CHECKSIG','Cryptographically verify a serialized TreeKey signature over supplied data.',
 ['Require three HEX arguments and charge31 extra instructions before extracting them.','Reject a zero-length public key or signature.','Create a TreeKey verifier with the supplied public key, decode Signature and verify the data.','Return the verification Boolean.'],
 'C++ reuses the native TreeKey and Signature rather than introducing an unrelated signing scheme. A missing decoded signature currently uses a generic native exception; the harness normalizes this category.',
 'KissVMOracle signs0x1234 with an actual TreeKey, then emits the valid case, modified data and a wrong public key. The driver adds empty/malformed signature cases and compares instruction accounting.',
 'Why should the additional instruction charge be checked even for an invalid signature?')
add('SIGNEDBY','Ask whether a public key is present among the Contract’s accepted signatures.',
 ['Require one HEX public key.','Ask Contract.checkSignature whether the key is recognized.','Return the Boolean; this wrapper does not decode and cryptographically verify a new signature.'],
 'C++ delegates to the same Contract signature membership operation.',
 'The rich fixture contains accepted keys 0xCA and0xFE; SIGNEDBY(0xCA) and shared unary invalid-type/arity cases run through the real evaluator.',
 'Explain the distinction between SIGNEDBY and CHECKSIG in a validation pipeline.')
add('MULTISIG','Count matching signature-key arguments until a requested threshold is reached.',
 ['Require a threshold and at least one key. Convert the threshold to int and reject negatives.','Visit each key expression in order and count each successful Contract signature lookup. The source does not deduplicate repeated key arguments.','Break after checking the current key if the threshold has been reached; return whether enough matches were found. Even threshold 0 can evaluate the first key.'],
 'C++ preserves the count and post-iteration early-break position.',
 'Explicit rich cases check two distinct keys, duplicate0xCA keys and a missing trailing expression after a satisfied threshold. Other cases use0/negative thresholds and a numeric key.',
 'Does MULTISIG(2 0xCA 0xCA) require two different keys in this source? Locate the operation that answers this.')
for name,previous in [('STATE',False),('PREVSTATE',True)]:
 add(name,'Read a '+('previous input' if previous else 'current transaction')+' state port.',
 ['Require exactly one numeric port.','Convert the number using MiniNumber.getAsInt.','Delegate to Contract.'+('getPrevState' if previous else 'getState')+' and return its Value; missing-port validation belongs there.'],
 'The C++ wrapper preserves the shared Value result and delegates lookup to Contract.',
 'The fixture installs current ports 0=17 and1=[hello], and previous ports 0=12 and1=[old]. Unary cases query existing and missing ports and wrong types; result/error and monotonic state are compared.',
 'Predict STATE(0) versus PREVSTATE(0), then trace how missing port2 is handled.')
add('SAMESTATE','Compare old and current state text across an inclusive port range.',
 ['Require two numeric endpoints; reject negative start or end before start.','For each port from start through end, read previous and current state and compare their toString values.','On the first difference, emit a trace message and return FALSE. If every port matches, return TRUE.'],
 'C++ uses an int64 loop variable to avoid undefined overflow at the32-bit endpoint while preserving ordinary inclusive traversal. Extreme-loop/API equivalence is not established by current cases.',
 'Explicit cases include ranges0..1,−1..0,1..0 and2..2. The fixture deliberately has unequal old/current values, exercising early FALSE, invalid range and missing state.',
 'Why can moving all state lookups ahead of the comparison change behavior when a later port is absent?')
for name,first in [('REPLACE',False),('REPLACEFIRST',True)]:
 add(name,'Replace '+('the first' if first else 'all')+' literal string match'+('' if first else 'es')+' while retaining Java replacement syntax.',
 ['Require main, search and replacement strings in that order.','Quote the search pattern so dots and other regex characters are literal. The replacement string is not quoted: backslash escapes and group0 remain meaningful.','Find '+('the first match and replace it.' if first else 'matches in order, append each replacement and enforce the intermediate maximum size; append the tail and check again.'),'Return StringValue, including its own encoding and length rules.'],
 'C++ reuses the UTF-16 replaceQuoted/expandReplacement helper. It models backslash and group0 behavior and handles empty searches by advancing one code unit. It does not invoke an arbitrary regex engine.',
 'The driver crosses originals abcabc/emoji/a.b/empty, searches a/emoji/dot/empty, and replacements including $0,$00,$01,$1, dangling backslash and ${no}. Generic replacement-runtime diagnostics are normalized, but KISS errors and successful traces are compared.',
 'Why is a literal-search replacement still different from an unconditional string.find()+insert loop?')
add('SUBSTR','Extract a half-open UTF-16 substring.',
 ['Read start and end and compute Java int end−start. Reject a negative length.','Evaluate the string, then reject a negative start or an end beyond Java String.length().','Extract [start,end) and construct StringValue. A range may split a surrogate pair; construction then applies the shared UTF-8 round trip.'],
 'C++ indexes std::u16string and preserves modulo32-bit subtraction; explicit extra range checks prevent undefined access.',
 'The endpoint cross-product runs on [a🐼b]. Additional cases join two individually split halves, compare a half with [?], and measure/encode that half, exposing the StringValue conversion boundary.',
 'Why can extracting each half of an emoji separately and concatenating them fail to reproduce the original emoji?')
for direction,prefix in [('input','GETIN'),('output','GETOUT')]:
 for suffix,field in [('ADDR','address'),('AMT','amount'),('TOK','token identifier')]+([('ID','coin identifier')] if direction=='input' else [('KEEPSTATE','state-retention flag')]):
  name=prefix+suffix
  logic=['Require one numeric index, convert it to int, obtain the transaction’s '+direction+' list and reject an index outside0..size−1.']
  if suffix=='ID':logic+=['After checking the transaction input exists, select the corresponding witness CoinProof’s coin.','Return that proof coin’s ID, which may differ from the transaction placeholder ID.']
  elif suffix=='AMT':logic+=['For a Minima coin, return its amount directly.','For another token, require attached Token metadata and return its scaled token amount.']
  else:logic+=['Read the selected coin’s '+field+'.','Return '+('BooleanValue for keep-state.' if suffix=='KEEPSTATE' else 'HexValue for these bytes.')]
  add(name,'Read the '+field+' of a selected transaction '+direction+'.',logic,
   'C++ retains the bounds check before selecting the vector entry and reuses Coin/Token/Witness. '+('GETINID deliberately reads witness proof data after the transaction check.' if suffix=='ID' else 'Nullable token metadata has an explicit source error in amount getters.' if suffix=='AMT' else 'No new transaction search is introduced.'),
   'The rich fixture has 3 '+direction+'s, a Minima coin, a scaled token coin and a coin with missing token metadata. The driver queries indices −1/0/1/2/3/0.5/4294967296 and TRUE. Input proof IDs are deliberately different from transaction IDs.',
   'Use the rich fixture to predict index 1. Explain why testing only index 0 would miss token or witness behavior.')
for name,direction in [('SUMINPUTS','input'),('SUMOUTPUTS','output')]:
 add(name,'Sum amounts of '+direction+' coins matching one token ID.',
 ['Require one HEX token identifier.','Start at MiniNumber.ZERO and visit every '+direction+' coin. Ignore other token IDs.','For matching Minima coins add the raw amount; for other matching tokens add the amount scaled through attached Token metadata.','Return the final NumberValue.'],
 'C++ uses existing decimal addition and Token scaling. The current optional.value() on missing metadata may yield a generic native error; the harness normalization means exact exception-class parity is not established.',
 'The driver checks Minima 0x00, the fixture’s scaled token variable tok,0x77 with missing metadata, and unmatched 0x88. This distinguishes raw amounts, scaled amounts, error and zero total.',
 'Why would summing raw stored values produce a wrong answer for a non-Minima token?')
for name,direction in [('VERIFYIN','input'),('VERIFYOUT','output')]:
 logic=['Evaluate index, expected address, amount and token ID in source order.']
 if direction=='output':logic+=['Accept4 or5 arguments; if supplied, evaluate an expected keep-state Boolean before selecting the output.']
 logic+=['Check that the selected '+direction+' exists. Compare its address and token ID with the expected values.','For non-Minima tokens, require metadata and compare the scaled amount.','Return the conjunction of all comparisons'+(' and, when requested, the keep-state match. Log a detailed trace when the result is false.' if direction=='output' else '.')]
 add(name,'Check several properties of a selected '+direction+' coin at once.',logic,
  'C++ composes the existing Coin/Token comparisons and amount conversion. VERIFYOUT retains the optional fifth parameter and mismatch trace, including the source’s spelling. Passing current cases does not prove arbitrary null/evaluation-order parity.',
  'The driver crosses indices −1..3, amounts0/5/7/12.5/25 and three token selectors. VERIFYOUT adds TRUE/FALSE/wrong-type/excess keep-state arguments. Values, execution state and trace output are compared.',
  'Create a case where address matches but amount differs. Then explain how a missing Token can throw instead of simply returning FALSE.')
