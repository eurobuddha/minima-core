from lesson_writer import author

def w(name,purpose,steps,exercise,limits=''):
 author('src/org/minima/system/commands/'+name+'.md',purpose,steps,'This mapped command remains unported. Reuse Command reply formatting and the pinned literal text or command metadata. Text-preservation work should keep Java escape interpretation, UTF-16 length and reply nesting explicit rather than editing the content during translation.',lambda l:'Proposed validation is an exact Java/native reply comparison after the counterpart exists. Include name/help metadata, fresh function instances, parameter acceptance and every branch described here. No native test pass is claimed for this absent command.',exercise,limits+' The native counterpart is absent; these are source explanations and future validation plans.')
w('help','Returns either detailed metadata for one command or a curated table of short descriptions.',[
 'Read optional command, default empty. For a nonempty name, ask CommandRunner.getCommandOnly; an unknown result raises Command not found with the name.',
 'For a found command, return the requested name, short help and full help. The command object’s metadata is the source of these strings.',
 'With no requested name, construct an explicit sequence of commands and insert their short help into a JSON object. This list is curated in the source, not generated from all registered commands; nodecount is commented out.',
 'Each table key passes through getStrOfLength(15): truncate to the first fifteen Java characters or pad on the right with spaces. A longer name is not preserved as a separate full key.',
 'Because the table is a map, two names sharing the same first fifteen characters would collide. Its observable iteration follows JSONObject/HashMap behavior rather than necessarily matching insertion order.'
], 'How would two command names with the same first fifteen UTF-16 code units affect the help object? Which test should compare keys rather than formatted terminal columns?')
w('tutorial','Returns a fixed grammar and function-reference string for KISS VM scripting.',[
 'Create the standard reply and concatenate the source string literals into one tutorial text.',
 'The text describes statement forms, expression precedence, values, globals and built-in function signatures. This method does not invoke the tokenizer/parser or check that the prose matches the current implementation.',
 'Store the text directly under response, then return. There is no filesystem or network read.',
 'Preserve the literal newlines, backslashes, spacing and source spellings. For example, SUMINPUT/SUMOUTPUT in the grammar and SUMINPUTS/SUMOUTPUTS in later descriptions are text in this file, not aliases implemented by this command.'
], 'Why would a successful exact-text test say nothing about whether a script shown in that text executes correctly? Identify the separate parser/VM test needed for that claim.')
w('whitepaper','Exposes the white-paper text embedded as the class’s WP constant.',[
 'The WP field is a long compile-time concatenation of literal strings. It contains the document title, abstract, numbered sections and references.',
 'runCommand creates the standard reply and a separate response object.',
 'Put the unchanged WP string under response.whitepaper, then return. This differs from tutorial, which puts its text directly under response.',
 'getFunction returns a fresh whitepaper command. The method does not fetch the referenced URLs, evaluate the paper’s arguments or execute protocol logic.'
], 'A client expects response to be a string because tutorial returned one. What assertion catches its incorrect assumption when calling whitepaper?',
 'This lesson reviews the executable wrapper and the constant’s role. It does not independently fact-check the historical paper or present its claims as current protocol guarantees.')
