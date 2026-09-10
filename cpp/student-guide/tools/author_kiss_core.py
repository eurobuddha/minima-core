#!/usr/bin/env python3
"""Manually authored source/body and harness review notes for KISS core types."""
from pathlib import Path
import os,re
BASE=Path(__file__).resolve().parents[1]
CPP=BASE.parent

def update(rel,purpose,steps,adaptation,tests,exercise,limits='',suite='kissvm'):
 p=BASE/'files/src/org/minima/kissvm'/rel
 if rel in ('values/HexValue.md','values/NumberValue.md','functions/MinimaFunction.md'):return  # Root owns current constructor/test follow-up.
 def link(target,label):return f'[{label}]({os.path.relpath(target,p.parent)})'
 s=p.read_text()
 def section(title,body):
  nonlocal s
  s=re.sub(r'(## '+re.escape(title)+r'\n\n).*?(?=\n## |\Z)',lambda m:m[1]+body+'\n',s,flags=re.S)
 s=re.sub(r'\*\*Teaching status:\*\*[^\n]+','**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.',s)
 s=s.replace('— source reading notes','— KISS core lesson')
 section('What this file does',purpose)
 section('Follow the logic','\n'.join(f'{i}. {x}' for i,x in enumerate(steps,1)))
 section('How the translation is approached',adaptation)
 if suite=='kiss':
  refs='The actual '+link(CPP/'support/KissOracle.java','KissOracle.java')+' and '+link(CPP/'support/kiss.cpp','kiss.cpp')+' execute the operations built by '+link(CPP/'tools/kiss_differential.py','kiss_differential.py')+'. Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing '+link(CPP/'build/kiss-differential-results.json','normal report')+' records 1,451 cases passed and zero failures; no new run was performed for this lesson.'
 else:
  refs='The '+link(CPP/'tools/kissvm_differential.py','KISS execution driver')+', '+link(CPP/'support/KissVMOracle.java','Java oracle')+' and '+link(CPP/'support/kissvm.cpp','native harness')+' compare actual execution, state, instruction counts and selected traces. See '+link(BASE/'evidence/kiss-builtins.md','the test-method lesson')+' for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.'
 section('What the tests tell us',tests+'\n\n'+refs)
 section('Check your understanding',exercise)
 sourcehash=s[s.index('Pinned Java SHA-256:'):].strip()
 section('Limits and traceability','This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity. '+limits+' Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.\n\n'+sourcehash)
 p.write_text(s)

update('values/BooleanValue.md','Represent a KISS Boolean as its own typed value rather than an implicit numeric truth value.',
 ['Construct the object with one Boolean field. The static TRUE and FALSE objects are reusable values.','Return type tag VALUE_BOOLEAN; print exactly TRUE or FALSE in uppercase.','isFalse negates the field, while isEqual compares two Boolean fields.'],
 'C++ uses an explicit BooleanValue subclass with the same type tag and two static objects. Value.getValue exposes stable shared references to the static objects through a non-owning deleter; raw object-identity parity beyond that path is not established.',
 'The value/type/verify/same cases include TRUE, FALSE, lowercase true and mixed Boolean/number/string types. The execution suite additionally tests RETURN/IF/ASSERT requiring Boolean rather than automatically coercing number 1.',
 'Why does RETURN 1 fail when RETURN TRUE succeeds?',suite='kiss')
update('values/HexValue.md','Wrap bytes as a typed KISS HEX value, including a distinct constructor from numbers.',
 ['Text and MiniData constructors delegate to the byte form. The byte form constructs MiniData and rejects more than MAX_DATA_SIZE bytes.','The numeric constructor first rejects negative values, then fractions. It converts the integer using BigInteger.toByteArray, whose signed representation can contain a leading zero byte.','Expose bytes/MiniData, print the hexadecimal representation and compare through MiniData.'],
 'C++ owns MiniData and returns const byte references. Numeric construction reuses BigInt.toJavaBytes, preserving the leading-sign-byte behavior instead of treating it as the HEX built-in conversion. Native invalid_argument currently represents several Java errors, so precise exception-class parity is open.',
 'The driver separately exercises hexnumber for negative, fractional and large values, plus byte sizes 0, 1, 65535, 65536 and 65537. It compares raw bytes as well as type and display text.',
 'Trace why constructing HexValue from the number 128 can need a leading zero byte. Which constructor enforces the byte-size limit?',suite='kiss')
