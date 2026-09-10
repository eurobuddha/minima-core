"""Authored lessons from the paired Java/native bodies and object oracle."""
from lesson_writer import author,CPP

def evidence(detail):
 def build(link):
  return detail+'\n\nThe '+link(CPP/'support/ObjectOracle.java','Java object oracle')+' and '+link(CPP/'support/objects.cpp','native object harness')+' produce matching wire bytes and JSON or explicit derived values. The '+link(CPP/'tools/object_differential.py','object differential driver')+' also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.'
 return build

common='The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.'

def write(rel,purpose,steps,adapt,tests,exercise,limits=''):
 author('src/org/minima/'+rel+'.md',purpose,steps,adapt,evidence(tests),exercise,limits+' '+common)

write('objects/StateVariable','A typed state slot carried by a coin or transaction. Its port identifies the slot, while the type determines both the canonical text and the binary payload.',[
 'Reject ports outside 0 through 255. Reject a value whose trimmed text is empty, but do not trim the value before classifying it. Leading whitespace can therefore change which branch is taken.',
 'Classify in order: a Minima address beginning Mx, hex beginning 0x, TRUE/FALSE ignoring case, bracketed script text, otherwise a number. Address and hex inputs become canonical hex; booleans become uppercase; numbers use MiniNumber canonical text.',
 'Use type tags 1 for hex, 2 for number, 4 for string and 8 for boolean. Brackets remain part of a string value; they are not stripped here.',
 'Serialize the one-byte port and type, followed by the corresponding MiniData, MiniNumber, MiniString or boolean byte. The reader chooses its payload decoder from the type byte and rejects unknown tags.',
 'JSON reports port, type and canonical data text. Two spellings such as 1.00 and 1 can therefore describe the same normalized numeric state.'
], 'The native object stores MiniByte and MiniString values and dispatches serialization with a switch. Its constructor uses std::invalid_argument for validation; the differential harness normalizes failures, so matching rejection alone does not prove the Java exception type was retained.',
 'The state fixture crosses ports -1, 0, 127, 128, 255 and 256 with typed and invalid values, comparing canonical JSON and binary bytes. This probes unsigned-byte boundaries as well as type classification.',
 'Predict the type and stored text for TRUE, [TRUE], 1.00 and 0x01. Then explain why a leading space before 0x01 is not equivalent to trimming it first.')
write('objects/CoinProof','Packages the complete coin and the MMR path needed to locate its commitment. The path is evidence to be checked elsewhere; constructing this wrapper does not authenticate a coin.',[
 'Keep a Coin and an MMRProof together so that a verifier can reconstruct the exact committed leaf.',
 'Build that leaf with CreateMMRDataLeafNode(coin, coin.amount). Both the serialized coin and its amount contribute to the commitment; the amount also becomes the MMR sum.',
 'Render separate coin and proof JSON fields. Serialize the coin first and the proof second, with no extra wrapper tag.',
 'Read the coin completely, then read the proof. The Java convenience converter logs an IOException and returns null; ordinary reading propagates errors.'
], 'Java stores object references. C++ takes values and moves them into the wrapper, while exposing a mutable coin getter. The native convenience converter returns std::optional and catches every std::exception rather than only IOException. Those ownership and failure differences need separate review.',
 'The coinproof fixture uses coin indices 0 through 31, varying state, spent status, entry number and token metadata, and compares the wrapped bytes, JSON and derived leaf. Its proof starts at block time 42 with an empty chain, so this fixture alone does not demonstrate an authenticated nonempty path.',
 'If a caller changes the coin amount after constructing a proof wrapper, which derived data changes? Why must the surrounding verifier check the path against a trusted root?')
write('objects/ScriptProof','Commits to script text and a Merkle path, deriving the address at which that script can be used.',[
 'For a script alone, Java builds a one-leaf MMR with sum zero and obtains its proof. A single leaf has no sibling chunks.',
 'For a supplied proof, keep the script and proof together. Hash the MiniString script as a zero-sum leaf, then fold the proof to obtain a root hash.',
 'Construct an Address from that root hash. This means the address includes the proof context, not merely a direct hash of script characters.',
 'Write script then proof. On reading, recompute the address rather than reading a separately serialized cached address. JSON includes script, address and proof.'
], 'The native single-script constructor delegates to an empty default MMRProof, reusing the single-leaf equivalence. It stores script and proof by value and recalculates the address in both normal construction and deserialization.',
 'The scriptproof branch compares four script strings through the constructors and derived addresses. The broader script/address and leaf branches include Unicode text, and byte round trips exercise address recomputation. Direct supplied-proof constructor equivalence still needs an explicitly linked fixture.',
 'Why can an empty proof be valid for a single-leaf tree? How would adding one sibling alter the resulting script address?')
