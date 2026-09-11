# All 457 source-file lessons

Each link corresponds to exactly one pinned Java file. **Authored** means the page has a student-level explanation; **reading draft** still needs that work. **Test explanation** describes source tests and their observed assertions, with its own limits. None of these labels certifies full C++ parity.

Current documentation: 369 authored production lessons, 0 production reading drafts, 88 original-test explanations. See [audit data](coverage-audit.json).

[Course introduction](README.md) · [How to study](how-to-study-this-port.md) · [Checker rejection lab](new-tests/checker-rejections.md) · [Command runner error lab](new-tests/command-runner-errors.md) · [Wallet command lab](new-tests/wallet-commands.md) · [Address command lab](new-tests/address-commands.md) · [Witness command lab](new-tests/witness-commands.md) · [Coin ownership lab](new-tests/coin-ownership.md) · [Coin and proof reader lab](new-tests/coin-streams.md) · [Chain command lab](new-tests/chain-commands.md) · [Transaction-input lab](new-tests/transaction-inputs.md) · [Transaction-check lab](new-tests/transaction-check.md) · [Increment/decrement lab](new-tests/increment-decrement.md) · [Mining/posting lab](new-tests/transaction-mining.md) · [Powers/significant digits lab](new-tests/power-significant-digits.md) · [Bit-counting lab](new-tests/bit-counting.md) · [Vault/signing lab](new-tests/vault-and-signing.md) · [Bit-get/set lab](new-tests/bit-get-set.md) · [Wallet-key lab](new-tests/wallet-keys.md) · [Coin-funding lab](new-tests/coin-selection-and-funding.md) · [Reverse/length lab](new-tests/reverse-and-length.md) · [Offline-file lab](new-tests/offline-send-files.md) · [Unsigned-file lab](new-tests/unsigned-transaction-files.md) · [Send-polling lab](new-tests/send-polling-command.md) · [Wallet orchestration lab](new-tests/wallet-command-orchestration.md) · [Raw-transaction lab](new-tests/raw-transaction-commands.md) · [Custom-address sending lab](new-tests/custom-address-send-commands.md) · [Consolidation lab](new-tests/consolidate-custom-address.md) · [Wallet consolidation lab](new-tests/wallet-consolidation.md) · [Wallet balance lab](new-tests/wallet-balance.md) · [Coin-search lab](new-tests/coin-search-command.md) · [Transaction-history lab](new-tests/transaction-history.md) · [TxPoW-search lab](new-tests/txpow-search.md) · [Node-state lab](new-tests/node-state-commands.md) · [Seed derivation lab](new-tests/seed-random-command.md) · [Original cast tests](new-tests/cast-original-tests.md) · [MySQL connection lab](new-tests/mysql-connection.md) · [Main runtime lab](new-tests/main-runtime.md)

## src/org/minima

| File | Teaching state | Native state |
|---|---|---|
| [Minima.java](files/src/org/minima/Minima.md) | authored | implemented_partial |

## src/org/minima/database

| File | Teaching state | Native state |
|---|---|---|
| [MinimaDB.java](files/src/org/minima/database/MinimaDB.md) | authored | implemented_partial |

## src/org/minima/database/archive

| File | Teaching state | Native state |
|---|---|---|
| [ArchiveManager.java](files/src/org/minima/database/archive/ArchiveManager.md) | authored | implemented_partial |
| [RawArchiveInput.java](files/src/org/minima/database/archive/RawArchiveInput.md) | authored | implemented_partial |
| [TxBlockDB.java](files/src/org/minima/database/archive/TxBlockDB.md) | authored | implemented_partial |

## src/org/minima/database/cascade

| File | Teaching state | Native state |
|---|---|---|
| [Cascade.java](files/src/org/minima/database/cascade/Cascade.md) | authored | implemented_partial |
| [CascadeNode.java](files/src/org/minima/database/cascade/CascadeNode.md) | authored | implemented_partial |

## src/org/minima/database/txpowdb

| File | Teaching state | Native state |
|---|---|---|
| [TxPoWDB.java](files/src/org/minima/database/txpowdb/TxPoWDB.md) | authored | implemented_partial |

## src/org/minima/database/txpowdb/onchain

| File | Teaching state | Native state |
|---|---|---|
| [TxPoWOnChainDB.java](files/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.md) | authored | implemented_partial |

## src/org/minima/database/txpowdb/ram

| File | Teaching state | Native state |
|---|---|---|
| [RamDB.java](files/src/org/minima/database/txpowdb/ram/RamDB.md) | authored | implemented_partial |
| [RamData.java](files/src/org/minima/database/txpowdb/ram/RamData.md) | authored | implemented_partial |

## src/org/minima/database/txpowdb/sql

| File | Teaching state | Native state |
|---|---|---|
| [TxPoWList.java](files/src/org/minima/database/txpowdb/sql/TxPoWList.md) | authored | implemented_partial |
| [TxPoWSqlDB.java](files/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.md) | authored | implemented_partial |

## src/org/minima/database/txpowtree

| File | Teaching state | Native state |
|---|---|---|
| [CoinDB.java](files/src/org/minima/database/txpowtree/CoinDB.md) | authored | implemented_partial |
| [TxPoWTreeNode.java](files/src/org/minima/database/txpowtree/TxPoWTreeNode.md) | authored | implemented_partial |
| [TxPoWTreeNodeAction.java](files/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.md) | authored | implemented_partial |
| [TxPowTree.java](files/src/org/minima/database/txpowtree/TxPowTree.md) | authored | implemented_partial |

## src/org/minima/database/userprefs

| File | Teaching state | Native state |
|---|---|---|
| [UserDB.java](files/src/org/minima/database/userprefs/UserDB.md) | authored | implemented_partial |

## src/org/minima/database/userprefs/txndb

| File | Teaching state | Native state |
|---|---|---|
| [TxnDB.java](files/src/org/minima/database/userprefs/txndb/TxnDB.md) | authored | implemented_partial |
| [TxnRow.java](files/src/org/minima/database/userprefs/txndb/TxnRow.md) | authored | implemented_partial |

## src/org/minima/database/wallet

| File | Teaching state | Native state |
|---|---|---|
| [KeyRow.java](files/src/org/minima/database/wallet/KeyRow.md) | authored | implemented_partial |
| [ScriptRow.java](files/src/org/minima/database/wallet/ScriptRow.md) | authored | implemented_partial |
| [SeedRow.java](files/src/org/minima/database/wallet/SeedRow.md) | authored | implemented_partial |
| [Wallet.java](files/src/org/minima/database/wallet/Wallet.md) | authored | implemented_partial |

## src/org/minima/kissvm

| File | Teaching state | Native state |
|---|---|---|
| [Contract.java](files/src/org/minima/kissvm/Contract.md) | authored | implemented_partial |

## src/org/minima/kissvm/exceptions

| File | Teaching state | Native state |
|---|---|---|
| [ExecutionException.java](files/src/org/minima/kissvm/exceptions/ExecutionException.md) | authored | implemented_partial |
| [MinimaException.java](files/src/org/minima/kissvm/exceptions/MinimaException.md) | authored | implemented_partial |
| [MinimaParseException.java](files/src/org/minima/kissvm/exceptions/MinimaParseException.md) | authored | implemented_partial |

## src/org/minima/kissvm/expressions