update('values/NumberValue.md','Expose Minima decimal arithmetic to the KISS evaluator.',
 ['Construct MiniNumber from an integer/string or retain the supplied MiniNumber.','Delegate comparisons and the zero-is-false helper to MiniNumber.','For addition, subtraction, multiplication and division, return a new NumberValue containing the corresponding MiniNumber result.','Return the NUMBER type tag and MiniNumber text.'],
 'C++ stores MiniNumber by value and reuses its decimal operations. It does not convert values through double. Java reference aliasing to a supplied MiniNumber and the native value model are not fully audited by these arithmetic cases.',
 'The number cases cross 0, 1, -1, 123.456 and a large unsigned-looking value against 0, 1, -1, 3 and a small fraction. They collect all four arithmetic outputs and comparisons; division by zero can terminate the case. Successful numbers include serialized decimal bytes.',
 'Why might 1.0 and 1 compare equal yet still deserve a wire-representation check?',suite='kiss')
update('values/StringValue.md','Store KISS text through MiniString, with an encoded-byte size limit.',
 ['Construct MiniString from the supplied Java string. This performs the source encoding round trip.','Measure getBytes().length and reject a value larger than MAX_DATA_SIZE. This limit counts bytes, not visible characters.','Expose text, bytes and MiniString; compare text equality.','Concatenation constructs a new StringValue, so conversion and size checks run again.'],
 'C++ stores both MiniString and decoded UTF-16. Constructing from UTF-16 first encodes through MiniString and decodes again, replacing isolated surrogates with question marks just as the pinned path does.',
 'The value driver tests ordinary, empty and emoji strings and sizes 0, 1, 16383, 16384 and 16385 repeated pandas. Four UTF-8 bytes per panda bring the final two cases to the byte-limit boundary. The execution suite also splits surrogate pairs through SUBSTR and observes reconstruction.',
 'Why do 16,384 pandas fit a 65,536-byte limit while 16,385 fail?',suite='kiss')
update('values/Value.md','Classify and construct the four KISS value kinds, and enforce type compatibility.',
 ['Test the token in this order: surrounding square brackets, lowercase 0x prefix, exact TRUE/FALSE, then a leading minus or ScriptTokenizer.isNumeric.','Construct the matching subclass or throw an invalid-value error. The classifier getValueType follows the same syntax categories but does not construct the value.','verifyType requires exact tag equality. checkSameType first compares the two tags, then optionally checks the requested tag.','Translate known tags to their display names and return ERROR_UNKNOWN_TYPE for an unknown tag.'],
 'The native factory returns shared polymorphic values and stable shared references to static booleans. It retains classification order and delegates real numeric/HEX parsing, so recognition and successful construction remain different questions.',
 'The driver uses value/type/hexnumber/check on the same strings, including bracket mismatches, 0x/0X, uppercase/lowercase Boolean text, exponent-like numbers, a lone minus and whitespace. verify and same check several valid and invalid type tags.',
 'Can getValueType("-") identify NUMBER while getValue("-") still throws? Follow both methods.',suite='kiss')
for name,role in [('MinimaException','the shared checked-exception base for KISS'),('MinimaParseException','a failure while building a valid KISS program'),('ExecutionException','a failure while evaluating an otherwise parsed operation')]:
 update('exceptions/'+name+'.md','Identify '+role+'.',
 ['Receive the error string in the constructor.','Pass it unchanged to the superclass constructor. There is no separate algorithm or recovery policy in this file.','Let callers catch this type or a superclass; the distinction changes whether an error is propagated, wrapped or recorded in Contract.'],
 'C++ preserves the three-class inheritance relationship, with MinimaException deriving from std::runtime_error. Rendering a Java-qualified exception name is handled by surrounding exceptionString logic, not by duplicating it in every constructor.',
 'The execution driver contains malformed token/statement/function-arity cases and runtime failures such as ASSERT 0 or an out-of-range built-in. The actual oracle catches MinimaException separately from generic Exception, so the exception family affects serialized observations. There is no dedicated constructor-only assertion claimed here.',
 'Explain why catching every failure as a generic runtime error would lose information even though each class stores only a message.',
 limits='Complete Java checked-exception semantics and every native failure category are not established.')
