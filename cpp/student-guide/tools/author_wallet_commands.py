from lesson_writer import author

def w(name,purpose,steps,exercise):
 author('src/org/minima/system/commands/'+name+'.md',purpose,steps,'This mapped native command is absent. A future port should reuse Command parameter conversion and the existing Address, Contract, ScriptRow or Signature components at the same call boundaries; persistence must follow Wallet’s behavior rather than being replaced with a new storage model.',lambda l:'No native command test is claimed. Proposed tests should supply a controlled wallet, record calls and returned JSON, then compare the branch and failure scenarios described below against pinned Java. Successful lower-level object tests do not establish the missing wallet integration.',exercise,'This is an authored source-logic lesson and proposed validation plan. The mapped command remains unported; storage, permission and runtime failure behavior still need implementation and direct tests.')
w('scripts/newscript','Adds or replaces a custom wallet script with an explicit coin-tracking preference.',[
 'Resolve the wallet before reading script and required trackall. Optional clean defaults false.',
 'When clean is true, call Contract.cleanScript before deriving the Address. Cleaning can therefore change the address used as the database key.',
 'Look up the derived address; if a script row already exists, remove it first.',
 'Call addScript with the selected text, simple=false, default=false, publickey=0x00 and the tracking flag. Convert the returned row to JSON.',
 'Removal and insertion are separate calls. A later insertion failure can follow successful removal; a test should inspect the intermediate persistent state rather than assuming an atomic replacement.'
], 'Compare clean:false and clean:true for whitespace-bearing source. Then inject addScript failure after a matching old row is removed: what state must your fixture observe?')
w('scripts/removescript','Removes the wallet script identified by a normalized address.',[
 'Create the standard reply and resolve the wallet.',
 'Read address through getAddressParam, which performs the established Minima-address conversion.',
 'Call removeScript without first checking whether a row exists.',
 'After that call returns, report Script removed. This wording does not establish that a record previously existed.',
 'The help string starts with a carriage-return escape from the source spelling; exact metadata comparison should retain actual characters rather than a visually corrected string.'
], 'Test absent and existing addresses with the same wallet return behavior. Should the command replies differ? Why must malformed address input be tested before the delete callback?')
w('scripts/scripts','Lists tracked wallet scripts or retrieves one exact address row.',[
 'Resolve the wallet and read optional address, defaulting to empty.',
 'A case-insensitive mx prefix triggers Address.convertMinimaAddress. Only IllegalArgumentException from that conversion is wrapped as CommandException with the original exception text.',
 'An empty address requests getAllAddresses and preserves returned row order while converting each row to JSON.',
 'Otherwise perform an exact wallet lookup on the supplied or converted address. A missing row raises Script with that address not found.',
 'Ordinary hex-looking text is not passed through getAddressParam here. Direct comparison with removescript should therefore include malformed non-Mx text and case differences.'
], 'Why can the empty-address result be an array while a successful address lookup returns one object? Design a test that catches accidental normalization of ordinary hex text.')
w('signatures/sign','Asks the wallet to sign raw data with the private key corresponding to a supplied public key.',[
 'Parse required data first, then required publickey as MiniData. Their order determines the first error when both inputs are malformed.',
 'Resolve the wallet and pass the public key’s canonical 0x string and data object to signData.',
 'The wallet owns key lookup and stateful signing-use updates; this command does not implement either policy itself.',
 'Serialize the resulting Signature through MiniData.getMiniDataVersion and return its hex bytes.',
 'Signing can mutate wallet key usage before a later serialization failure. Proposed failure tests must observe both the returned error and retained usage state.'
], 'Why is verifying the returned signature insufficient to prove correct key-use persistence? What should a two-call wallet fixture record in addition to signature bytes?')