write('objects/keys/SignatureProof','Stores a one-time public key, its signature bytes, and a path connecting that key to a root public key.',[
 'Keep the public key and signature as MiniData and the path as MMRProof. The constructor does not verify the signature.',
 'Compute a zero-sum MMR leaf from the public key, then fold the proof and return its root hash as the root public key.',
 'JSON presents the leaf public key, derived root key, proof and signature. The derived root key is calculated rather than serialized independently.',
 'Write public key, signature and proof in that order. Read the same sequence to rebuild the wrapper.'
], 'The native class uses value members and move construction. It reuses the common MMRData and MMRProof implementations for root derivation, keeping signature cryptography in the separate signature/key classes.',
 'The sigproof fixture builds explicit key/signature bytes with an empty proof at time 17 and compares wire data, JSON and the derived root key. Tree-key and signature cases elsewhere in the corpus exercise larger structures; this wrapper test by itself does not authenticate signature bytes.',
 'Why can arbitrary signature bytes still produce a well-formed SignatureProof JSON object? Identify the additional operation needed before trusting the signature.')
write('objects/mmr/MMRData','A hash-and-sum commitment used at every MMR node. The hash commits to structure and contents; the value tracks the accumulated amount.',[
 'Create a leaf by hashing serialized MiniNumber.ZERO, the serialized object, and its sum. The zero prefix distinguishes a leaf from an internal node.',
 'Create a parent by adding left and right values, then hashing MiniNumber.ONE, left hash, right hash and the combined sum. Left/right order matters even when addition gives the same sum.',
 'Equality requires both hash and value to match. Comparing only the hash would omit part of the object’s explicit equality contract.',
 'The unspendable flag is mutable in-memory metadata. It is absent from serialization and JSON, so a round trip does not preserve it.',
 'Write the hash using the hash-specific MiniData encoding, followed by MiniNumber value. Read the same order.'
], 'The native leaf helper is a C++ template accepting serializable objects; both leaf and parent reuse Crypto::hashAllObjects. It preserves the unspendable flag as a public bool with accessor methods, separate from wire data.',
 'The leaf branch uses script MiniString data and selected sums; proof cases repeatedly combine leaves and compare computed roots. Coin proofs also commit complete serialized coins. These observations exercise both domains and sum propagation, but do not prove all possible object serializers are correct.',
 'Construct two conceptual leaves with sums 2 and 3. Explain why swapping them preserves the root sum 5 but usually changes the root hash.')
write('objects/mmr/MMRProof','An ordered list of sibling commitments that lets a verifier reconstruct an MMR ancestor from one leaf.',[
 'Store a block time and a chain of chunks. Each chunk holds a one-byte left flag and an MMRData sibling containing both hash and value.',
 'Start calculateProof with the supplied leaf. For each chunk, place the sibling on the left when its flag is true; otherwise place it on the right. Replace the running value with the new parent.',
 'An empty chain returns the input leaf unchanged. The method calculates a candidate root; it does not decide whether that root is trusted.',
 'Serialize block time, the chunk count as MiniNumber, then each left flag and sibling. The reader converts the count to int and reads chunks in sequence.',
 'Java replaces its chain before reading the count and appends completed chunks; malformed input can therefore leave a partially changed existing object. The convenience MiniData converter propagates IOException.'
], 'C++ uses a vector of chunks and a static deserializer that returns a newly constructed value. The offset still advances as fields are read, but there is no preexisting caller object to mutate. The reader preserves the original MiniByte flag after interpreting its truth value, maintaining noncanonical flag bytes during round trips.',
 'Ninety seeded proof cases vary chunk counts, directions, sibling text and sums. The oracle compares serialized proof, JSON and the calculated root. Generated wire cases also test truncation and trailing data. Comparing a reconstructed root with a trusted chain root remains the caller’s job.',
 'Write the parent expressions for two chunks: first left, then right. Explain why reversing their order changes the result even if all sibling hashes are unchanged.')