update('functions/MinimaFunction.md','Hold unevaluated function arguments and supply the registry/validation rules shared by all 53 built-ins.',
 ['Normalize the constructor name and begin with an empty expression list. addParameter retains expressions for later evaluation.','getParameter rejects an index beyond the current count; checks for exact or minimum arity have distinct errors. Type checks use a bitmask, so a caller can accept HEX or STRING.','Parser-time checkParamNumberCorrect emits MinimaParseException. Runtime checks emit ExecutionException; preserving that separation matters.','getFunction searches the static prototype list and asks the match for a fresh function object, preventing arguments from one call contaminating another.'],
 'C++ uses shared expression pointers and a function-static vector of prototypes. Its upper helper handles ASCII; Java constructor normalization is broader and locale-sensitive. These public-name edge cases remain outside complete parity.',
 'Every built-in in the driver is evaluated through the parser/registry. Missing/excess parameters, unknown names and incorrect types exercise the shared checks. MIN/MAX and GET/EXISTS require minimum counts, while most other functions require exact counts. No isolated arbitrary registry-mutation test is claimed.',
 'Why is returning the registry prototype itself unsafe when it stores parameter expressions?')
update('statements/Statement.md','Define the executable statement interface used by a block.',
 ['The Java interface declares execute(Contract), which may throw ExecutionException. It has no executable body.','Concrete statements evaluate expressions and mutate the supplied Contract. StatementBlock supplies sequencing, accounting and trace calls around them.'],
 'C++ uses an abstract base with a virtual destructor, execute and a required toString method. Shared StatementPtr values let parsed blocks own polymorphic statements. The additional explicit toString contract corresponds to Java Object dispatch used by tracing.',
 'All script cases dispatch through StatementBlock to concrete statements. This validates representative implementations of the interface, not an abstract interface in isolation.',
 'What goes wrong if a polymorphic C++ base is deleted through a pointer without a virtual destructor?')
update('statements/StatementBlock.md','Execute an ordered list of statements while updating instruction and stack state.',
 ['Increment Contract stack depth on entry.','Before each statement, return immediately if success has already been set.','Increment instruction count, log the statement text, then execute it. Each step can fail before the next occurs.','Decrement stack depth only after normal completion of the loop. The source has no finally block: early return or an exception skips this decrement.'],
 'C++ preserves this explicit ordering, including the skipped decrement. A scope guard that always decremented would be a tidy-looking but behavior-changing rewrite.',
 'The driver varies script instruction limits 0, 1, 2, 8 and 1024, nested block depth, RETURN before later statements and failing assertions. The harness compares both instruction count and final stack depth, making bookkeeping observable.',
 'Trace RETURN TRUE RETURN FALSE: which statements execute, and does this block reach its final decrement?')
update('statements/commands/ASSERTstatement.md','Stop the contract with failure when a required Boolean condition is false.',
 ['Evaluate the assertion expression.','Reject a non-Boolean with the source ASSERT type error.','If false, call Contract.setRETURNValue(false). If true, leave the return state unchanged so execution can continue.'],
 'The native statement stores a shared expression and reproduces the conditional state update. It does not throw merely because a valid Boolean assertion is false.',
 'Explicit scripts include ASSERT FALSE RETURN TRUE, ASSERT TRUE RETURN TRUE and ASSERT 0 at several instruction limits. Comparing success-set and exception flags distinguishes ordinary false from a type error.',
 'Why is ASSERT FALSE not equivalent to throwing an ExecutionException?')
update('statements/commands/RETURNstatement.md','Set the contract result using a strictly Boolean expression.',
 ['Evaluate the return expression.','Reject any value whose type tag is not BOOLEAN.','Pass its true/false field to Contract.setRETURNValue; the containing block checks that success-set flag before executing another statement.'],
 'C++ retains a separate Boolean type check and writes the shared Contract result. Ordinary C++ truthiness would incorrectly accept numbers or strings.',
 'The driver includes RETURN TRUE, RETURN FALSE, RETURN 1, a missing RETURN expression and multiple RETURN statements, all at varied instruction limits.',
 'Which component actually stops later statements: RETURNstatement or StatementBlock?')