| File | Teaching state | Native state |
|---|---|---|
| [BooleanExpression.java](files/src/org/minima/kissvm/expressions/BooleanExpression.md) | authored | implemented_partial |
| [ConstantExpression.java](files/src/org/minima/kissvm/expressions/ConstantExpression.md) | authored | implemented_partial |
| [Expression.java](files/src/org/minima/kissvm/expressions/Expression.md) | authored | implemented_partial |
| [ExpressionParser.java](files/src/org/minima/kissvm/expressions/ExpressionParser.md) | authored | implemented_partial |
| [FunctionExpression.java](files/src/org/minima/kissvm/expressions/FunctionExpression.md) | authored | implemented_partial |
| [GlobalExpression.java](files/src/org/minima/kissvm/expressions/GlobalExpression.md) | authored | implemented_partial |
| [OperatorExpression.java](files/src/org/minima/kissvm/expressions/OperatorExpression.md) | authored | implemented_partial |
| [VariableExpression.java](files/src/org/minima/kissvm/expressions/VariableExpression.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions

| File | Teaching state | Native state |
|---|---|---|
| [MinimaFunction.java](files/src/org/minima/kissvm/functions/MinimaFunction.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/cast

| File | Teaching state | Native state |
|---|---|---|
| [ASCII.java](files/src/org/minima/kissvm/functions/cast/ASCII.md) | authored | implemented_partial |
| [BOOL.java](files/src/org/minima/kissvm/functions/cast/BOOL.md) | authored | implemented_partial |
| [HEX.java](files/src/org/minima/kissvm/functions/cast/HEX.md) | authored | implemented_partial |
| [NUMBER.java](files/src/org/minima/kissvm/functions/cast/NUMBER.md) | authored | implemented_partial |
| [STRING.java](files/src/org/minima/kissvm/functions/cast/STRING.md) | authored | implemented_partial |
| [UTF8.java](files/src/org/minima/kissvm/functions/cast/UTF8.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/general

| File | Teaching state | Native state |
|---|---|---|
| [ADDRESS.java](files/src/org/minima/kissvm/functions/general/ADDRESS.md) | authored | implemented_partial |
| [EXISTS.java](files/src/org/minima/kissvm/functions/general/EXISTS.md) | authored | implemented_partial |
| [FUNCTION.java](files/src/org/minima/kissvm/functions/general/FUNCTION.md) | authored | implemented_partial |
| [GET.java](files/src/org/minima/kissvm/functions/general/GET.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/hex

| File | Teaching state | Native state |
|---|---|---|
| [BITCOUNT.java](files/src/org/minima/kissvm/functions/hex/BITCOUNT.md) | authored | implemented_partial |
| [BITGET.java](files/src/org/minima/kissvm/functions/hex/BITGET.md) | authored | implemented_partial |
| [BITSET.java](files/src/org/minima/kissvm/functions/hex/BITSET.md) | authored | implemented_partial |
| [CONCAT.java](files/src/org/minima/kissvm/functions/hex/CONCAT.md) | authored | implemented_partial |
| [LEN.java](files/src/org/minima/kissvm/functions/hex/LEN.md) | authored | implemented_partial |
| [OVERWRITE.java](files/src/org/minima/kissvm/functions/hex/OVERWRITE.md) | authored | implemented_partial |
| [REV.java](files/src/org/minima/kissvm/functions/hex/REV.md) | authored | implemented_partial |
| [SETLEN.java](files/src/org/minima/kissvm/functions/hex/SETLEN.md) | authored | implemented_partial |
| [SUBSET.java](files/src/org/minima/kissvm/functions/hex/SUBSET.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/number

| File | Teaching state | Native state |
|---|---|---|
| [ABS.java](files/src/org/minima/kissvm/functions/number/ABS.md) | authored | implemented_partial |
| [CEIL.java](files/src/org/minima/kissvm/functions/number/CEIL.md) | authored | implemented_partial |
| [DEC.java](files/src/org/minima/kissvm/functions/number/DEC.md) | authored | implemented_partial |
| [FLOOR.java](files/src/org/minima/kissvm/functions/number/FLOOR.md) | authored | implemented_partial |
| [INC.java](files/src/org/minima/kissvm/functions/number/INC.md) | authored | implemented_partial |
| [MAX.java](files/src/org/minima/kissvm/functions/number/MAX.md) | authored | implemented_partial |
| [MIN.java](files/src/org/minima/kissvm/functions/number/MIN.md) | authored | implemented_partial |
| [POW.java](files/src/org/minima/kissvm/functions/number/POW.md) | authored | implemented_partial |
| [SIGDIG.java](files/src/org/minima/kissvm/functions/number/SIGDIG.md) | authored | implemented_partial |
| [SQRT.java](files/src/org/minima/kissvm/functions/number/SQRT.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/sha

| File | Teaching state | Native state |
|---|---|---|
| [PROOF.java](files/src/org/minima/kissvm/functions/sha/PROOF.md) | authored | implemented_partial |
| [SHA2.java](files/src/org/minima/kissvm/functions/sha/SHA2.md) | authored | implemented_partial |
| [SHA3.java](files/src/org/minima/kissvm/functions/sha/SHA3.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/sigs

| File | Teaching state | Native state |
|---|---|---|
| [CHECKSIG.java](files/src/org/minima/kissvm/functions/sigs/CHECKSIG.md) | authored | implemented_partial |
| [MULTISIG.java](files/src/org/minima/kissvm/functions/sigs/MULTISIG.md) | authored | implemented_partial |
| [SIGNEDBY.java](files/src/org/minima/kissvm/functions/sigs/SIGNEDBY.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/state

| File | Teaching state | Native state |
|---|---|---|
| [PREVSTATE.java](files/src/org/minima/kissvm/functions/state/PREVSTATE.md) | authored | implemented_partial |
| [SAMESTATE.java](files/src/org/minima/kissvm/functions/state/SAMESTATE.md) | authored | implemented_partial |
| [STATE.java](files/src/org/minima/kissvm/functions/state/STATE.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/string

| File | Teaching state | Native state |
|---|---|---|
| [REPLACE.java](files/src/org/minima/kissvm/functions/string/REPLACE.md) | authored | implemented_partial |
| [REPLACEFIRST.java](files/src/org/minima/kissvm/functions/string/REPLACEFIRST.md) | authored | implemented_partial |
| [SUBSTR.java](files/src/org/minima/kissvm/functions/string/SUBSTR.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/txn/input

| File | Teaching state | Native state |
|---|---|---|
| [GETINADDR.java](files/src/org/minima/kissvm/functions/txn/input/GETINADDR.md) | authored | implemented_partial |
| [GETINAMT.java](files/src/org/minima/kissvm/functions/txn/input/GETINAMT.md) | authored | implemented_partial |
| [GETINID.java](files/src/org/minima/kissvm/functions/txn/input/GETINID.md) | authored | implemented_partial |
| [GETINTOK.java](files/src/org/minima/kissvm/functions/txn/input/GETINTOK.md) | authored | implemented_partial |
| [SUMINPUTS.java](files/src/org/minima/kissvm/functions/txn/input/SUMINPUTS.md) | authored | implemented_partial |
| [VERIFYIN.java](files/src/org/minima/kissvm/functions/txn/input/VERIFYIN.md) | authored | implemented_partial |

## src/org/minima/kissvm/functions/txn/output

| File | Teaching state | Native state |
|---|---|---|
| [GETOUTADDR.java](files/src/org/minima/kissvm/functions/txn/output/GETOUTADDR.md) | authored | implemented_partial |
| [GETOUTAMT.java](files/src/org/minima/kissvm/functions/txn/output/GETOUTAMT.md) | authored | implemented_partial |
| [GETOUTKEEPSTATE.java](files/src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.md) | authored | implemented_partial |
| [GETOUTTOK.java](files/src/org/minima/kissvm/functions/txn/output/GETOUTTOK.md) | authored | implemented_partial |
| [SUMOUTPUTS.java](files/src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.md) | authored | implemented_partial |
| [VERIFYOUT.java](files/src/org/minima/kissvm/functions/txn/output/VERIFYOUT.md) | authored | implemented_partial |

## src/org/minima/kissvm/statements

| File | Teaching state | Native state |
|---|---|---|
| [Statement.java](files/src/org/minima/kissvm/statements/Statement.md) | authored | implemented_partial |
| [StatementBlock.java](files/src/org/minima/kissvm/statements/StatementBlock.md) | authored | implemented_partial |
| [StatementParser.java](files/src/org/minima/kissvm/statements/StatementParser.md) | authored | implemented_partial |

## src/org/minima/kissvm/statements/commands

| File | Teaching state | Native state |
|---|---|---|
| [ASSERTstatement.java](files/src/org/minima/kissvm/statements/commands/ASSERTstatement.md) | authored | implemented_partial |
| [EXECstatement.java](files/src/org/minima/kissvm/statements/commands/EXECstatement.md) | authored | implemented_partial |
| [IFstatement.java](files/src/org/minima/kissvm/statements/commands/IFstatement.md) | authored | implemented_partial |
| [LETstatement.java](files/src/org/minima/kissvm/statements/commands/LETstatement.md) | authored | implemented_partial |
| [MASTstatement.java](files/src/org/minima/kissvm/statements/commands/MASTstatement.md) | authored | implemented_partial |
| [RETURNstatement.java](files/src/org/minima/kissvm/statements/commands/RETURNstatement.md) | authored | implemented_partial |
| [WHILEstatement.java](files/src/org/minima/kissvm/statements/commands/WHILEstatement.md) | authored | implemented_partial |

## src/org/minima/kissvm/tokens

| File | Teaching state | Native state |
|---|---|---|
| [LexicalTokenizer.java](files/src/org/minima/kissvm/tokens/LexicalTokenizer.md) | authored | implemented_partial |
| [ScriptToken.java](files/src/org/minima/kissvm/tokens/ScriptToken.md) | authored | implemented_partial |
| [ScriptTokenizer.java](files/src/org/minima/kissvm/tokens/ScriptTokenizer.md) | authored | implemented_partial |

## src/org/minima/kissvm/values

| File | Teaching state | Native state |
|---|---|---|
| [BooleanValue.java](files/src/org/minima/kissvm/values/BooleanValue.md) | authored | implemented_partial |
| [HexValue.java](files/src/org/minima/kissvm/values/HexValue.md) | authored | implemented_partial |
| [NumberValue.java](files/src/org/minima/kissvm/values/NumberValue.md) | authored | implemented_partial |
| [StringValue.java](files/src/org/minima/kissvm/values/StringValue.md) | authored | implemented_partial |
| [Value.java](files/src/org/minima/kissvm/values/Value.md) | authored | implemented_partial |

## src/org/minima/objects

| File | Teaching state | Native state |
|---|---|---|
| [Address.java](files/src/org/minima/objects/Address.md) | authored | implemented_partial |
| [Coin.java](files/src/org/minima/objects/Coin.md) | authored | implemented_partial |
| [CoinProof.java](files/src/org/minima/objects/CoinProof.md) | authored | implemented_partial |
| [Greeting.java](files/src/org/minima/objects/Greeting.md) | authored | implemented_partial |
| [IBD.java](files/src/org/minima/objects/IBD.md) | authored | implemented_partial |
| [Magic.java](files/src/org/minima/objects/Magic.md) | authored | implemented_partial |
| [Pulse.java](files/src/org/minima/objects/Pulse.md) | authored | implemented_partial |
| [ScriptProof.java](files/src/org/minima/objects/ScriptProof.md) | authored | implemented_partial |
| [StateVariable.java](files/src/org/minima/objects/StateVariable.md) | authored | implemented_partial |
| [Token.java](files/src/org/minima/objects/Token.md) | authored | implemented_partial |
| [Transaction.java](files/src/org/minima/objects/Transaction.md) | authored | implemented_partial |
| [TxBlock.java](files/src/org/minima/objects/TxBlock.md) | authored | implemented_partial |
| [TxBody.java](files/src/org/minima/objects/TxBody.md) | authored | implemented_partial |
| [TxHeader.java](files/src/org/minima/objects/TxHeader.md) | authored | implemented_partial |
| [TxPoW.java](files/src/org/minima/objects/TxPoW.md) | authored | implemented_partial |
| [Witness.java](files/src/org/minima/objects/Witness.md) | authored | implemented_partial |

## src/org/minima/objects/base

| File | Teaching state | Native state |
|---|---|---|
| [MiniByte.java](files/src/org/minima/objects/base/MiniByte.md) | authored | implemented_partial |
| [MiniData.java](files/src/org/minima/objects/base/MiniData.md) | authored | implemented_partial |
| [MiniNumber.java](files/src/org/minima/objects/base/MiniNumber.md) | authored | implemented_partial |
| [MiniString.java](files/src/org/minima/objects/base/MiniString.md) | authored | implemented_partial |

## src/org/minima/objects/keys

| File | Teaching state | Native state |
|---|---|---|
| [Signature.java](files/src/org/minima/objects/keys/Signature.md) | authored | implemented_partial |
| [SignatureProof.java](files/src/org/minima/objects/keys/SignatureProof.md) | authored | implemented_partial |
| [TreeKey.java](files/src/org/minima/objects/keys/TreeKey.md) | authored | implemented_partial |
| [TreeKeyNode.java](files/src/org/minima/objects/keys/TreeKeyNode.md) | authored | implemented_partial |
| [Winternitz.java](files/src/org/minima/objects/keys/Winternitz.md) | authored | implemented_partial |

## src/org/minima/objects/mmr

| File | Teaching state | Native state |
|---|---|---|
| [MMR.java](files/src/org/minima/objects/mmr/MMR.md) | authored | implemented_partial |
| [MMRData.java](files/src/org/minima/objects/mmr/MMRData.md) | authored | implemented_partial |
| [MMREntry.java](files/src/org/minima/objects/mmr/MMREntry.md) | authored | implemented_partial |
| [MMREntryNumber.java](files/src/org/minima/objects/mmr/MMREntryNumber.md) | authored | implemented_partial |
| [MMRProof.java](files/src/org/minima/objects/mmr/MMRProof.md) | authored | implemented_partial |
| [MegaMMR.java](files/src/org/minima/objects/mmr/MegaMMR.md) | authored | implemented_partial |

## src/org/minima/system

| File | Teaching state | Native state |
|---|---|---|
| [Main.java](files/src/org/minima/system/Main.md) | authored | implemented_partial |

## src/org/minima/system/brains

| File | Teaching state | Native state |
|---|---|---|
| [TimedChecker.java](files/src/org/minima/system/brains/TimedChecker.md) | authored | implemented_partial |
| [TxPoWChecker.java](files/src/org/minima/system/brains/TxPoWChecker.md) | authored | implemented_partial |
| [TxPoWGenerator.java](files/src/org/minima/system/brains/TxPoWGenerator.md) | authored | implemented_partial |
| [TxPoWMiner.java](files/src/org/minima/system/brains/TxPoWMiner.md) | authored | implemented_partial |
| [TxPoWProcessor.java](files/src/org/minima/system/brains/TxPoWProcessor.md) | authored | implemented_partial |
| [TxPoWSearcher.java](files/src/org/minima/system/brains/TxPoWSearcher.md) | authored | implemented_partial |

## src/org/minima/system/commands

| File | Teaching state | Native state |
|---|---|---|
| [Command.java](files/src/org/minima/system/commands/Command.md) | authored | implemented_partial |
| [CommandException.java](files/src/org/minima/system/commands/CommandException.md) | authored | implemented_partial |
| [CommandRunner.java](files/src/org/minima/system/commands/CommandRunner.md) | authored | implemented_partial |
| [help.java](files/src/org/minima/system/commands/help.md) | authored | unported |
| [tutorial.java](files/src/org/minima/system/commands/tutorial.md) | authored | unported |
| [whitepaper.java](files/src/org/minima/system/commands/whitepaper.md) | authored | unported |

## src/org/minima/system/commands/backup

| File | Teaching state | Native state |
|---|---|---|
| [archive.java](files/src/org/minima/system/commands/backup/archive.md) | authored | implemented_partial |
| [backup.java](files/src/org/minima/system/commands/backup/backup.md) | authored | implemented_partial |
| [decryptbackup.java](files/src/org/minima/system/commands/backup/decryptbackup.md) | authored | implemented_partial |
| [mysql.java](files/src/org/minima/system/commands/backup/mysql.md) | authored | unported |
| [mysqlcoins.java](files/src/org/minima/system/commands/backup/mysqlcoins.md) | authored | unported |
| [rescue.java](files/src/org/minima/system/commands/backup/rescue.md) | authored | implemented_partial |
| [reset.java](files/src/org/minima/system/commands/backup/reset.md) | authored | implemented_partial |
| [restore.java](files/src/org/minima/system/commands/backup/restore.md) | authored | implemented_partial |
| [restoresync.java](files/src/org/minima/system/commands/backup/restoresync.md) | authored | implemented_partial |
| [vault.java](files/src/org/minima/system/commands/backup/vault.md) | authored | implemented_partial |

## src/org/minima/system/commands/backup/mmrsync

| File | Teaching state | Native state |
|---|---|---|
| [MegaMMRBackup.java](files/src/org/minima/system/commands/backup/mmrsync/MegaMMRBackup.md) | authored | implemented_partial |
| [MegaMMRIBD.java](files/src/org/minima/system/commands/backup/mmrsync/MegaMMRIBD.md) | authored | implemented_partial |
| [MegaMMRSyncData.java](files/src/org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.md) | authored | implemented_partial |
| [megammr.java](files/src/org/minima/system/commands/backup/mmrsync/megammr.md) | authored | implemented_partial |
| [megammrsync.java](files/src/org/minima/system/commands/backup/mmrsync/megammrsync.md) | authored | implemented_partial |

## src/org/minima/system/commands/base

| File | Teaching state | Native state |
|---|---|---|
| [automine.java](files/src/org/minima/system/commands/base/automine.md) | authored | implemented_partial |
| [balance.java](files/src/org/minima/system/commands/base/balance.md) | authored | implemented_partial |
| [benchmark.java](files/src/org/minima/system/commands/base/benchmark.md) | authored | unported |
| [block.java](files/src/org/minima/system/commands/base/block.md) | authored | implemented_partial |
| [burn.java](files/src/org/minima/system/commands/base/burn.md) | authored | implemented_partial |
| [checkaddress.java](files/src/org/minima/system/commands/base/checkaddress.md) | authored | implemented_partial |
| [coincheck.java](files/src/org/minima/system/commands/base/coincheck.md) | authored | implemented_partial |
| [coinexport.java](files/src/org/minima/system/commands/base/coinexport.md) | authored | implemented_partial |
| [coinimport.java](files/src/org/minima/system/commands/base/coinimport.md) | authored | implemented_partial |
| [coinnotify.java](files/src/org/minima/system/commands/base/coinnotify.md) | authored | implemented_partial |
| [cointrack.java](files/src/org/minima/system/commands/base/cointrack.md) | authored | implemented_partial |
| [consolidate.java](files/src/org/minima/system/commands/base/consolidate.md) | authored | implemented_partial |
| [convert.java](files/src/org/minima/system/commands/base/convert.md) | authored | implemented_partial |
| [debugflag.java](files/src/org/minima/system/commands/base/debugflag.md) | authored | implemented_partial |
| [getaddress.java](files/src/org/minima/system/commands/base/getaddress.md) | authored | implemented_partial |
| [hash.java](files/src/org/minima/system/commands/base/hash.md) | authored | implemented_partial |
| [hashtest.java](files/src/org/minima/system/commands/base/hashtest.md) | authored | implemented_partial |
| [healthcheck.java](files/src/org/minima/system/commands/base/healthcheck.md) | authored | implemented_partial |
| [incentivecash.java](files/src/org/minima/system/commands/base/incentivecash.md) | authored | implemented_partial |
| [jniminingtest.java](files/src/org/minima/system/commands/base/jniminingtest.md) | authored | unported |
| [jnitest.java](files/src/org/minima/system/commands/base/jnitest.md) | authored | unported |
| [logs.java](files/src/org/minima/system/commands/base/logs.md) | authored | implemented_partial |
| [magic.java](files/src/org/minima/system/commands/base/magic.md) | authored | implemented_partial |
| [maths.java](files/src/org/minima/system/commands/base/maths.md) | authored | implemented_partial |
| [mempool.java](files/src/org/minima/system/commands/base/mempool.md) | authored | implemented_partial |
| [missingcmd.java](files/src/org/minima/system/commands/base/missingcmd.md) | authored | implemented_partial |
| [mmrcreate.java](files/src/org/minima/system/commands/base/mmrcreate.md) | authored | implemented_partial |
| [mmrproof.java](files/src/org/minima/system/commands/base/mmrproof.md) | authored | implemented_partial |
| [newaddress.java](files/src/org/minima/system/commands/base/newaddress.md) | authored | implemented_partial |
| [printmmr.java](files/src/org/minima/system/commands/base/printmmr.md) | authored | implemented_partial |
| [printtree.java](files/src/org/minima/system/commands/base/printtree.md) | authored | implemented_partial |
| [quit.java](files/src/org/minima/system/commands/base/quit.md) | authored | implemented_partial |
| [random.java](files/src/org/minima/system/commands/base/random.md) | authored | implemented_partial |
| [scanchain.java](files/src/org/minima/system/commands/base/scanchain.md) | authored | implemented_partial |
| [seedrandom.java](files/src/org/minima/system/commands/base/seedrandom.md) | authored | implemented_partial |
| [slavenode.java](files/src/org/minima/system/commands/base/slavenode.md) | authored | implemented_partial |
| [sphincs.java](files/src/org/minima/system/commands/base/sphincs.md) | authored | unported |
| [status.java](files/src/org/minima/system/commands/base/status.md) | authored | implemented_partial |
| [systemcheck.java](files/src/org/minima/system/commands/base/systemcheck.md) | authored | unported |
| [test.java](files/src/org/minima/system/commands/base/test.md) | authored | unported |
| [timemilli.java](files/src/org/minima/system/commands/base/timemilli.md) | authored | implemented_partial |
| [tokencreate.java](files/src/org/minima/system/commands/base/tokencreate.md) | authored | implemented_partial |
| [tokenvalidate.java](files/src/org/minima/system/commands/base/tokenvalidate.md) | authored | implemented_partial |
| [trace.java](files/src/org/minima/system/commands/base/trace.md) | authored | implemented_partial |
| [tutorial.java](files/src/org/minima/system/commands/base/tutorial.md) | authored | unported |

## src/org/minima/system/commands/network

| File | Teaching state | Native state |
|---|---|---|
| [connect.java](files/src/org/minima/system/commands/network/connect.md) | authored | implemented_partial |
| [disconnect.java](files/src/org/minima/system/commands/network/disconnect.md) | authored | implemented_partial |
| [message.java](files/src/org/minima/system/commands/network/message.md) | authored | implemented_partial |
| [network.java](files/src/org/minima/system/commands/network/network.md) | authored | implemented_partial |
| [nodecount.java](files/src/org/minima/system/commands/network/nodecount.md) | authored | implemented_partial |
| [p2pstate.java](files/src/org/minima/system/commands/network/p2pstate.md) | authored | implemented_partial |
| [peers.java](files/src/org/minima/system/commands/network/peers.md) | authored | implemented_partial |
| [ping.java](files/src/org/minima/system/commands/network/ping.md) | authored | implemented_partial |
| [rpc.java](files/src/org/minima/system/commands/network/rpc.md) | authored | implemented_partial |
| [webhooks.java](files/src/org/minima/system/commands/network/webhooks.md) | authored | implemented_partial |

## src/org/minima/system/commands/scripts

| File | Teaching state | Native state |
|---|---|---|
| [newscript.java](files/src/org/minima/system/commands/scripts/newscript.md) | authored | implemented_partial |
| [removescript.java](files/src/org/minima/system/commands/scripts/removescript.md) | authored | implemented_partial |
| [runscript.java](files/src/org/minima/system/commands/scripts/runscript.md) | authored | implemented_partial |
| [scripts.java](files/src/org/minima/system/commands/scripts/scripts.md) | authored | implemented_partial |

## src/org/minima/system/commands/search

| File | Teaching state | Native state |
|---|---|---|
| [coins.java](files/src/org/minima/system/commands/search/coins.md) | authored | implemented_partial |
| [history.java](files/src/org/minima/system/commands/search/history.md) | authored | implemented_partial |
| [keys.java](files/src/org/minima/system/commands/search/keys.md) | authored | implemented_partial |
| [tokens.java](files/src/org/minima/system/commands/search/tokens.md) | authored | implemented_partial |
| [txpow.java](files/src/org/minima/system/commands/search/txpow.md) | authored | implemented_partial |

## src/org/minima/system/commands/send

| File | Teaching state | Native state |
|---|---|---|
| [multisig.java](files/src/org/minima/system/commands/send/multisig.md) | authored | implemented_partial |
| [multisigread.java](files/src/org/minima/system/commands/send/multisigread.md) | authored | implemented_partial |
| [send.java](files/src/org/minima/system/commands/send/send.md) | authored | implemented_partial |
| [sendnosign.java](files/src/org/minima/system/commands/send/sendnosign.md) | authored | implemented_partial |
| [sendpoll.java](files/src/org/minima/system/commands/send/sendpoll.md) | authored | implemented_partial |
| [sendpost.java](files/src/org/minima/system/commands/send/sendpost.md) | authored | implemented_partial |
| [sendsign.java](files/src/org/minima/system/commands/send/sendsign.md) | authored | implemented_partial |
| [sendview.java](files/src/org/minima/system/commands/send/sendview.md) | authored | implemented_partial |

## src/org/minima/system/commands/send/wallet

| File | Teaching state | Native state |
|---|---|---|
| [consolidatefrom.java](files/src/org/minima/system/commands/send/wallet/consolidatefrom.md) | authored | implemented_partial |
| [constructfrom.java](files/src/org/minima/system/commands/send/wallet/constructfrom.md) | authored | implemented_partial |
| [createfrom.java](files/src/org/minima/system/commands/send/wallet/createfrom.md) | authored | implemented_partial |
| [createtokenfrom.java](files/src/org/minima/system/commands/send/wallet/createtokenfrom.md) | authored | implemented_partial |
| [postfrom.java](files/src/org/minima/system/commands/send/wallet/postfrom.md) | authored | implemented_partial |
| [rawfrom.java](files/src/org/minima/system/commands/send/wallet/rawfrom.md) | authored | implemented_partial |
| [rawtxnfrom.java](files/src/org/minima/system/commands/send/wallet/rawtxnfrom.md) | authored | implemented_partial |
| [sendfrom.java](files/src/org/minima/system/commands/send/wallet/sendfrom.md) | authored | implemented_partial |
| [signfrom.java](files/src/org/minima/system/commands/send/wallet/signfrom.md) | authored | implemented_partial |

## src/org/minima/system/commands/sendpoll

| File | Teaching state | Native state |
|---|---|---|
| [SendPollManager.java](files/src/org/minima/system/commands/sendpoll/SendPollManager.md) | authored | implemented_partial |
| [SendPollMessage.java](files/src/org/minima/system/commands/sendpoll/SendPollMessage.md) | authored | implemented_partial |

## src/org/minima/system/commands/signatures

| File | Teaching state | Native state |
|---|---|---|
| [sign.java](files/src/org/minima/system/commands/signatures/sign.md) | authored | implemented_partial |
| [verify.java](files/src/org/minima/system/commands/signatures/verify.md) | authored | implemented_partial |

## src/org/minima/system/commands/txn

| File | Teaching state | Native state |
|---|---|---|
| [txnaddamount.java](files/src/org/minima/system/commands/txn/txnaddamount.md) | authored | implemented_partial |
| [txnauto.java](files/src/org/minima/system/commands/txn/txnauto.md) | authored | implemented_partial |
| [txnbasics.java](files/src/org/minima/system/commands/txn/txnbasics.md) | authored | implemented_partial |
| [txncheck.java](files/src/org/minima/system/commands/txn/txncheck.md) | authored | implemented_partial |
| [txnclear.java](files/src/org/minima/system/commands/txn/txnclear.md) | authored | implemented_partial |
| [txncoinlock.java](files/src/org/minima/system/commands/txn/txncoinlock.md) | authored | implemented_partial |
| [txncreate.java](files/src/org/minima/system/commands/txn/txncreate.md) | authored | implemented_partial |
| [txndelete.java](files/src/org/minima/system/commands/txn/txndelete.md) | authored | implemented_partial |
| [txnexport.java](files/src/org/minima/system/commands/txn/txnexport.md) | authored | implemented_partial |
| [txnimport.java](files/src/org/minima/system/commands/txn/txnimport.md) | authored | implemented_partial |
| [txninput.java](files/src/org/minima/system/commands/txn/txninput.md) | authored | implemented_partial |
| [txnlist.java](files/src/org/minima/system/commands/txn/txnlist.md) | authored | implemented_partial |
| [txnlock.java](files/src/org/minima/system/commands/txn/txnlock.md) | authored | implemented_partial |
| [txnmine.java](files/src/org/minima/system/commands/txn/txnmine.md) | authored | implemented_partial |
| [txnminepost.java](files/src/org/minima/system/commands/txn/txnminepost.md) | authored | implemented_partial |
| [txnmmr.java](files/src/org/minima/system/commands/txn/txnmmr.md) | authored | implemented_partial |
| [txnoutput.java](files/src/org/minima/system/commands/txn/txnoutput.md) | authored | implemented_partial |
| [txnpost.java](files/src/org/minima/system/commands/txn/txnpost.md) | authored | implemented_partial |
| [txnscript.java](files/src/org/minima/system/commands/txn/txnscript.md) | authored | implemented_partial |
| [txnsign.java](files/src/org/minima/system/commands/txn/txnsign.md) | authored | implemented_partial |
| [txnstate.java](files/src/org/minima/system/commands/txn/txnstate.md) | authored | implemented_partial |
| [txnutils.java](files/src/org/minima/system/commands/txn/txnutils.md) | authored | implemented_partial |
| [txnview.java](files/src/org/minima/system/commands/txn/txnview.md) | authored | implemented_partial |

## src/org/minima/system/genesis

| File | Teaching state | Native state |
|---|---|---|
| [GenesisCoin.java](files/src/org/minima/system/genesis/GenesisCoin.md) | authored | implemented_partial |
| [GenesisMMR.java](files/src/org/minima/system/genesis/GenesisMMR.md) | authored | implemented_partial |
| [GenesisTxPoW.java](files/src/org/minima/system/genesis/GenesisTxPoW.md) | authored | implemented_partial |

## src/org/minima/system/network

| File | Teaching state | Native state |
|---|---|---|
| [NetworkManager.java](files/src/org/minima/system/network/NetworkManager.md) | authored | implemented_partial |

## src/org/minima/system/network/minima

| File | Teaching state | Native state |
|---|---|---|
| [NIOClient.java](files/src/org/minima/system/network/minima/NIOClient.md) | authored | implemented_partial |
| [NIOClientInfo.java](files/src/org/minima/system/network/minima/NIOClientInfo.md) | authored | implemented_partial |
| [NIOManager.java](files/src/org/minima/system/network/minima/NIOManager.md) | authored | implemented_partial |
| [NIOMessage.java](files/src/org/minima/system/network/minima/NIOMessage.md) | authored | implemented_partial |
| [NIOServer.java](files/src/org/minima/system/network/minima/NIOServer.md) | authored | implemented_partial |
| [NIOTraffic.java](files/src/org/minima/system/network/minima/NIOTraffic.md) | authored | implemented_partial |
| [RelayPolicy.java](files/src/org/minima/system/network/minima/RelayPolicy.md) | authored | implemented_partial |

## src/org/minima/system/network/p2p

| File | Teaching state | Native state |
|---|---|---|
| [P2PDB.java](files/src/org/minima/system/network/p2p/P2PDB.md) | authored | implemented_partial |
| [P2PFunctions.java](files/src/org/minima/system/network/p2p/P2PFunctions.md) | authored | implemented_partial |
| [P2PManager.java](files/src/org/minima/system/network/p2p/P2PManager.md) | authored | implemented_partial |
| [P2PPeersChecker.java](files/src/org/minima/system/network/p2p/P2PPeersChecker.md) | authored | implemented_partial |
| [P2PState.java](files/src/org/minima/system/network/p2p/P2PState.md) | authored | implemented_partial |
| [SwapLinksFunctions.java](files/src/org/minima/system/network/p2p/SwapLinksFunctions.md) | authored | implemented_partial |
| [UtilFuncs.java](files/src/org/minima/system/network/p2p/UtilFuncs.md) | authored | implemented_partial |
| [WalkLinksFuncs.java](files/src/org/minima/system/network/p2p/WalkLinksFuncs.md) | authored | implemented_partial |

## src/org/minima/system/network/p2p/messages

| File | Teaching state | Native state |
|---|---|---|
| [InetSocketAddressIO.java](files/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.md) | authored | implemented_partial |
| [P2PDoSwap.java](files/src/org/minima/system/network/p2p/messages/P2PDoSwap.md) | authored | implemented_partial |
| [P2PGreeting.java](files/src/org/minima/system/network/p2p/messages/P2PGreeting.md) | authored | implemented_partial |
| [P2PSwapLink.java](files/src/org/minima/system/network/p2p/messages/P2PSwapLink.md) | authored | unported |
| [P2PWalkLinks.java](files/src/org/minima/system/network/p2p/messages/P2PWalkLinks.md) | authored | implemented_partial |

## src/org/minima/system/network/p2p/params

| File | Teaching state | Native state |
|---|---|---|
| [P2PParams.java](files/src/org/minima/system/network/p2p/params/P2PParams.md) | authored | implemented_partial |
| [P2PTestParams.java](files/src/org/minima/system/network/p2p/params/P2PTestParams.md) | authored | implemented_partial |

## src/org/minima/system/network/p2p2

| File | Teaching state | Native state |
|---|---|---|
| [P2P2DB.java](files/src/org/minima/system/network/p2p2/P2P2DB.md) | authored | implemented_partial |
| [P2P2Functions.java](files/src/org/minima/system/network/p2p2/P2P2Functions.md) | authored | implemented_partial |
| [P2P2Manager.java](files/src/org/minima/system/network/p2p2/P2P2Manager.md) | authored | implemented_partial |

## src/org/minima/system/network/rpc

| File | Teaching state | Native state |
|---|---|---|
| [Authorizer.java](files/src/org/minima/system/network/rpc/Authorizer.md) | authored | implemented_partial |
| [CMDHandler.java](files/src/org/minima/system/network/rpc/CMDHandler.md) | authored | implemented_partial |
| [HTTPServer.java](files/src/org/minima/system/network/rpc/HTTPServer.md) | authored | implemented_partial |
| [Server.java](files/src/org/minima/system/network/rpc/Server.md) | authored | implemented_partial |

## src/org/minima/system/network/webhooks

| File | Teaching state | Native state |
|---|---|---|
| [NotifyManager.java](files/src/org/minima/system/network/webhooks/NotifyManager.md) | authored | implemented_partial |

## src/org/minima/system/params

| File | Teaching state | Native state |
|---|---|---|
| [GeneralParams.java](files/src/org/minima/system/params/GeneralParams.md) | authored | implemented_partial |
| [GlobalParams.java](files/src/org/minima/system/params/GlobalParams.md) | authored | implemented_partial |
| [ParamConfigurer.java](files/src/org/minima/system/params/ParamConfigurer.md) | authored | implemented_partial |
| [TestParams.java](files/src/org/minima/system/params/TestParams.md) | authored | implemented_partial |

## src/org/minima/utils

| File | Teaching state | Native state |
|---|---|---|
| [BIP39.java](files/src/org/minima/utils/BIP39.md) | authored | implemented_partial |
| [BaseConverter.java](files/src/org/minima/utils/BaseConverter.md) | authored | implemented_partial |
| [Crypto.java](files/src/org/minima/utils/Crypto.md) | authored | implemented_partial |
| [FastByteArrayStream.java](files/src/org/minima/utils/FastByteArrayStream.md) | authored | implemented_partial |
| [JsonDB.java](files/src/org/minima/utils/JsonDB.md) | authored | implemented_partial |
| [Maths.java](files/src/org/minima/utils/Maths.md) | authored | implemented_partial |
| [MiniFile.java](files/src/org/minima/utils/MiniFile.md) | authored | implemented_partial |
| [MiniFormat.java](files/src/org/minima/utils/MiniFormat.md) | authored | implemented_partial |
| [MiniUtil.java](files/src/org/minima/utils/MiniUtil.md) | authored | implemented_partial |
| [MinimaLogger.java](files/src/org/minima/utils/MinimaLogger.md) | authored | implemented_partial |
| [MinimaRPCClient.java](files/src/org/minima/utils/MinimaRPCClient.md) | authored | implemented_partial |
| [MinimaUncaughtException.java](files/src/org/minima/utils/MinimaUncaughtException.md) | authored | implemented_partial |
| [RPCClient.java](files/src/org/minima/utils/RPCClient.md) | authored | implemented_partial |
| [SqlDB.java](files/src/org/minima/utils/SqlDB.md) | authored | implemented_partial |
| [Stack.java](files/src/org/minima/utils/Stack.md) | authored | implemented_partial |
| [Streamable.java](files/src/org/minima/utils/Streamable.md) | authored | unported |
| [TimedRequest.java](files/src/org/minima/utils/TimedRequest.md) | authored | implemented_partial |
| [ZipExtractor.java](files/src/org/minima/utils/ZipExtractor.md) | authored | unported |

## src/org/minima/utils/encrypt

| File | Teaching state | Native state |
|---|---|---|
| [CryptoPackage.java](files/src/org/minima/utils/encrypt/CryptoPackage.md) | authored | implemented_partial |
| [EncryptDecrypt.java](files/src/org/minima/utils/encrypt/EncryptDecrypt.md) | authored | implemented_partial |
| [GenerateKey.java](files/src/org/minima/utils/encrypt/GenerateKey.md) | authored | implemented_partial |
| [PasswordCrypto.java](files/src/org/minima/utils/encrypt/PasswordCrypto.md) | authored | implemented_partial |
| [SignVerify.java](files/src/org/minima/utils/encrypt/SignVerify.md) | authored | implemented_partial |

## src/org/minima/utils/encrypt/javajs

| File | Teaching state | Native state |
|---|---|---|
| [AesUtil.java](files/src/org/minima/utils/encrypt/javajs/AesUtil.md) | authored | unported |

## src/org/minima/utils/jni

| File | Teaching state | Native state |
|---|---|---|
| [jnifunctions.java](files/src/org/minima/utils/jni/jnifunctions.md) | authored | unported |

## src/org/minima/utils/json

| File | Teaching state | Native state |
|---|---|---|
| [ItemList.java](files/src/org/minima/utils/json/ItemList.md) | authored | implemented_partial |
| [JSONArray.java](files/src/org/minima/utils/json/JSONArray.md) | authored | implemented_partial |
| [JSONAware.java](files/src/org/minima/utils/json/JSONAware.md) | authored | implemented_partial |
| [JSONObject.java](files/src/org/minima/utils/json/JSONObject.md) | authored | implemented_partial |
| [JSONStreamAware.java](files/src/org/minima/utils/json/JSONStreamAware.md) | authored | implemented_partial |
| [JSONValue.java](files/src/org/minima/utils/json/JSONValue.md) | authored | implemented_partial |
| [JSONWriter.java](files/src/org/minima/utils/json/JSONWriter.md) | authored | implemented_partial |

## src/org/minima/utils/json/parser

| File | Teaching state | Native state |
|---|---|---|
| [ContainerFactory.java](files/src/org/minima/utils/json/parser/ContainerFactory.md) | authored | implemented_partial |
| [ContentHandler.java](files/src/org/minima/utils/json/parser/ContentHandler.md) | authored | implemented_partial |
| [JSONParser.java](files/src/org/minima/utils/json/parser/JSONParser.md) | authored | implemented_partial |
| [ParseException.java](files/src/org/minima/utils/json/parser/ParseException.md) | authored | implemented_partial |
| [Yylex.java](files/src/org/minima/utils/json/parser/Yylex.md) | authored | implemented_partial |
| [Yytoken.java](files/src/org/minima/utils/json/parser/Yytoken.md) | authored | implemented_partial |

## src/org/minima/utils/messages

| File | Teaching state | Native state |
|---|---|---|
| [Message.java](files/src/org/minima/utils/messages/Message.md) | authored | implemented_partial |
| [MessageListener.java](files/src/org/minima/utils/messages/MessageListener.md) | authored | implemented_partial |
| [MessageProcessor.java](files/src/org/minima/utils/messages/MessageProcessor.md) | authored | implemented_partial |
| [MessageStack.java](files/src/org/minima/utils/messages/MessageStack.md) | authored | implemented_partial |
| [TimerMessage.java](files/src/org/minima/utils/messages/TimerMessage.md) | authored | implemented_partial |
| [TimerProcessor.java](files/src/org/minima/utils/messages/TimerProcessor.md) | authored | implemented_partial |

## src/org/minima/utils/mysql

| File | Teaching state | Native state |
|---|---|---|
| [ArchiveServer.java](files/src/org/minima/utils/mysql/ArchiveServer.md) | authored | unported |
| [MySQLConnect.java](files/src/org/minima/utils/mysql/MySQLConnect.md) | authored | implemented_partial |

## src/org/minima/utils/sphincs/FORS

| File | Teaching state | Native state |
|---|---|---|
| [FORS.java](files/src/org/minima/utils/sphincs/FORS/FORS.md) | authored | unported |
| [FORSSignature.java](files/src/org/minima/utils/sphincs/FORS/FORSSignature.md) | authored | unported |

## src/org/minima/utils/sphincs/HORST

| File | Teaching state | Native state |
|---|---|---|
| [HORST.java](files/src/org/minima/utils/sphincs/HORST/HORST.md) | authored | unported |
| [HORSTSignature.java](files/src/org/minima/utils/sphincs/HORST/HORSTSignature.md) | authored | unported |
| [PrivateKey.java](files/src/org/minima/utils/sphincs/HORST/PrivateKey.md) | authored | unported |
| [PublicKey.java](files/src/org/minima/utils/sphincs/HORST/PublicKey.md) | authored | unported |

## src/org/minima/utils/sphincs

| File | Teaching state | Native state |
|---|---|---|
| [SPHINCS.java](files/src/org/minima/utils/sphincs/SPHINCS.md) | authored | unported |
| [SPHINCSSignature.java](files/src/org/minima/utils/sphincs/SPHINCSSignature.md) | authored | unported |
| [SPHINCSUtils.java](files/src/org/minima/utils/sphincs/SPHINCSUtils.md) | authored | unported |
| [TransactionTest.java](files/src/org/minima/utils/sphincs/TransactionTest.md) | authored | unported |

## test/org/minima/database/txpowdb/ram

| File | Teaching state | Native state |
|---|---|---|
| [RamDBTest.java](files/test/org/minima/database/txpowdb/ram/RamDBTest.md) | test explanation | implemented_partial |

## test/org/minima/database/txpowtree

| File | Teaching state | Native state |
|---|---|---|
| [TxPowTreeTest.java](files/test/org/minima/database/txpowtree/TxPowTreeTest.md) | test explanation | implemented_partial |

## test/org/minima/objects

| File | Teaching state | Native state |
|---|---|---|
| [AddressTests.java](files/test/org/minima/objects/AddressTests.md) | test explanation | unported |
| [CoinTests.java](files/test/org/minima/objects/CoinTests.md) | test explanation | unported |
| [TokenTests.java](files/test/org/minima/objects/TokenTests.md) | test explanation | unported |
| [TransactionTests.java](files/test/org/minima/objects/TransactionTests.md) | test explanation | implemented_partial |
| [TxHeaderTests.java](files/test/org/minima/objects/TxHeaderTests.md) | test explanation | implemented_partial |

## test/org/minima/objects/base

| File | Teaching state | Native state |
|---|---|---|
| [MiniByteTests.java](files/test/org/minima/objects/base/MiniByteTests.md) | test explanation | implemented_partial |
| [MiniDataTests.java](files/test/org/minima/objects/base/MiniDataTests.md) | test explanation | unported |
| [MiniNumberTests.java](files/test/org/minima/objects/base/MiniNumberTests.md) | test explanation | unported |
| [MiniStringTests.java](files/test/org/minima/objects/base/MiniStringTests.md) | test explanation | implemented_partial |

## test/org/minima/system/brains

| File | Teaching state | Native state |
|---|---|---|
| [TxPoWProcessorTest.java](files/test/org/minima/system/brains/TxPoWProcessorTest.md) | test explanation | unported |

## test/org/minima/system/network/p2p

| File | Teaching state | Native state |
|---|---|---|
| [P2PDBTest.java](files/test/org/minima/system/network/p2p/P2PDBTest.md) | test explanation | implemented_partial |
| [SwapLinksFunctionsTest.java](files/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.md) | test explanation | unported |
| [UtilFuncsTest.java](files/test/org/minima/system/network/p2p/UtilFuncsTest.md) | test explanation | unported |
| [WalkLinksFuncsTest.java](files/test/org/minima/system/network/p2p/WalkLinksFuncsTest.md) | test explanation | unported |

## test/org/minima/system/network/p2p/messages

| File | Teaching state | Native state |
|---|---|---|
| [GreetingTest.java](files/test/org/minima/system/network/p2p/messages/GreetingTest.md) | test explanation | implemented_partial |
| [InetSocketAddressIOTest.java](files/test/org/minima/system/network/p2p/messages/InetSocketAddressIOTest.md) | test explanation | implemented_partial |

## test/org/minima/system/network/p2p/testingutils

| File | Teaching state | Native state |
|---|---|---|
| [QuickClients.java](files/test/org/minima/system/network/p2p/testingutils/QuickClients.md) | test explanation | unported |
| [QuickInetLists.java](files/test/org/minima/system/network/p2p/testingutils/QuickInetLists.md) | test explanation | unported |
| [QuickState.java](files/test/org/minima/system/network/p2p/testingutils/QuickState.md) | test explanation | unported |

## test/org/minima/system/params

| File | Teaching state | Native state |
|---|---|---|
| [ParamConfigurerTest.java](files/test/org/minima/system/params/ParamConfigurerTest.md) | test explanation | implemented_partial |

## test/org/minima/tests/kissvm

| File | Teaching state | Native state |
|---|---|---|
| [ContractTests.java](files/test/org/minima/tests/kissvm/ContractTests.md) | test explanation | implemented_partial |

## test/org/minima/tests/kissvm/expressions

| File | Teaching state | Native state |
|---|---|---|
| [BooleanExpressionTests.java](files/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.md) | test explanation | unported |
| [ConstantExpressionTests.java](files/test/org/minima/tests/kissvm/expressions/ConstantExpressionTests.md) | test explanation | implemented_partial |
| [ExpressionParserTests.java](files/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.md) | test explanation | unported |
| [FunctionExpressionTests.java](files/test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.md) | test explanation | implemented_partial |
| [GlobalExpressionTests.java](files/test/org/minima/tests/kissvm/expressions/GlobalExpressionTests.md) | test explanation | implemented_partial |
| [OperatorExpressionTests.java](files/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.md) | test explanation | unported |
| [VariableExpressionTests.java](files/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.md) | test explanation | implemented_partial |

## test/org/minima/tests/kissvm/functions

| File | Teaching state | Native state |
|---|---|---|
| [DUMMYFUNCTION.java](files/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.md) | test explanation | implemented_partial |
| [MinimaFunctionTests.java](files/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.md) | test explanation | implemented_partial |

## test/org/minima/tests/kissvm/functions/base

| File | Teaching state | Native state |
|---|---|---|
| [CONCATTests.java](files/test/org/minima/tests/kissvm/functions/base/CONCATTests.md) | test explanation | unported |
| [GETTests.java](files/test/org/minima/tests/kissvm/functions/base/GETTests.md) | test explanation | unported |
| [LENTests.java](files/test/org/minima/tests/kissvm/functions/base/LENTests.md) | test explanation | implemented_partial |
| [REVTests.java](files/test/org/minima/tests/kissvm/functions/base/REVTests.md) | test explanation | implemented_partial |
| [SUBSETTests.java](files/test/org/minima/tests/kissvm/functions/base/SUBSETTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/functions/cast

| File | Teaching state | Native state |
|---|---|---|
| [BOOLTests.java](files/test/org/minima/tests/kissvm/functions/cast/BOOLTests.md) | test explanation | implemented_partial |
| [HEXTests.java](files/test/org/minima/tests/kissvm/functions/cast/HEXTests.md) | test explanation | implemented_partial |
| [NUMBERTests.java](files/test/org/minima/tests/kissvm/functions/cast/NUMBERTests.md) | test explanation | implemented_partial |
| [SCRIPTTests.java](files/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.md) | test explanation | implemented_partial |

## test/org/minima/tests/kissvm/functions/maths

| File | Teaching state | Native state |
|---|---|---|
| [ABSTests.java](files/test/org/minima/tests/kissvm/functions/maths/ABSTests.md) | test explanation | implemented_partial |
| [BITCOUNTTests.java](files/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.md) | test explanation | implemented_partial |
| [BITGETTests.java](files/test/org/minima/tests/kissvm/functions/maths/BITGETTests.md) | test explanation | implemented_partial |
| [BITSETTests.java](files/test/org/minima/tests/kissvm/functions/maths/BITSETTests.md) | test explanation | implemented_partial |
| [CEILTests.java](files/test/org/minima/tests/kissvm/functions/maths/CEILTests.md) | test explanation | implemented_partial |
| [DECTests.java](files/test/org/minima/tests/kissvm/functions/maths/DECTests.md) | test explanation | implemented_partial |
| [FLOORTests.java](files/test/org/minima/tests/kissvm/functions/maths/FLOORTests.md) | test explanation | implemented_partial |
| [INCTests.java](files/test/org/minima/tests/kissvm/functions/maths/INCTests.md) | test explanation | implemented_partial |
| [MAXTests.java](files/test/org/minima/tests/kissvm/functions/maths/MAXTests.md) | test explanation | implemented_partial |
| [MINTests.java](files/test/org/minima/tests/kissvm/functions/maths/MINTests.md) | test explanation | implemented_partial |
| [POWTests.java](files/test/org/minima/tests/kissvm/functions/maths/POWTests.md) | test explanation | implemented_partial |
| [SIGDIGTests.java](files/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.md) | test explanation | implemented_partial |

## test/org/minima/tests/kissvm/functions/sha

| File | Teaching state | Native state |
|---|---|---|
| [CHAINSHATests.java](files/test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.md) | test explanation | unported |
| [SHA2Tests.java](files/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.md) | test explanation | unported |
| [SHA3Tests.java](files/test/org/minima/tests/kissvm/functions/sha/SHA3Tests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/functions/sigs

| File | Teaching state | Native state |
|---|---|---|
| [CHECKSIGTests.java](files/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.md) | test explanation | unported |
| [MULTISIGTests.java](files/test/org/minima/tests/kissvm/functions/sigs/MULTISIGTests.md) | test explanation | unported |
| [SIGNEDBYTests.java](files/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/functions/state

| File | Teaching state | Native state |
|---|---|---|
| [PREVSTATETests.java](files/test/org/minima/tests/kissvm/functions/state/PREVSTATETests.md) | test explanation | unported |
| [SAMESTATETests.java](files/test/org/minima/tests/kissvm/functions/state/SAMESTATETests.md) | test explanation | unported |
| [STATETests.java](files/test/org/minima/tests/kissvm/functions/state/STATETests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/functions/txn/input

| File | Teaching state | Native state |
|---|---|---|
| [GETINADDRTests.java](files/test/org/minima/tests/kissvm/functions/txn/input/GETINADDRTests.md) | test explanation | unported |
| [GETINAMTTests.java](files/test/org/minima/tests/kissvm/functions/txn/input/GETINAMTTests.md) | test explanation | unported |
| [GETINIDTests.java](files/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.md) | test explanation | unported |
| [GETINTOKTests.java](files/test/org/minima/tests/kissvm/functions/txn/input/GETINTOKTests.md) | test explanation | unported |
| [VERIFYINTests.java](files/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/functions/txn/output

| File | Teaching state | Native state |
|---|---|---|
| [GETOUTADDRTests.java](files/test/org/minima/tests/kissvm/functions/txn/output/GETOUTADDRTests.md) | test explanation | unported |
| [GETOUTAMTTests.java](files/test/org/minima/tests/kissvm/functions/txn/output/GETOUTAMTTests.md) | test explanation | unported |
| [GETOUTTOKTests.java](files/test/org/minima/tests/kissvm/functions/txn/output/GETOUTTOKTests.md) | test explanation | unported |
| [VERIFYOUTTests.java](files/test/org/minima/tests/kissvm/functions/txn/output/VERIFYOUTTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/statements

| File | Teaching state | Native state |
|---|---|---|
| [StatementBlockTests.java](files/test/org/minima/tests/kissvm/statements/StatementBlockTests.md) | test explanation | implemented_partial |
| [StatementParserTests.java](files/test/org/minima/tests/kissvm/statements/StatementParserTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/statements/commands

| File | Teaching state | Native state |
|---|---|---|
| [ASSERTstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/ASSERTstatementTests.md) | test explanation | implemented_partial |
| [EXECstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.md) | test explanation | implemented_partial |
| [IFstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/IFstatementTests.md) | test explanation | unported |
| [LETstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/LETstatementTests.md) | test explanation | unported |
| [MASTstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.md) | test explanation | unported |
| [RETURNstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/RETURNstatementTests.md) | test explanation | unported |
| [WHILEstatementTests.java](files/test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/tokens

| File | Teaching state | Native state |
|---|---|---|
| [LexicalTokenizerTests.java](files/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.md) | test explanation | implemented_partial |
| [TokenTests.java](files/test/org/minima/tests/kissvm/tokens/TokenTests.md) | test explanation | unported |

## test/org/minima/tests/kissvm/values

| File | Teaching state | Native state |
|---|---|---|
| [BooleanValueTests.java](files/test/org/minima/tests/kissvm/values/BooleanValueTests.md) | test explanation | implemented_partial |
| [HexValueTests.java](files/test/org/minima/tests/kissvm/values/HexValueTests.md) | test explanation | implemented_partial |
| [NumberValueTests.java](files/test/org/minima/tests/kissvm/values/NumberValueTests.md) | test explanation | implemented_partial |
| [ScriptValueTests.java](files/test/org/minima/tests/kissvm/values/ScriptValueTests.md) | test explanation | implemented_partial |
| [ValueTests.java](files/test/org/minima/tests/kissvm/values/ValueTests.md) | test explanation | implemented_partial |

## test/org/minima/utils

| File | Teaching state | Native state |
|---|---|---|
| [TestUtils.java](files/test/org/minima/utils/TestUtils.md) | test explanation | unported |