write('objects/mmr/MMREntry','A position in an MMR: row, entry number and optional hash-and-sum data. It provides the tree arithmetic used to find relatives.',[
 'A position-only constructor represents an empty entry; a constructor with data represents a populated entry. Position comparison checks row and entry number, independently of data.',
 'Even entry numbers are left children. The sibling is one position higher for a left child and one lower for a right child.',
 'The parent has row + 1 and floor(entry / 2). Children have row - 1 and positions 2*entry and 2*entry + 1.',
 'JSON allows null data for an empty entry. Serialization nevertheless expects populated data and writes row as MiniNumber, then entry number and MMRData.',
 'Java marks an entry nonempty before reading its fields. A failed read can leave that receiver partly changed; this is distinct from successfully reading a complete entry.'
], 'Native data is a shared_ptr, so entry copies can share mutable MMRData. Absence of that pointer represents emptiness. Row arithmetic explicitly uses unsigned addition/subtraction followed by bit_cast to reproduce Java int wraparound without C++ signed-overflow undefined behavior. Static deserialization returns a new populated entry.',
 'The mmrentry branch compares sibling, parent, children and side classification for positive, negative and very large entry numbers and several rows. The object driver also varies fractional entry-number construction; this checks the actual MiniNumber-derived arithmetic rather than assuming every position is a small unsigned integer.',
 'For row 2 and entry 5, calculate sibling, parent row/entry and both child positions. Then explain why two entries with different data can still have the same position.',
 'Serializing an empty entry throws a native runtime_error versus a Java null dereference; generic error normalization does not settle this exception difference.')
write('system/genesis/GenesisCoin','Defines the special starting coin used to seed the initial MMR and fund the genesis transaction.',[
 'Use the fixed identifier 0x5350415254414355534C4F5645534D494E494D41.',
 'Construct an ordinary Coin locked to Address.TRUE_ADDRESS, with amount MiniNumber.BILLION and the Minima token identifier.',
 'Inherit the normal Coin representation and wire behavior. This class adds no separate serialization layer or validation routine.'
], 'The C++ class derives from Coin and invokes the matching base constructor with the same constants. Reusing Coin keeps genesis serialization subject to the same field encoding as later coins.',
 'Each genesis fixture outputs the GenesisCoin wire bytes and JSON alongside the genesis MMR and TxPoW. Five destination addresses vary the genesis transaction while this starting coin remains fixed. This verifies the observed constant initialization, not a running network bootstrap.',
 'Why is the genesis coin’s address different from the destination supplied to GenesisTxPoW? Trace the coin from input to output.')
write('system/genesis/GenesisMMR','Builds the initial one-leaf MMR containing the genesis coin, before the genesis transaction spends it.',[
 'Initialize the inherited MMR at block time zero.',
 'Construct GenesisCoin and turn the complete coin into an MMR leaf using its amount as the sum.',
 'Add that one entry, then finalizeSet so the inherited MMR computes and caches its finalized structure.',
 'Subsequent genesis transaction construction can request the proof to peak for entry zero.'
], 'Native code reuses the already mapped MMR, GenesisCoin and MMRData implementations. The constructor is a short composition of these operations rather than a second Merkle-tree implementation.',
 'The genesis oracle returns this MMR’s bytes and JSON, and GenesisTxPoW requests its entry-zero proof. The broader MMR fixtures cover trees with multiple sizes and mutations, but those counts are not independent GenesisMMR tests.',
 'Explain why this MMR’s total equals one billion and why its entry-zero proof has no sibling chunks.')
write('system/genesis/GenesisTxPoW','Constructs the special first transaction/block candidate by spending the predefined genesis coin to a supplied destination.',[
 'Create the genesis MMR. Initialize transaction and block difficulty to MAX_HASH, nonce to 256, current time, block number 1 and all superparents to 0x00.',
 'Add GenesisCoin as the transaction input. Add a one-billion Minima output with COINID_OUTPUT and the supplied destination address bytes.',
 'Attach the genesis coin proof to peak zero and the TRUE_ADDRESS script proof to the witness. These explain where the input comes from and how its script can be satisfied.',
 'Set the body hash and calculate transaction identifiers, then build a TxBlock over the genesis MMR. Java constructs a TxPoWTreeNode and obtains the resulting MMR root and total.',
 'Put that root and total in the header, calculate the TxPoW identifier, log it, and explicitly mark both block and transaction proof-of-work flags true.'
], 'The native constructor uses the same object constructors and constants. It replaces Java’s TxPoWTreeNode route with the existing constructBlockMMR support routine, so that adapter is a material dependency to review. C++ logs through std::clog.',
 'Five genesis destinations are compared. Before comparing, both harnesses overwrite time with 1788883200000 and PRNG with 000102, then recalculate body hash and TxPoW ID. This makes the fixture deterministic; it is not evidence that independent real-time constructor calls produce identical IDs. Output includes the genesis coin and initial MMR as well as the resulting TxPoW.',
 'Which fields must change if the destination changes? Why does the test pin time and PRNG before comparing IDs?',
 'Full-node startup, persistence, networking and chain acceptance are outside this constructor fixture. The support MMR adapter is not evidence that all TxPoWTreeNode behavior has been mapped.')