update('statements/commands/IFstatement.md','Run the first branch whose condition evaluates to Boolean TRUE.',
 ['Keep paired condition and action lists in insertion order.','Evaluate each condition and require BOOLEAN.','Run the matching StatementBlock, then break. Later conditions are not evaluated. An ELSE branch is supplied by the parser as an always-true condition.'],
 'C++ uses parallel vectors of shared expressions and blocks. It preserves first-match control flow and delegates instruction/stack behavior to StatementBlock.',
 'The driver tests TRUE/FALSE branches, ELSEIF, an invalid numeric condition, an empty branch, a RETURN inside a branch and nested IF depths 1, 63, 64, 65 and 70.',
 'Construct a program where the first condition is true and the second would reference a missing variable. Why should it still succeed?')
update('statements/commands/WHILEstatement.md','Repeat a block while a Boolean condition remains true, stopping when the contract result is set.',
 ['Evaluate the condition before entering the loop and require BOOLEAN. The source error text says RETURN, even though this is WHILE.','When true, run the body block. If Contract.isSuccessSet becomes true, return before evaluating the condition again.','Otherwise evaluate and type-check the condition again; stop once false.'],
 'C++ expresses the same sequence as an infinite loop with explicit exits. It retains the source error wording instead of silently correcting it.',
 'The driver includes an initially false loop, an incrementing loop that terminates at five, WHILE TRUE with a RETURN, a non-Boolean condition and an endless loop cut off by instruction limits.',
 'Why must a RETURN inside the body prevent evaluation of a now-invalid condition?')
update('statements/commands/LETstatement.md','Assign a value to a named variable or an array-style numeric-index key.',
 ['For a normal name, evaluate the right-hand expression then store its Value.','For an array assignment, evaluate each index in order, require NUMBER and append its trimmed text and a comma.','Only after the key is formed, evaluate the right-hand value and store it. GET and EXISTS build the same key format.','toString renders the assignment for traces. The Java array formatter catches Error and logs its stack; that exceptional formatting branch is not reproduced by the current native body.'],
 'C++ stores shared expressions and retains assigned Value pointers through Contract. Its trim helper removes bytes at most space, matching the source’s trim intent for numeric text. The missing Error catch in toString remains an explicit gap.',
 'The driver tests normal assignment, replacement, LET (1 2), an invalid Boolean index, an empty index list and malformed assignment syntax. Shared variable state and traces are compared; no injected serious-error formatter test is claimed.',
 'Why should a failure evaluating an array index prevent evaluation of the right-hand expression?')
for name,kind in [('EXECstatement','string'),('MASTstatement','witness')]:
 steps=['Evaluate the supplied expression and cast it to '+('StringValue before the protected parsing block.' if kind=='string' else 'HexValue, then locate the corresponding script in the Contract witness. Reject a missing script before parsing.')]
 steps+=['Tokenize the selected script and parse it at the current Contract stack depth.','Run the resulting StatementBlock in the same Contract, so variable and return-state changes remain visible to the caller.','Rethrow ExecutionException unchanged; wrap other caught parsing/execution exceptions as ExecutionException. Failures before the try block are not wrapped by this catch.']
 update('statements/commands/'+name+'.md',('Execute a script value inside the current Contract.' if kind=='string' else 'Execute the witness script selected by its address/proof lookup.'),steps,
 'C++ reuses ScriptTokenizer and StatementParser and uses dynamic_cast for the source type cast. Exact generic class-cast/runtime diagnostic parity remains outside the current normalized suite.',
 ('The driver checks EXEC [RETURN TRUE], shared LET variables, nested EXEC depths, a number instead of a script and malformed nested RETURN.' if kind=='string' else 'KissVMOracle installs actual ScriptProof objects for RETURN TRUE and LET x=42, then emits their real addresses for MAST. The driver also tests a missing address and a numeric parameter.')+' The harness compares shared state, success, instruction/stack counts and applicable traces.',
 'Why would evaluating the nested script in a new independent Contract change the result?')
print('Authored 18 KISS core lessons.')
update('expressions/Expression.md','Define the operation that evaluates a syntax-tree node into a typed Value.',
 ['The Java interface declares getValue(Contract), which can throw ExecutionException. There is no evaluation algorithm in the interface itself.','Concrete nodes retain their operands and evaluate them against the caller’s Contract. Parsing builds this tree before execution.'],
 'C++ uses an abstract base with a virtual destructor and shared ExpressionPtr/ValuePtr ownership. It makes toString explicit because traces need polymorphic expression display.',
 'Every expression-mode program in the KISS driver invokes getValue on a parsed tree. This exercises concrete nodes behind the interface; it is not an independent proof of every possible subclass implementation.',
 'Why should parsing an expression avoid evaluating its variable values immediately?')
update('expressions/ConstantExpression.md','Return a literal value while charging for its evaluation.',
 ['Retain the Value supplied at construction.','On getValue, increment the instruction count before returning that same stored Value.','Display the stored Value text without another evaluation.'],
 'C++ retains a shared Value pointer and returns it unchanged. Removing the instruction increment as an optimization would change resource accounting even though the numerical result is identical.',
 'The driver evaluates Boolean, numeric, HEX and string literals directly and as operands, and varies script instruction limits. The harness compares instruction counts, values and failure state.',
 'Why can evaluating a constant fail when the contract has exhausted its instruction budget?')
update('expressions/VariableExpression.md','Resolve a named variable from the current Contract.',
 ['Store the variable name at construction.','Charge one instruction, then query Contract.getVariable.','Throw the source-specific missing-variable execution error if the result is null; otherwise return the stored Value.','Render variable:name for expression display.'],
 'C++ preserves shared Value identity and performs an explicit null test. Variable values are looked up at evaluation time, allowing earlier LET statements to change the result.',
 'The fixture defines a, b, t, f, h and s; the driver evaluates those and missing. Script cases also overwrite variables and use them after LET or nested EXEC/FUNCTION.',
 'When LET x=x+1 runs, which value of x does the right-hand expression read?')
update('expressions/GlobalExpression.md','Resolve a built-in global such as a block or coin property.',
 ['Retain the global name.','Charge one instruction before calling Contract.getGlobal.','Return the result and let Contract implement global availability and monotonicity rules.','Display global:name.'],
 'C++ is the same thin delegation layer. The meaning of a global belongs to Contract, so this wrapper must not cache a previously read value independently.',
 'The ordinary fixture supplies @BLOCK and @CREATED, while rich cases also query @TOTIN, @TOTOUT, @AMOUNT and @COINID. @MISSING exercises a failed lookup; monotonic state is part of the harness output.',
 'Why might reading a block-dependent global affect whether a script result can safely be reused?')
update('expressions/FunctionExpression.md','Evaluate one built-in function with shared instruction, stack and trace bookkeeping.',
 ['Charge one instruction and increment Contract stack depth.','Call the retained MinimaFunction, whose arguments are expressions evaluated by the function as needed.','On normal return, decrement depth, trace function name/parameters and returned value, then return it.','If runFunction throws, the decrement and result trace are skipped because the source has no finally block.'],
 'C++ retains a shared function instance and keeps the explicit sequencing. Its formatter reconstructs Java list-style parameter text used by trace comparisons.',
 'All built-in expression cases pass through this wrapper. Wrong argument types, arithmetic failures and nested functions are compared with final stack depth and instruction count, exposing premature cleanup or missing bookkeeping.',
 'Would an unconditional scope guard to restore stack depth match the source after a function throws?')
update('expressions/BooleanExpression.md','Evaluate logical and comparison operators with eager operand evaluation.',
 ['Charge one instruction, then evaluate left and right operands in that order. AND/OR do not short-circuit this evaluation. Unary NOT carries a dummy FALSE ConstantExpression as the right operand, which is still evaluated.','Except for NOT, require matching operand types. Logical operators require BOOLEAN; ordering comparisons require NUMBER; EQ/NEQ handle matching supported types.','Apply the operator, create the Boolean result, then trace the expression and result.','Reject an unknown operator code with ExecutionException.'],
 'C++ uses shared operand nodes and explicit evaluation before its operator switch. It must not translate operand evaluation directly into C++ && or ||, which would skip the right side. Generic type-error categories remain normalized in the current suite.',
 'The driver crosses literal/value categories with every Boolean operator and includes RETURN FALSE AND missing and RETURN TRUE OR missing. Those two programs expose eager evaluation. Unary cases expose the extra dummy operand’s instruction charge.',
 'Why does TRUE OR missing still fail in this VM? Count the expression evaluations involved.')
update('expressions/OperatorExpression.md','Implement numeric, string and hexadecimal operators with source-specific byte normalization.',
 ['Charge an instruction and evaluate both operands, including the dummy FALSE operand used by unary NEG and bitwise NOT.','ADD accepts matching NUMBER or STRING values and returns immediately, so it skips the common result trace. Other numeric operations enforce NUMBER; division checks zero explicitly.','Shifts require HEX on the left and NUMBER on the right, bound the absolute shift before integer conversion and delegate to MiniData.','Bitwise AND aligns the rightmost bytes to the shorter length; OR/XOR zero-pad on the left to the longer length. NOT inverts each byte. All these helpers strip leading zero bytes and represent an all-zero result as one zero byte.','Other successful paths emit the common expression/result trace.'],
 'C++ reuses MiniNumber and MiniData arithmetic and implements byte loops over checked vectors. A shared stripZeros helper preserves the explicit zero representation. ADD’s early return is retained, even though tracing it consistently might seem cleaner.',
 'The driver crosses many value types with arithmetic/bitwise operators, tests divide-by-zero and fractional/negative/excessive shifts, and includes precedence examples. It compares values, number wire bytes and trace, so the special ADD trace omission is observable.',
 'Why is the zero result of bitwise operations different from BITSET clearing its final set bit? Compare their byte-normalization rules.')
update('expressions/ExpressionParser.md','Build an expression tree using recursive-descent precedence levels.',
 ['Reject an empty top-level token list. Parse through a LexicalTokenizer and then require that all tokens were consumed.','From weakest to strongest, parse Boolean combinations, comparisons, HEX bitwise operators, the shared +/−/%/shift level, multiplication/division, then unary/base expressions. Operators at the same level are accumulated left-to-right.','Each level consumes recognized operators and pushes back the first unrecognized token so its caller can handle it.','Base expressions include literals, variables, globals, Boolean constants, functions and parentheses. A leading minus consumes a following numeric token directly; NEG is the general unary expression operator.','Function parsing requires parentheses, builds each argument expression, enforces parameter/depth limits, then checks the function’s declared arity. Parentheses recurse and require a matching close token.'],
 'C++ translates these levels directly with shared expression nodes. It preserves the source’s unusual precedence: modulo shares the addition/shift level, and all Boolean combinations share one level. The LexicalTokenizer constructor ignores its supplied initial depth in both versions; that quirk affects where depth checks start.',
 'Explicit expressions include TRUE OR FALSE AND FALSE, 1+7%3*2, NOT TRUE EQ FALSE, mixed bitwise operators, parenthesized arithmetic, -a, -(1), NEG NEG 5, extra tokens and missing parentheses. Depth cases use 1/63/64/65/70 nested groups; the harness compares tree display and execution.',
 'Draw the parse tree of TRUE OR FALSE AND FALSE using this source. Why must a port not assume Java or C++ operator precedence?')
update('tokens/LexicalTokenizer.md','Maintain a cursor and depth counter over an already-tokenized list.',
 ['Retain the token list and set the cursor to zero. The Java constructor receives zStackDepth but never assigns it, leaving mStackDepth initially zero.','getNextToken rejects exhaustion, then returns the current token and advances the cursor. goBackToken rejects position zero before decrementing.','Expose whether there are more tokens and whether all have been used. Increment/decrement the depth counter separately.'],
 'C++ intentionally ignores the constructor depth parameter and stores tokens in a vector. Explicit unsigned arithmetic plus bit conversion models Java signed32-bit depth wraparound. Copying the token list rather than retaining arbitrary mutable Java list aliases remains an API difference to review.',
 'The lexical driver supplies initial depth 17, reads the observed depth, increments once and decrements twice, then walks positions and tokens. It checks all-used and one-token pushback/re-read. It does not claim all arbitrary list mutations or repeated invalid pushbacks are covered.',
 'What depths are observed after construction with17 and after +1,−1,−1? Explain why implementing the apparent constructor intent would fail parity.',suite='kiss')
update('tokens/ScriptToken.md','Pair a token kind with its source text and render a stable diagnostic label.',
 ['Construct the token from an integer type code and text.','Map known codes to names such as COMMAND, FUNCTION or VALUE; unknown codes return the string null.','Render TYPE:text and expose the raw code/text.','The static tokenize helper creates a ScriptTokenizer and delegates to it.'],
 'C++ stores the same integer constants and strings, including the source spelling TOKEN_FUNCTIION. Its explicit toString replaces Java Object dispatch.',
 'For each tokenized script, the values/tokens harness records numeric type, raw token text and toString. This checks the three views together. Unknown manually constructed type codes are not claimed as directly tested by this driver.',
 'Why retain a misspelled public constant while printing the correctly spelled label FUNCTION?',suite='kiss')
update('tokens/ScriptTokenizer.md','Split script text into typed tokens before syntactic parsing.',
 ['Walk Java UTF-16 positions from the start, tracking whether the previous consumed character was whitespace. Recognize whitespace, single-character operators, doubled shift characters and parentheses.','A square bracket starts a nested-bracket string scan. The scanner emits the accumulated text even when the closing bracket is missing; later stages may reject it. Dollar-prefixed words become function-parameter tokens.','For an ordinary word, optionally compute case-insensitive forms and classify in source order: command, registered function, Boolean operator, numeric/HEX literal, Boolean constant, global or variable.','Commands require preceding whitespace; variables are lowercase letters with a32-character limit. Numeric and HEX recognition use restricted ASCII forms.','Throw for an unrecognized word or malformed doubled shift token. A terminal < or > fails while accessing its next character, before the explicit mismatch error.'],
 'C++ uses UTF-16 positions and explicit ASCII classifiers in place of Java regexes. Its case helper includes inspected Unicode-to-ASCII mappings relevant to the default/root-locale fixture. Native function names are a pinned separate list; Java reads the mutable MinimaFunction registry. Arbitrary registry mutation and other default locales remain gaps.',
 'The driver runs each script in both case modes, includes all built-in and command names, nested/unclosed strings, missing command whitespace, malformed shifts, nonbreaking spaces and Unicode case examples such as ligatures, long-s and Kelvin sign. It adds300 deterministic random strings and compares full token triples or normalized errors.',
 'Why can a token scanner accept an unclosed bracket string without the complete program being valid?',suite='kiss')
update('statements/StatementParser.md','Group command tokens into executable statements and nested blocks.',
 ['Increase the supplied parse depth and reject a depth above the contract limit. At each new statement require a COMMAND token.','LET distinguishes a named variable from a parenthesized list of index expressions, finds the equals sign, and parses the right-hand expression up to the next command. EXEC/MAST/ASSERT/RETURN similarly consume an expression to the next command.','IF collects a condition through THEN, finds the matching same-depth ELSE/ELSEIF/ENDIF boundary and recursively parses each action block. ELSE becomes a constant TRUE condition.','WHILE collects its condition through DO, scans through the matching ENDWHILE and recursively parses the body. Boundary scanners retain nested delimiters so inner blocks remain intact.','Return a StatementBlock containing the constructed statements. Some missing-terminator paths fail through list access or discard the last available token; do not silently replace them with a different grammar.'],
 'C++ uses the existing ExpressionParser and statement classes. It replaces the source’s recursive delimiter scanners with an iterative nesting counter, preserving the token slices passed to recursive block parsing. Source depth addition uses explicit32-bit wraparound. Generic malformed-list failures are normalized by current tests.',
 'The driver contains ordinary/nested IF, ELSEIF, WHILE and LET array programs, missing right-hand expressions and delimiters, stray ELSE, empty branches and depths around64. Selected malformed WHILE cases expose the source’s lenient boundary behavior. The actual Contract parse flags and subsequent execution are compared.',
 'When finding an outer ELSE, why must the scanner ignore an ELSE belonging to a nested IF? Draw the depth counter transitions.')
print('All 30 non-Contract KISS core lessons authored; with53built-ins and root Contract,84 KISS production lessons.')
