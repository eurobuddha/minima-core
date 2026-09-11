cmake_minimum_required(VERSION 3.20)
project(minima_file_port VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(OpenSSL 3.2 REQUIRED COMPONENTS Crypto)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(ICU 78 REQUIRED COMPONENTS uc i18n)
find_package(CURL 7.85 REQUIRED)

set_source_files_properties(src/support/fdlibm/e_log.cpp src/support/fdlibm/e_log10.cpp
    PROPERTIES COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-ffp-contract=off>")

# Only actual implementations belong here. Unported manifest rows are not stubs.
add_library(minima_file_port STATIC
    src/support/NativeThread.cpp
    src/org/minima/utils/MinimaUncaughtException.cpp
    src/org/minima/system/params/TestParams.cpp
    src/org/minima/system/params/ParamConfigurer.cpp
    src/org/minima/utils/MinimaRPCClient.cpp
    src/org/minima/utils/MinimaLogger.cpp
    src/org/minima/utils/RPCClient.cpp
    src/org/minima/system/network/webhooks/NotifyManager.cpp
    src/org/minima/system/brains/TxPoWMiner.cpp
    src/org/minima/system/brains/TxPoWProcessor.cpp
    src/org/minima/system/network/p2p/P2PManager.cpp
    src/org/minima/system/network/p2p/params/P2PTestParams.cpp
    src/org/minima/system/network/p2p/P2PPeersChecker.cpp
    src/org/minima/system/network/p2p/WalkLinksFuncs.cpp
    src/org/minima/system/network/p2p/SwapLinksFunctions.cpp
    src/org/minima/system/network/p2p/P2PFunctions.cpp
    src/org/minima/system/network/p2p/UtilFuncs.cpp
    src/support/P2PEnvironment.cpp
    src/org/minima/system/network/minima/NIOManager.cpp
    src/org/minima/system/network/minima/NIOMessage.cpp
    src/support/NIOConnections.cpp
    src/org/minima/system/network/minima/NIOServer.cpp
    src/support/NIOSocket.cpp
    src/org/minima/system/network/minima/NIOClient.cpp
    src/org/minima/system/network/minima/NIOClientInfo.cpp
    src/org/minima/system/network/p2p/P2PDB.cpp
    src/org/minima/system/network/p2p2/P2P2DB.cpp
    src/org/minima/system/network/p2p2/P2P2Functions.cpp
    src/org/minima/system/network/p2p2/P2P2Manager.cpp
    src/org/minima/system/network/rpc/Server.cpp
    src/org/minima/system/network/rpc/HTTPServer.cpp
    src/org/minima/system/network/rpc/CMDHandler.cpp
    src/org/minima/system/network/NetworkManager.cpp
    src/org/minima/system/network/p2p/P2PState.cpp
    src/org/minima/system/network/p2p/params/P2PParams.cpp
    src/org/minima/system/network/p2p/messages/P2PGreeting.cpp
    src/support/InetSocketAddress.cpp
    src/org/minima/system/network/p2p/messages/InetSocketAddressIO.cpp
    src/org/minima/system/network/p2p/messages/P2PDoSwap.cpp
    src/org/minima/system/network/p2p/messages/P2PWalkLinks.cpp
    src/support/fdlibm/e_log.cpp
    src/support/fdlibm/e_log10.cpp
    src/org/minima/objects/IBD.cpp
    src/org/minima/database/archive/TxBlockDB.cpp
    src/org/minima/database/archive/RawArchiveInput.cpp
    src/org/minima/database/wallet/SeedRow.cpp
    src/org/minima/database/wallet/KeyRow.cpp
    src/org/minima/database/wallet/ScriptRow.cpp
    src/org/minima/database/txpowdb/sql/TxPoWList.cpp
    src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.cpp
    src/org/minima/database/txpowdb/sql/TxPoWSqlDB.cpp
    src/org/minima/database/txpowdb/TxPoWDB.cpp
    src/org/minima/database/txpowtree/CoinDB.cpp
    src/org/minima/database/wallet/Wallet.cpp
    src/org/minima/database/archive/ArchiveManager.cpp
    src/org/minima/database/MinimaDB.cpp
    src/org/minima/database/userprefs/txndb/TxnRow.cpp
    src/org/minima/database/userprefs/txndb/TxnDB.cpp
    src/org/minima/utils/messages/Message.cpp
    src/org/minima/utils/messages/MessageListener.cpp
    src/org/minima/utils/messages/MessageStack.cpp
    src/org/minima/utils/messages/TimerMessage.cpp
    src/org/minima/utils/messages/MessageProcessor.cpp
    src/org/minima/utils/messages/TimerProcessor.cpp

    src/org/minima/utils/MiniUtil.cpp
    src/org/minima/utils/Maths.cpp
    src/org/minima/utils/Stack.cpp
    src/org/minima/utils/TimedRequest.cpp
    src/org/minima/database/userprefs/UserDB.cpp
    src/org/minima/utils/JsonDB.cpp
    src/org/minima/utils/mysql/MySQLConnect.cpp
    src/org/minima/utils/mysql/ArchiveServer.cpp
    src/org/minima/utils/SqlDB.cpp
    src/org/minima/utils/BIP39.cpp
    src/org/minima/utils/FastByteArrayStream.cpp
    src/org/minima/system/commands/Command.cpp
    src/org/minima/system/commands/base/debugflag.cpp
    src/org/minima/system/commands/txn/txnlock.cpp
    src/org/minima/system/commands/CommandException.cpp
    src/org/minima/system/commands/CommandRunner.cpp
    src/org/minima/system/commands/base/block.cpp
    src/org/minima/system/commands/base/printmmr.cpp
    src/org/minima/system/commands/base/printtree.cpp
    src/org/minima/system/commands/base/automine.cpp
    src/org/minima/system/commands/base/hash.cpp
    src/org/minima/system/commands/base/newaddress.cpp
    src/org/minima/system/commands/base/getaddress.cpp
    src/org/minima/system/commands/base/coinnotify.cpp
    src/org/minima/system/commands/base/slavenode.cpp

    src/org/minima/system/commands/base/maths.cpp
    src/org/minima/system/commands/base/mmrcreate.cpp
    src/org/minima/system/commands/base/mmrproof.cpp
    src/org/minima/system/commands/scripts/runscript.cpp
    src/org/minima/system/commands/scripts/scripts.cpp
    src/org/minima/system/commands/scripts/newscript.cpp
    src/org/minima/system/commands/scripts/removescript.cpp
    src/org/minima/system/commands/signatures/sign.cpp

    src/org/minima/system/commands/signatures/verify.cpp
    src/org/minima/system/commands/txn/txncreate.cpp
    src/org/minima/system/commands/txn/txndelete.cpp
    src/org/minima/system/commands/txn/txnlist.cpp
    src/org/minima/system/commands/txn/txnclear.cpp
    src/org/minima/system/commands/txn/txnstate.cpp
    src/org/minima/system/commands/txn/txnexport.cpp
    src/org/minima/system/commands/txn/txnimport.cpp
    src/org/minima/system/commands/txn/txnview.cpp
    src/org/minima/system/commands/txn/txnoutput.cpp
    src/org/minima/system/commands/txn/txnutils.cpp
    src/org/minima/system/commands/txn/txninput.cpp
    src/org/minima/system/commands/txn/txnbasics.cpp
    src/org/minima/system/commands/send/sendview.cpp
    src/org/minima/system/commands/send/sendpost.cpp
    src/org/minima/system/commands/send/sendsign.cpp
    src/org/minima/system/commands/send/wallet/rawfrom.cpp
    src/org/minima/system/commands/send/wallet/sendfrom.cpp
    src/org/minima/system/commands/send/wallet/consolidatefrom.cpp
    src/org/minima/system/commands/send/wallet/createtokenfrom.cpp
    src/org/minima/system/commands/send/wallet/rawtxnfrom.cpp
    src/org/minima/system/commands/send/wallet/createfrom.cpp
    src/org/minima/system/commands/send/wallet/constructfrom.cpp
    src/org/minima/system/commands/send/wallet/postfrom.cpp
    src/org/minima/system/commands/send/wallet/signfrom.cpp
    src/org/minima/system/commands/send/sendpoll.cpp
    src/org/minima/system/commands/send/sendnosign.cpp
    src/org/minima/system/commands/send/send.cpp
    src/org/minima/system/commands/txn/txnaddamount.cpp
    src/org/minima/system/commands/txn/txncoinlock.cpp
    src/org/minima/system/commands/search/keys.cpp
    src/org/minima/system/commands/backup/vault.cpp
    src/org/minima/system/commands/txn/txnsign.cpp
    src/org/minima/system/commands/txn/txnpost.cpp
    src/org/minima/system/commands/txn/txnmine.cpp
    src/org/minima/system/commands/txn/txnminepost.cpp
    src/org/minima/system/commands/txn/txncheck.cpp
    src/org/minima/system/commands/txn/txnmmr.cpp
    src/org/minima/system/commands/txn/txnscript.cpp
    src/org/minima/system/commands/search/tokens.cpp
    src/org/minima/system/commands/network/connect.cpp
    src/org/minima/system/commands/network/webhooks.cpp
    src/org/minima/system/commands/base/coincheck.cpp
    src/org/minima/system/commands/base/coinexport.cpp
    src/org/minima/system/commands/base/consolidate.cpp
    src/org/minima/system/commands/base/balance.cpp
    src/org/minima/system/commands/search/coins.cpp
    src/org/minima/system/commands/base/seedrandom.cpp
    src/org/minima/system/commands/base/mempool.cpp
    src/org/minima/system/commands/base/cointrack.cpp
    src/org/minima/system/commands/base/magic.cpp
    src/org/minima/system/commands/search/txpow.cpp
    src/org/minima/system/commands/search/history.cpp
    src/org/minima/system/commands/base/convert.cpp
    src/org/minima/system/commands/base/random.cpp
    src/org/minima/system/commands/base/checkaddress.cpp
    src/org/minima/system/commands/base/missingcmd.cpp
    src/org/minima/system/network/rpc/Authorizer.cpp
    src/org/minima/system/network/minima/NIOTraffic.cpp
    src/org/minima/database/txpowdb/ram/RamDB.cpp
    src/org/minima/database/txpowdb/ram/RamData.cpp
    src/org/minima/system/brains/TimedChecker.cpp
    src/org/minima/system/brains/TxPoWSearcher.cpp
    src/org/minima/system/brains/TxPoWChecker.cpp
    src/org/minima/system/brains/TxPoWGenerator.cpp
    src/org/minima/system/network/minima/RelayPolicy.cpp
    src/org/minima/utils/MiniFile.cpp
    src/org/minima/utils/ZipExtractor.cpp
    src/org/minima/utils/sphincs/HORST/PrivateKey.cpp
    src/org/minima/utils/sphincs/HORST/PublicKey.cpp
    src/org/minima/utils/sphincs/HORST/HORSTSignature.cpp
    src/org/minima/utils/sphincs/HORST/HORST.cpp
    src/org/minima/utils/sphincs/FORS/FORSSignature.cpp
    src/org/minima/utils/sphincs/FORS/FORS.cpp
    src/org/minima/utils/sphincs/TransactionTest.cpp
    src/org/minima/utils/sphincs/SPHINCSUtils.cpp
    src/org/minima/utils/sphincs/SPHINCS.cpp
    src/org/minima/utils/sphincs/SPHINCSSignature.cpp
    src/org/minima/utils/MiniFormat.cpp
    src/org/minima/utils/encrypt/GenerateKey.cpp
    src/org/minima/utils/encrypt/javajs/AesUtil.cpp
    src/org/minima/utils/encrypt/EncryptDecrypt.cpp
    src/org/minima/utils/encrypt/SignVerify.cpp
    src/org/minima/utils/encrypt/CryptoPackage.cpp
    src/org/minima/utils/encrypt/PasswordCrypto.cpp
    src/org/minima/database/txpowtree/TxPowTree.cpp
    src/org/minima/database/txpowtree/TxPoWTreeNode.cpp
    src/org/minima/database/txpowtree/TxPoWTreeNodeAction.cpp
    src/org/minima/database/cascade/Cascade.cpp
    src/org/minima/database/cascade/CascadeNode.cpp
    src/org/minima/kissvm/functions/txn/input/GETINADDR.cpp
    src/org/minima/kissvm/functions/txn/input/GETINAMT.cpp
    src/org/minima/kissvm/functions/txn/input/GETINID.cpp
    src/org/minima/kissvm/functions/txn/input/GETINTOK.cpp
    src/org/minima/kissvm/functions/txn/input/SUMINPUTS.cpp
    src/org/minima/kissvm/functions/txn/input/VERIFYIN.cpp
    src/org/minima/kissvm/functions/txn/output/GETOUTADDR.cpp
    src/org/minima/kissvm/functions/txn/output/GETOUTAMT.cpp
    src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.cpp
    src/org/minima/kissvm/functions/txn/output/GETOUTTOK.cpp
    src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.cpp
    src/org/minima/kissvm/functions/txn/output/VERIFYOUT.cpp
    src/org/minima/kissvm/functions/cast/ASCII.cpp
    src/org/minima/kissvm/functions/cast/BOOL.cpp
    src/org/minima/kissvm/functions/cast/HEX.cpp
    src/org/minima/kissvm/functions/cast/NUMBER.cpp
    src/org/minima/kissvm/functions/cast/STRING.cpp
    src/org/minima/kissvm/functions/cast/UTF8.cpp
    src/org/minima/kissvm/functions/general/ADDRESS.cpp
    src/org/minima/kissvm/functions/general/EXISTS.cpp
    src/org/minima/kissvm/functions/general/FUNCTION.cpp
    src/org/minima/kissvm/functions/general/GET.cpp
    src/org/minima/kissvm/functions/hex/BITCOUNT.cpp
    src/org/minima/kissvm/functions/hex/BITGET.cpp
    src/org/minima/kissvm/functions/hex/BITSET.cpp
    src/org/minima/kissvm/functions/hex/CONCAT.cpp
    src/org/minima/kissvm/functions/hex/LEN.cpp
    src/org/minima/kissvm/functions/hex/OVERWRITE.cpp
    src/org/minima/kissvm/functions/hex/REV.cpp
    src/org/minima/kissvm/functions/hex/SETLEN.cpp
    src/org/minima/kissvm/functions/hex/SUBSET.cpp
    src/org/minima/kissvm/functions/sha/PROOF.cpp
    src/org/minima/kissvm/functions/sha/SHA2.cpp
    src/org/minima/kissvm/functions/sha/SHA3.cpp
    src/org/minima/kissvm/functions/sigs/CHECKSIG.cpp
    src/org/minima/kissvm/functions/sigs/MULTISIG.cpp
    src/org/minima/kissvm/functions/sigs/SIGNEDBY.cpp
    src/org/minima/kissvm/functions/state/PREVSTATE.cpp
    src/org/minima/kissvm/functions/state/SAMESTATE.cpp
    src/org/minima/kissvm/functions/state/STATE.cpp
    src/org/minima/kissvm/functions/string/REPLACE.cpp
    src/org/minima/kissvm/functions/string/REPLACEFIRST.cpp
    src/org/minima/kissvm/functions/string/SUBSTR.cpp
    src/org/minima/kissvm/Contract.cpp
    src/org/minima/kissvm/expressions/BooleanExpression.cpp
    src/org/minima/kissvm/expressions/ConstantExpression.cpp
    src/org/minima/kissvm/expressions/Expression.cpp
    src/org/minima/kissvm/expressions/ExpressionParser.cpp
    src/org/minima/kissvm/expressions/FunctionExpression.cpp
    src/org/minima/kissvm/expressions/GlobalExpression.cpp
    src/org/minima/kissvm/expressions/OperatorExpression.cpp
    src/org/minima/kissvm/expressions/VariableExpression.cpp
    src/org/minima/kissvm/functions/MinimaFunction.cpp
    src/org/minima/kissvm/functions/number/ABS.cpp
    src/org/minima/kissvm/functions/number/CEIL.cpp
    src/org/minima/kissvm/functions/number/DEC.cpp
    src/org/minima/kissvm/functions/number/FLOOR.cpp
    src/org/minima/kissvm/functions/number/INC.cpp
    src/org/minima/kissvm/functions/number/MAX.cpp
    src/org/minima/kissvm/functions/number/MIN.cpp
    src/org/minima/kissvm/functions/number/POW.cpp
    src/org/minima/kissvm/functions/number/SIGDIG.cpp
    src/org/minima/kissvm/functions/number/SQRT.cpp
    src/org/minima/kissvm/statements/Statement.cpp
    src/org/minima/kissvm/statements/StatementBlock.cpp
    src/org/minima/kissvm/statements/StatementParser.cpp
    src/org/minima/kissvm/statements/commands/ASSERTstatement.cpp
    src/org/minima/kissvm/statements/commands/EXECstatement.cpp
    src/org/minima/kissvm/statements/commands/IFstatement.cpp
    src/org/minima/kissvm/statements/commands/LETstatement.cpp
    src/org/minima/kissvm/statements/commands/MASTstatement.cpp
    src/org/minima/kissvm/statements/commands/RETURNstatement.cpp
    src/org/minima/kissvm/statements/commands/WHILEstatement.cpp
    src/org/minima/kissvm/exceptions/ExecutionException.cpp
    src/org/minima/kissvm/exceptions/MinimaException.cpp
    src/org/minima/kissvm/exceptions/MinimaParseException.cpp
    src/org/minima/kissvm/tokens/LexicalTokenizer.cpp
    src/org/minima/kissvm/tokens/ScriptToken.cpp
    src/org/minima/kissvm/tokens/ScriptTokenizer.cpp
    src/org/minima/kissvm/values/BooleanValue.cpp
    src/org/minima/kissvm/values/HexValue.cpp
    src/org/minima/kissvm/values/NumberValue.cpp
    src/org/minima/kissvm/values/StringValue.cpp
    src/org/minima/kissvm/values/Value.cpp
    src/org/minima/objects/base/MiniByte.cpp
    src/org/minima/objects/base/MiniData.cpp
    src/org/minima/objects/base/MiniNumber.cpp
    src/org/minima/objects/base/MiniString.cpp
    src/org/minima/utils/json/JSONAware.cpp
    src/org/minima/utils/json/JSONStreamAware.cpp
    src/org/minima/utils/json/JSONWriter.cpp
    src/org/minima/utils/json/parser/ParseException.cpp
    src/org/minima/utils/json/parser/Yytoken.cpp
    src/org/minima/utils/json/parser/ContainerFactory.cpp
    src/org/minima/utils/json/parser/ContentHandler.cpp
    src/org/minima/utils/json/parser/Yylex.cpp
    src/org/minima/utils/json/parser/JSONParser.cpp
    src/org/minima/utils/json/JSONValue.cpp
    src/org/minima/utils/json/JSONObject.cpp
    src/org/minima/utils/json/JSONArray.cpp
    src/org/minima/utils/BaseConverter.cpp
    src/org/minima/utils/Crypto.cpp
    src/org/minima/objects/mmr/MMRData.cpp
    src/org/minima/objects/mmr/MMRProof.cpp
    src/org/minima/objects/Address.cpp
    src/org/minima/objects/StateVariable.cpp
    src/org/minima/objects/Token.cpp
    src/org/minima/objects/mmr/MMREntryNumber.cpp
    src/org/minima/objects/mmr/MMREntry.cpp
    src/org/minima/objects/Coin.cpp
    src/org/minima/objects/ScriptProof.cpp
    src/org/minima/objects/CoinProof.cpp
    src/org/minima/objects/Transaction.cpp
    src/org/minima/objects/keys/Winternitz.cpp
    src/org/minima/objects/keys/SignatureProof.cpp
    src/org/minima/objects/keys/Signature.cpp
    src/org/minima/objects/Witness.cpp
    src/org/minima/system/params/GeneralParams.cpp
    src/org/minima/objects/mmr/MMR.cpp
    src/org/minima/objects/keys/TreeKeyNode.cpp
    src/org/minima/objects/keys/TreeKey.cpp
    src/org/minima/objects/Magic.cpp
    src/org/minima/objects/TxBody.cpp
    src/org/minima/system/params/GlobalParams.cpp
    src/org/minima/objects/TxHeader.cpp
    src/org/minima/system/genesis/GenesisCoin.cpp
    src/org/minima/system/genesis/GenesisMMR.cpp
    src/org/minima/system/genesis/GenesisTxPoW.cpp
    src/org/minima/objects/Greeting.cpp
    src/org/minima/objects/Pulse.cpp
    src/org/minima/objects/TxBlock.cpp
    src/org/minima/objects/mmr/MegaMMR.cpp
    src/org/minima/system/commands/backup/mmrsync/MegaMMRBackup.cpp
    src/org/minima/system/commands/backup/mmrsync/MegaMMRIBD.cpp
    src/org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.cpp
    src/org/minima/system/commands/sendpoll/SendPollMessage.cpp
    src/org/minima/system/commands/sendpoll/SendPollManager.cpp
    src/org/minima/objects/TxPoW.cpp)
target_include_directories(minima_file_port PUBLIC src)
target_compile_options(minima_file_port PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(minima_file_port PUBLIC OpenSSL::Crypto ZLIB::ZLIB Threads::Threads ICU::uc ICU::i18n CURL::libcurl)

add_executable(minima_primitives support/primitives.cpp)
target_link_libraries(minima_primitives PRIVATE minima_file_port)
enable_testing()
add_executable(MiniByteTests test/org/minima/objects/base/MiniByteTests.cpp)
target_link_libraries(MiniByteTests PRIVATE minima_file_port)
add_test(NAME MiniByteTests COMMAND MiniByteTests)
add_executable(malformed_streams tests/malformed_streams.cpp)
target_link_libraries(malformed_streams PRIVATE minima_file_port)
add_test(NAME malformed_streams COMMAND malformed_streams)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME source_mapping COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/source_map.py --check)
add_test(NAME java_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(check_full_port
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/source_map.py --require-complete
    COMMENT "Require every mapped Java file to have a verified C++ implementation")

add_executable(minima_objects support/objects.cpp)
target_link_libraries(minima_objects PRIVATE minima_file_port)
add_test(NAME object_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/object_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(TransactionTests test/org/minima/objects/TransactionTests.cpp)
target_link_libraries(TransactionTests PRIVATE minima_file_port)
add_test(NAME TransactionTests COMMAND TransactionTests)

add_executable(TxHeaderTests test/org/minima/objects/TxHeaderTests.cpp)
target_link_libraries(TxHeaderTests PRIVATE minima_file_port)
add_test(NAME TxHeaderTests COMMAND TxHeaderTests)

add_executable(minima_json support/json.cpp)
target_link_libraries(minima_json PRIVATE minima_file_port)
add_test(NAME json_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/json_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_kiss_values support/kiss.cpp)
target_link_libraries(minima_kiss_values PRIVATE minima_file_port)
add_test(NAME kiss_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/kiss_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

foreach(test IN ITEMS ValueTests BooleanValueTests ScriptValueTests HexValueTests NumberValueTests)
    add_executable(${test} test/org/minima/tests/kissvm/values/${test}.cpp)
    target_link_libraries(${test} PRIVATE minima_file_port)
    add_test(NAME ${test} COMMAND ${test})
endforeach()
add_executable(LexicalTokenizerTests test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.cpp)
target_link_libraries(LexicalTokenizerTests PRIVATE minima_file_port)
add_test(NAME LexicalTokenizerTests COMMAND LexicalTokenizerTests)

add_executable(minima_kissvm support/kissvm.cpp)
target_link_libraries(minima_kissvm PRIVATE minima_file_port)
add_test(NAME kissvm_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/kissvm_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(ConstantExpressionTests test/org/minima/tests/kissvm/expressions/ConstantExpressionTests.cpp)
target_link_libraries(ConstantExpressionTests PRIVATE minima_file_port)
add_test(NAME ConstantExpressionTests COMMAND ConstantExpressionTests)

add_executable(VariableExpressionTests test/org/minima/tests/kissvm/expressions/VariableExpressionTests.cpp)
target_link_libraries(VariableExpressionTests PRIVATE minima_file_port)
add_test(NAME VariableExpressionTests COMMAND VariableExpressionTests)

add_executable(GlobalExpressionTests test/org/minima/tests/kissvm/expressions/GlobalExpressionTests.cpp)
target_link_libraries(GlobalExpressionTests PRIVATE minima_file_port)
add_test(NAME GlobalExpressionTests COMMAND GlobalExpressionTests)

add_executable(FunctionExpressionTests test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.cpp)
target_link_libraries(FunctionExpressionTests PRIVATE minima_file_port)
add_test(NAME FunctionExpressionTests COMMAND FunctionExpressionTests)

add_executable(ContractTests test/org/minima/tests/kissvm/ContractTests.cpp)
target_link_libraries(ContractTests PRIVATE minima_file_port)
add_test(NAME ContractTests COMMAND ContractTests)

add_executable(StatementBlockTests test/org/minima/tests/kissvm/statements/StatementBlockTests.cpp)
target_link_libraries(StatementBlockTests PRIVATE minima_file_port)
add_test(NAME StatementBlockTests COMMAND StatementBlockTests)

add_executable(minima_peer_messages support/peer_messages.cpp)
target_link_libraries(minima_peer_messages PRIVATE minima_file_port)
add_test(NAME peer_messages_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/peer_messages_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(txn_lock_workers support/txn_lock_workers.cpp)
target_link_libraries(txn_lock_workers PRIVATE minima_file_port)
add_test(NAME txn_lock_workers COMMAND txn_lock_workers)

add_executable(minima_small_utilities support/small_utilities.cpp)
target_link_libraries(minima_small_utilities PRIVATE minima_file_port)
add_test(NAME small_utilities_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/small_utilities_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

foreach(test ASSERTstatementTests EXECstatementTests)
    add_executable(${test} test/org/minima/tests/kissvm/statements/commands/${test}.cpp)
    target_link_libraries(${test} PRIVATE minima_file_port)
    add_test(NAME ${test} COMMAND ${test})
endforeach()

add_executable(minima_cascade support/cascade.cpp)
target_link_libraries(minima_cascade PRIVATE minima_file_port)
add_test(NAME cascade_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/cascade_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(TxPowTreeTest test/org/minima/database/txpowtree/TxPowTreeTest.cpp)
target_link_libraries(TxPowTreeTest PRIVATE minima_file_port)
add_test(NAME TxPowTreeTest COMMAND TxPowTreeTest)
add_executable(minima_tree support/tree.cpp)
target_link_libraries(minima_tree PRIVATE minima_file_port)
add_test(NAME tree_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/tree_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_utility support/utility.cpp)
target_link_libraries(minima_utility PRIVATE minima_file_port)
add_test(NAME utility_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/utility_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_aes_util support/aes_util.cpp)
target_link_libraries(minima_aes_util PRIVATE minima_file_port)
add_test(NAME aes_util_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/aes_util_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(aes_util_differential PROPERTIES TIMEOUT 180)

add_executable(minima_transaction_test support/transaction_test.cpp)
target_link_libraries(minima_transaction_test PRIVATE minima_file_port)
add_test(NAME transaction_test_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/transaction_test_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(transaction_test_differential PROPERTIES TIMEOUT 7500)

add_executable(minima_sphincs_utils support/sphincs_utils.cpp)
target_link_libraries(minima_sphincs_utils PRIVATE minima_file_port)
add_test(NAME sphincs_utils_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sphincs_utils_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sphincs_utils_differential PROPERTIES TIMEOUT 3600)

add_executable(minima_sphincs support/sphincs.cpp)
target_link_libraries(minima_sphincs PRIVATE minima_file_port)
add_test(NAME sphincs_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sphincs_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sphincs_differential PROPERTIES TIMEOUT 28800)
add_test(NAME sphincs_log_audit COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sphincs_log_audit.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sphincs_log_audit PROPERTIES TIMEOUT 120)

add_executable(minima_sphincs_signature support/sphincs_signature.cpp)
target_link_libraries(minima_sphincs_signature PRIVATE minima_file_port)
add_test(NAME sphincs_signature_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sphincs_signature_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sphincs_signature_differential PROPERTIES TIMEOUT 300)

add_executable(minima_fors support/fors.cpp)
target_link_libraries(minima_fors PRIVATE minima_file_port)
add_test(NAME fors_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/fors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(fors_differential PROPERTIES TIMEOUT 3600)

add_executable(minima_horst support/horst.cpp)
target_link_libraries(minima_horst PRIVATE minima_file_port)
add_test(NAME horst_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/horst_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(horst_differential PROPERTIES TIMEOUT 900)

add_executable(minima_zip_extractor support/zip_extractor.cpp)
target_link_libraries(minima_zip_extractor PRIVATE minima_file_port)
add_test(NAME zip_extractor_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/zip_extractor_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(zip_extractor_differential PROPERTIES TIMEOUT 240)

add_executable(minima_consensus support/consensus.cpp)
target_link_libraries(minima_consensus PRIVATE minima_file_port)
add_test(NAME consensus_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/consensus_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(RamDBTest test/org/minima/database/txpowdb/ram/RamDBTest.cpp)
target_link_libraries(RamDBTest PRIVATE minima_file_port)
add_test(NAME RamDBTest COMMAND RamDBTest)
add_executable(minima_ram support/ram.cpp)
target_link_libraries(minima_ram PRIVATE minima_file_port)
add_test(NAME ram_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/ram_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(timed_checker_checks support/timed.cpp)
target_link_libraries(timed_checker_checks PRIVATE minima_file_port)
add_test(NAME timed_checker_checks COMMAND timed_checker_checks)
add_test(NAME timed_checker_deadline COMMAND timed_checker_checks --deadline)
set_tests_properties(timed_checker_deadline PROPERTIES TIMEOUT 150)
add_executable(minima_prefs support/prefs.cpp)
target_link_libraries(minima_prefs PRIVATE minima_file_port)
add_test(NAME prefs_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/prefs_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_hashmap support/hashmap.cpp)
target_link_libraries(minima_hashmap PRIVATE minima_file_port)
add_test(NAME hashmap_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/ram_differential.py --hashmap --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(minima_messages support/messages.cpp)
target_link_libraries(minima_messages PRIVATE minima_file_port)
add_test(NAME message_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/message_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(message_workers support/message_workers.cpp)
target_link_libraries(message_workers PRIVATE minima_file_port)
add_test(NAME message_workers COMMAND message_workers)
set_tests_properties(message_workers PROPERTIES TIMEOUT 20)
add_executable(minima_txndb support/txndb.cpp)
target_link_libraries(minima_txndb PRIVATE minima_file_port)
add_test(NAME txndb_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/txndb_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(minima_archive_cache support/archive_cache.cpp)
target_link_libraries(minima_archive_cache PRIVATE minima_file_port)
add_test(NAME archive_cache_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_cache_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(minima_ibd support/ibd.cpp)
target_link_libraries(minima_ibd PRIVATE minima_file_port)
add_test(NAME ibd_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/ibd_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_mmrsync support/mmrsync.cpp)
target_link_libraries(minima_mmrsync PRIVATE minima_file_port)
add_test(NAME mmrsync_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/mmrsync_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_sendpoll support/sendpoll.cpp)
target_link_libraries(minima_sendpoll PRIVATE minima_file_port)
add_test(NAME sendpoll_workers COMMAND minima_sendpoll)
set_tests_properties(sendpoll_workers PROPERTIES TIMEOUT 30)

add_executable(minima_raw_archive support/raw_archive.cpp)
target_link_libraries(minima_raw_archive PRIVATE minima_file_port)
add_test(NAME raw_archive_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/raw_archive_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_wallet_rows support/wallet_rows.cpp)
target_link_libraries(minima_wallet_rows PRIVATE minima_file_port)
add_test(NAME wallet_rows_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_rows_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_bip39 support/bip39.cpp)
target_link_libraries(minima_bip39 PRIVATE minima_file_port)
add_test(NAME bip39_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/bip39_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_fast_bytes support/fast_bytes.cpp)
target_link_libraries(minima_fast_bytes PRIVATE minima_file_port)
add_test(NAME fast_bytes_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/fast_bytes_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_network_utilities support/network_utilities.cpp)
target_link_libraries(minima_network_utilities PRIVATE minima_file_port)
add_test(NAME network_utilities_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/network_utilities_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_command support/command.cpp)
target_link_libraries(minima_command PRIVATE minima_file_port)
add_test(NAME command_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_command_runner support/command_runner.cpp)
target_link_libraries(minima_command_runner PRIVATE minima_file_port)
add_test(NAME command_runner_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/command_runner_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(minima_commands support/commands_cli.cpp)
target_link_libraries(minima_commands PRIVATE minima_file_port)

add_executable(minima_transaction_commands support/transaction_commands.cpp)
target_link_libraries(minima_transaction_commands PRIVATE minima_file_port)
add_test(NAME transaction_commands_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/transaction_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_searcher support/searcher.cpp)
target_link_libraries(minima_searcher PRIVATE minima_file_port)
add_test(NAME searcher_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/searcher_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

foreach(test InetSocketAddressIOTest GreetingTest)
    add_executable(${test} test/org/minima/system/network/p2p/messages/${test}.cpp)
    target_link_libraries(${test} PRIVATE minima_file_port)
    add_test(NAME ${test} COMMAND ${test})
endforeach()

add_executable(minima_peer_state support/peer_state.cpp)
target_link_libraries(minima_peer_state PRIVATE minima_file_port)
add_test(NAME peer_state_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/peer_state_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(P2PDBTest test/org/minima/system/network/p2p/P2PDBTest.cpp)
target_link_libraries(P2PDBTest PRIVATE minima_file_port)
add_test(NAME P2PDBTest COMMAND P2PDBTest)

add_executable(minima_nio_client support/nio_client.cpp)
target_link_libraries(minima_nio_client PRIVATE minima_file_port)
add_test(NAME nio_client_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_client_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
add_executable(nio_loopback support/nio_loopback.cpp)
target_link_libraries(nio_loopback PRIVATE minima_file_port)
add_test(NAME nio_loopback COMMAND nio_loopback)
set_tests_properties(nio_loopback PROPERTIES TIMEOUT 25)

add_executable(minima_nio_server support/nio_server.cpp)
target_link_libraries(minima_nio_server PRIVATE minima_file_port)
add_test(NAME nio_server_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_server_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(nio_server_differential PROPERTIES TIMEOUT 120)

add_executable(minima_p2p2_db support/p2p2_db.cpp)
target_link_libraries(minima_p2p2_db PRIVATE minima_file_port)
add_test(NAME p2p2_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/p2p2_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_network_startup support/network_startup.cpp)
target_link_libraries(minima_network_startup PRIVATE minima_file_port)
add_test(NAME network_startup_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/network_startup_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})

add_executable(minima_nio_manager_transport support/nio_manager_transport.cpp)
target_link_libraries(minima_nio_manager_transport PRIVATE minima_file_port)
add_test(NAME nio_manager_transport_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_manager_transport_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(nio_manager_transport_differential PROPERTIES TIMEOUT 120)

add_executable(fixed_task_executor support/fixed_task_executor.cpp)
target_link_libraries(fixed_task_executor PRIVATE minima_file_port)
add_test(NAME fixed_task_executor COMMAND fixed_task_executor)
set_tests_properties(fixed_task_executor PROPERTIES TIMEOUT 20)

add_executable(nio_manager_loopback support/nio_manager_loopback.cpp)
target_link_libraries(nio_manager_loopback PRIVATE minima_file_port)
add_test(NAME nio_manager_loopback COMMAND nio_manager_loopback)
set_tests_properties(nio_manager_loopback PROPERTIES TIMEOUT 40)

add_executable(nio_connections support/nio_connections.cpp)
target_link_libraries(nio_connections PRIVATE minima_file_port)
add_test(NAME nio_connections COMMAND nio_connections)
set_tests_properties(nio_connections PROPERTIES TIMEOUT 10)

add_executable(minima_nio_manager_state support/nio_manager_state.cpp)
target_link_libraries(minima_nio_manager_state PRIVATE minima_file_port)
add_test(NAME nio_manager_state_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_manager_state_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(nio_manager_state_differential PROPERTIES TIMEOUT 120)

add_executable(minima_nio_message support/nio_message.cpp)
target_link_libraries(minima_nio_message PRIVATE minima_file_port)
add_test(NAME nio_message_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_message_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(nio_message_differential PROPERTIES TIMEOUT 120)

add_executable(minima_p2p_functions support/p2p_functions.cpp)
target_link_libraries(minima_p2p_functions PRIVATE minima_file_port)
add_test(NAME p2p_functions_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/p2p_functions_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(p2p_functions_differential PROPERTIES TIMEOUT 120)
add_test(NAME swap_links_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/swap_links_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(swap_links_differential PROPERTIES TIMEOUT 120)
add_test(NAME walk_links_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/walk_links_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(walk_links_differential PROPERTIES TIMEOUT 120)
add_executable(peers_checker_native support/peers_checker_native.cpp)
target_link_libraries(peers_checker_native PRIVATE minima_file_port)
add_test(NAME peers_checker_native COMMAND peers_checker_native)
add_executable(minima_peers_checker support/peers_checker.cpp)
target_link_libraries(minima_peers_checker PRIVATE minima_file_port)
add_test(NAME peers_checker_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/peers_checker_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(peers_checker_differential PROPERTIES TIMEOUT 120)
add_executable(p2p_manager_native support/p2p_manager_native.cpp)
target_link_libraries(p2p_manager_native PRIVATE minima_file_port)
add_test(NAME p2p_manager_native COMMAND p2p_manager_native)
set_tests_properties(p2p_manager_native PROPERTIES TIMEOUT 30)
add_executable(peer_state_concurrent support/peer_state_concurrent.cpp)
target_link_libraries(peer_state_concurrent PRIVATE minima_file_port)
add_test(NAME peer_state_concurrent COMMAND peer_state_concurrent)
set_tests_properties(peer_state_concurrent PROPERTIES TIMEOUT 60)
add_executable(minima_p2p_manager support/p2p_manager.cpp)
target_link_libraries(minima_p2p_manager PRIVATE minima_file_port)
add_test(NAME p2p_manager_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/p2p_manager_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(p2p_manager_differential PROPERTIES TIMEOUT 120)
add_test(NAME p2p_discovery_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/p2p_discovery_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(p2p_discovery_differential PROPERTIES TIMEOUT 120)
add_executable(minima_p2p2_runtime support/p2p2_runtime.cpp)
target_link_libraries(minima_p2p2_runtime PRIVATE minima_file_port)
add_test(NAME p2p2_runtime_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/p2p2_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(p2p2_runtime_differential PROPERTIES TIMEOUT 120)
add_executable(p2p2_worker support/p2p2_worker.cpp)
target_link_libraries(p2p2_worker PRIVATE minima_file_port)
add_test(NAME p2p2_worker COMMAND p2p2_worker)
set_tests_properties(p2p2_worker PROPERTIES TIMEOUT 30)
add_executable(minima_http_server support/http_server.cpp)
target_link_libraries(minima_http_server PRIVATE minima_file_port)
add_test(NAME http_server_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/http_server_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(http_server_differential PROPERTIES TIMEOUT 90)
add_test(NAME cmd_handler_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/cmd_handler_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(cmd_handler_differential PROPERTIES TIMEOUT 120)
add_executable(network_manager_native support/network_manager_native.cpp)
target_link_libraries(network_manager_native PRIVATE minima_file_port)
add_test(NAME network_manager_native COMMAND network_manager_native)
set_tests_properties(network_manager_native PROPERTIES TIMEOUT 60)
add_executable(minima_network_manager support/network_manager.cpp)
target_link_libraries(minima_network_manager PRIVATE minima_file_port)
add_test(NAME network_manager_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/network_manager_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(network_manager_differential PROPERTIES TIMEOUT 90)
add_executable(minima_sql_db support/sql_db.cpp)
target_link_libraries(minima_sql_db PRIVATE minima_file_port)
add_test(NAME sql_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sql_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sql_db_differential PROPERTIES TIMEOUT 120)
add_executable(sql_db_concurrent support/sql_db_concurrent.cpp)
target_link_libraries(sql_db_concurrent PRIVATE minima_file_port)
add_test(NAME sql_db_concurrent COMMAND sql_db_concurrent ${CMAKE_CURRENT_BINARY_DIR}/sql-db-concurrent/db)
set_tests_properties(sql_db_concurrent PROPERTIES TIMEOUT 20)
add_executable(minima_onchain_db support/onchain_db.cpp)
target_link_libraries(minima_onchain_db PRIVATE minima_file_port)
add_test(NAME onchain_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/onchain_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(onchain_db_differential PROPERTIES TIMEOUT 120)
add_executable(minima_txpow_sql_db support/txpow_sql_db.cpp)
target_link_libraries(minima_txpow_sql_db PRIVATE minima_file_port)
add_test(NAME txpow_sql_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/txpow_sql_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(txpow_sql_db_differential PROPERTIES TIMEOUT 120)
add_executable(minima_txpow_db support/txpow_db.cpp)
target_link_libraries(minima_txpow_db PRIVATE minima_file_port)
add_test(NAME txpow_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/txpow_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(txpow_db_differential PROPERTIES TIMEOUT 120)

add_executable(minima_coin_db support/coin_db.cpp)
target_link_libraries(minima_coin_db PRIVATE minima_file_port)
add_test(NAME coin_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/coin_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(coin_db_differential PROPERTIES TIMEOUT 120)

add_executable(minima_wallet support/wallet.cpp)
target_link_libraries(minima_wallet PRIVATE minima_file_port)
add_test(NAME wallet_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(wallet_differential PROPERTIES TIMEOUT 240)

add_executable(minima_archive_manager support/archive_manager.cpp)
target_link_libraries(minima_archive_manager PRIVATE minima_file_port)
add_test(NAME archive_manager_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_manager_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(archive_manager_differential PROPERTIES TIMEOUT 240)

add_executable(minima_db support/minima_db.cpp)
target_link_libraries(minima_db PRIVATE minima_file_port)
add_test(NAME minima_db_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(minima_db_differential PROPERTIES TIMEOUT 240)
add_executable(minima_db_lock support/minima_db_lock.cpp)
target_link_libraries(minima_db_lock PRIVATE minima_file_port)
add_test(NAME minima_db_lock COMMAND minima_db_lock)
set_tests_properties(minima_db_lock PROPERTIES TIMEOUT 30)
add_executable(minima_db_ownership support/minima_db_ownership.cpp)
target_link_libraries(minima_db_ownership PRIVATE minima_file_port)
add_test(NAME minima_db_ownership COMMAND minima_db_ownership)
set_tests_properties(minima_db_ownership PROPERTIES TIMEOUT 30)

add_executable(minima_txpow_miner support/txpow_miner.cpp)
target_link_libraries(minima_txpow_miner PRIVATE minima_file_port)
add_test(NAME txpow_miner_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/txpow_miner_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(txpow_miner_differential PROPERTIES TIMEOUT 240)

add_executable(txpow_miner_worker support/txpow_miner_worker.cpp)
target_link_libraries(txpow_miner_worker PRIVATE minima_file_port)
add_test(NAME txpow_miner_worker COMMAND txpow_miner_worker)
set_tests_properties(txpow_miner_worker PROPERTIES TIMEOUT 30)

add_executable(txblock_ownership support/txblock_ownership.cpp)
target_link_libraries(txblock_ownership PRIVATE minima_file_port)
add_test(NAME txblock_ownership_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/txblock_ownership_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(txblock_ownership_differential PROPERTIES TIMEOUT 240)
add_test(NAME timed_checker_shared_deadline COMMAND timed_checker_checks --shared-deadline)
set_tests_properties(timed_checker_shared_deadline PROPERTIES TIMEOUT 150)
add_executable(minima_txpow_processor support/txpow_processor.cpp)
target_link_libraries(minima_txpow_processor PRIVATE minima_file_port)
add_test(NAME txpow_processor_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/txpow_processor_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(txpow_processor_differential PROPERTIES TIMEOUT 600)
add_executable(txpow_processor_worker support/txpow_processor_worker.cpp)
target_link_libraries(txpow_processor_worker PRIVATE minima_file_port)
add_test(NAME txpow_processor_worker COMMAND txpow_processor_worker ${CMAKE_CURRENT_BINARY_DIR}/txpow-processor-fixture.json)
set_tests_properties(txpow_processor_worker PROPERTIES TIMEOUT 60 DEPENDS txpow_processor_differential)

add_executable(minima_notify_manager support/notify_manager.cpp)
target_link_libraries(minima_notify_manager PRIVATE minima_file_port)
add_test(NAME notify_manager_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/notify_manager_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(notify_manager_differential PROPERTIES TIMEOUT 240)
add_executable(notify_manager_worker support/notify_manager_worker.cpp)
target_link_libraries(notify_manager_worker PRIVATE minima_file_port)
add_test(NAME notify_manager_worker COMMAND notify_manager_worker)
set_tests_properties(notify_manager_worker PROPERTIES TIMEOUT 30)

add_executable(minima_rpc_client support/rpc_client.cpp)
target_link_libraries(minima_rpc_client PRIVATE minima_file_port)
add_test(NAME rpc_client_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/rpc_client_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(rpc_client_differential PROPERTIES TIMEOUT 300)
add_executable(notify_http_worker support/notify_http_worker.cpp)
target_link_libraries(notify_http_worker PRIVATE minima_file_port)
add_executable(rpc_configuration_native support/rpc_configuration_native.cpp)
target_link_libraries(rpc_configuration_native PRIVATE minima_file_port)
add_test(NAME rpc_configuration_native COMMAND rpc_configuration_native)
set_tests_properties(rpc_configuration_native PROPERTIES TIMEOUT 30)

add_executable(minima_logger support/minima_logger.cpp)
target_link_libraries(minima_logger PRIVATE minima_file_port)
add_test(NAME minima_logger_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_logger_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(minima_logger_differential PROPERTIES TIMEOUT 240)
add_executable(minima_logger_worker support/minima_logger_worker.cpp)
target_link_libraries(minima_logger_worker PRIVATE minima_file_port)
add_test(NAME minima_logger_worker COMMAND minima_logger_worker)
set_tests_properties(minima_logger_worker PROPERTIES TIMEOUT 30)

add_executable(minima-rpc support/minima_rpc_main.cpp)
target_link_libraries(minima-rpc PRIVATE minima_file_port)

add_executable(minima_rpc_cli support/minima_rpc_cli.cpp)
target_link_libraries(minima_rpc_cli PRIVATE minima_file_port)
add_executable(native_stack_trace support/native_stack_trace.cpp)
target_link_libraries(native_stack_trace PRIVATE minima_file_port)
add_test(NAME native_stack_trace COMMAND native_stack_trace)
set_tests_properties(native_stack_trace PROPERTIES TIMEOUT 30)
add_test(NAME minima_rpc_cli_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_rpc_cli_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(minima_rpc_cli_differential PROPERTIES TIMEOUT 300)

add_executable(minima_param_configurer support/param_configurer.cpp)
target_link_libraries(minima_param_configurer PRIVATE minima_file_port)
add_executable(param_configurer_process support/param_configurer_process.cpp)
target_link_libraries(param_configurer_process PRIVATE minima_file_port)
add_test(NAME param_configurer_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/param_configurer_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(param_configurer_differential PROPERTIES TIMEOUT 300)
add_executable(ParamConfigurerTest test/org/minima/system/params/ParamConfigurerTest.cpp)
target_link_libraries(ParamConfigurerTest PRIVATE minima_file_port)
add_test(NAME ParamConfigurerTest COMMAND ParamConfigurerTest ${CMAKE_CURRENT_BINARY_DIR}/param-configurer-original.conf)

add_executable(minima_uncaught support/minima_uncaught.cpp)
target_link_libraries(minima_uncaught PRIVATE minima_file_port)
add_test(NAME minima_uncaught_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_uncaught_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(minima_uncaught_differential PROPERTIES TIMEOUT 240)
add_executable(uncaught_workers support/uncaught_workers.cpp)
target_link_libraries(uncaught_workers PRIVATE minima_file_port)
add_executable(native_thread_checks support/native_thread_checks.cpp)
target_link_libraries(native_thread_checks PRIVATE minima_file_port)
add_test(NAME native_thread_checks COMMAND native_thread_checks)
set_tests_properties(native_thread_checks PROPERTIES TIMEOUT 30)

add_executable(http_server_errors support/http_server_errors.cpp)
target_link_libraries(http_server_errors PRIVATE minima_file_port)
add_test(NAME http_server_errors_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/http_server_errors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(http_server_errors_differential PROPERTIES TIMEOUT 240)
add_executable(executor_uncaught support/executor_uncaught.cpp)
target_link_libraries(executor_uncaught PRIVATE minima_file_port)
add_test(NAME executor_uncaught_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/executor_uncaught_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(executor_uncaught_differential PROPERTIES TIMEOUT 240)

add_executable(nio_server_errors support/nio_server_errors.cpp)
target_link_libraries(nio_server_errors PRIVATE minima_file_port)
add_test(NAME nio_server_errors_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_server_errors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(nio_server_errors_differential PROPERTIES TIMEOUT 240)
add_executable(nio_manager_errors support/nio_manager_errors.cpp)
target_link_libraries(nio_manager_errors PRIVATE minima_file_port)
add_test(NAME nio_manager_errors_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/nio_manager_errors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(nio_manager_errors_differential PROPERTIES TIMEOUT 240)

add_executable(timed_checker_errors support/timed_checker_errors.cpp)
target_link_libraries(timed_checker_errors PRIVATE minima_file_port)
add_test(NAME timed_checker_errors_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/timed_checker_errors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(timed_checker_errors_differential PROPERTIES TIMEOUT 300)
add_test(NAME timed_checker_error_deadline COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/timed_checker_errors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --deadline)
set_tests_properties(timed_checker_error_deadline PROPERTIES TIMEOUT 180)

add_executable(checker_logs support/checker_logs.cpp)
target_link_libraries(checker_logs PRIVATE minima_file_port)
add_test(NAME checker_logs_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/checker_logs_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(checker_logs_differential PROPERTIES TIMEOUT 300)

add_test(NAME script_value_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(script_value_original_tests PROPERTIES TIMEOUT 90)
add_test(NAME hex_value_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class HexValueTests)
add_test(NAME number_value_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class NumberValueTests)
add_executable(MinimaFunctionTests test/org/minima/tests/kissvm/functions/MinimaFunctionTests.cpp test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.cpp)
target_link_libraries(MinimaFunctionTests PRIVATE minima_file_port)
add_test(NAME MinimaFunctionTests COMMAND MinimaFunctionTests)
add_executable(ABSTests test/org/minima/tests/kissvm/functions/maths/ABSTests.cpp)
target_link_libraries(ABSTests PRIVATE minima_file_port)
add_test(NAME ABSTests COMMAND ABSTests)
add_test(NAME minima_function_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class MinimaFunctionTests)
add_test(NAME abs_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class ABSTests)
add_executable(command_runner_errors support/command_runner_errors.cpp)
target_link_libraries(command_runner_errors PRIVATE minima_file_port)
add_test(NAME command_runner_errors_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/command_runner_errors_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(hex_value_original_tests PROPERTIES TIMEOUT 90)

add_executable(FLOORTests test/org/minima/tests/kissvm/functions/maths/FLOORTests.cpp)
target_link_libraries(FLOORTests PRIVATE minima_file_port)
add_test(NAME FLOORTests COMMAND FLOORTests)
add_test(NAME floor_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class FLOORTests)
set_tests_properties(floor_original_tests PROPERTIES TIMEOUT 90)

add_executable(CEILTests test/org/minima/tests/kissvm/functions/maths/CEILTests.cpp)
target_link_libraries(CEILTests PRIVATE minima_file_port)
add_test(NAME CEILTests COMMAND CEILTests)
add_test(NAME ceil_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class CEILTests)
set_tests_properties(ceil_original_tests PROPERTIES TIMEOUT 90)

add_executable(wallet_commands support/wallet_commands.cpp)
target_link_libraries(wallet_commands PRIVATE minima_file_port)
add_test(NAME wallet_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(wallet_commands_differential PROPERTIES TIMEOUT 300)

add_test(NAME wallet_address_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group addresses)
set_tests_properties(wallet_address_commands_differential PROPERTIES TIMEOUT 900)

add_executable(MINTests test/org/minima/tests/kissvm/functions/maths/MINTests.cpp)
target_link_libraries(MINTests PRIVATE minima_file_port)
add_test(NAME MINTests COMMAND MINTests)
set_tests_properties(MINTests PROPERTIES TIMEOUT 180)
add_test(NAME min_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class MINTests)
set_tests_properties(min_original_tests PROPERTIES TIMEOUT 400)

add_executable(MAXTests test/org/minima/tests/kissvm/functions/maths/MAXTests.cpp)
target_link_libraries(MAXTests PRIVATE minima_file_port)
add_test(NAME MAXTests COMMAND MAXTests)
set_tests_properties(MAXTests PROPERTIES TIMEOUT 180)
add_test(NAME max_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class MAXTests)
set_tests_properties(max_original_tests PROPERTIES TIMEOUT 400)

add_test(NAME wallet_witness_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group witness)
set_tests_properties(wallet_witness_commands_differential PROPERTIES TIMEOUT 600)

add_executable(minima_coin_streams support/coin_streams.cpp)
target_link_libraries(minima_coin_streams PRIVATE minima_file_port)
add_test(NAME coin_streams_differential COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/coin_streams_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(coin_streams_differential PROPERTIES TIMEOUT 300)

add_executable(MiniStringTests test/org/minima/objects/base/MiniStringTests.cpp)
target_link_libraries(MiniStringTests PRIVATE minima_file_port)
add_test(NAME MiniStringTests COMMAND MiniStringTests)
add_test(NAME ministring_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class MiniStringTests)
set_tests_properties(ministring_original_tests PROPERTIES TIMEOUT 90)

add_test(NAME chain_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group chain-commands)
set_tests_properties(chain_commands_differential PROPERTIES TIMEOUT 300)

add_test(NAME transaction_input_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group inputs)
set_tests_properties(transaction_input_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME transaction_check_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group check)
set_tests_properties(transaction_check_command_differential PROPERTIES TIMEOUT 600)

add_executable(INCTests test/org/minima/tests/kissvm/functions/maths/INCTests.cpp)
target_link_libraries(INCTests PRIVATE minima_file_port)
add_test(NAME INCTests COMMAND INCTests)
add_test(NAME inc_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class INCTests)

add_executable(DECTests test/org/minima/tests/kissvm/functions/maths/DECTests.cpp)
target_link_libraries(DECTests PRIVATE minima_file_port)
add_test(NAME DECTests COMMAND DECTests)
add_test(NAME dec_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class DECTests)

add_test(NAME transaction_mining_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group mining)
set_tests_properties(transaction_mining_commands_differential PROPERTIES TIMEOUT 600)

add_executable(POWTests test/org/minima/tests/kissvm/functions/maths/POWTests.cpp)
target_link_libraries(POWTests PRIVATE minima_file_port)
add_test(NAME POWTests COMMAND POWTests)
add_test(NAME pow_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class POWTests)

add_executable(SIGDIGTests test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.cpp)
target_link_libraries(SIGDIGTests PRIVATE minima_file_port)
add_test(NAME SIGDIGTests COMMAND SIGDIGTests)
add_test(NAME sigdig_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class SIGDIGTests)

add_executable(BITCOUNTTests test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.cpp)
target_link_libraries(BITCOUNTTests PRIVATE minima_file_port)
add_test(NAME BITCOUNTTests COMMAND BITCOUNTTests)
add_test(NAME bitcount_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class BITCOUNTTests)

add_test(NAME vault_signing_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group vault)
set_tests_properties(vault_signing_commands_differential PROPERTIES TIMEOUT 600)

add_executable(BITGETTests test/org/minima/tests/kissvm/functions/maths/BITGETTests.cpp)
target_link_libraries(BITGETTests PRIVATE minima_file_port)
add_test(NAME BITGETTests COMMAND BITGETTests)
add_test(NAME bitget_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class BITGETTests)

add_executable(BITSETTests test/org/minima/tests/kissvm/functions/maths/BITSETTests.cpp)
target_link_libraries(BITSETTests PRIVATE minima_file_port)
add_test(NAME BITSETTests COMMAND BITSETTests)
add_test(NAME bitset_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class BITSETTests)

add_test(NAME wallet_key_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group keys)
set_tests_properties(wallet_key_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME coin_funding_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group funding)
set_tests_properties(coin_funding_commands_differential PROPERTIES TIMEOUT 900)

add_executable(REVTests test/org/minima/tests/kissvm/functions/base/REVTests.cpp)
target_link_libraries(REVTests PRIVATE minima_file_port)
add_test(NAME REVTests COMMAND REVTests)
add_test(NAME rev_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class REVTests)

add_executable(LENTests test/org/minima/tests/kissvm/functions/base/LENTests.cpp)
target_link_libraries(LENTests PRIVATE minima_file_port)
add_test(NAME LENTests COMMAND LENTests)
add_test(NAME len_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class LENTests)

add_test(NAME offline_send_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group offline)
set_tests_properties(offline_send_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME unsigned_send_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group unsigned)
set_tests_properties(unsigned_send_command_differential PROPERTIES TIMEOUT 600)

add_executable(minima_sendpoll_command support/sendpoll_command.cpp)
target_link_libraries(minima_sendpoll_command PRIVATE minima_file_port)
add_test(NAME sendpoll_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/sendpoll_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sendpoll_command_differential PROPERTIES TIMEOUT 240)

add_executable(wallet_wrappers support/wallet_wrappers.cpp)
target_link_libraries(wallet_wrappers PRIVATE minima_file_port)
add_test(NAME wallet_wrappers_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_wrappers_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(wallet_wrappers_differential PROPERTIES TIMEOUT 600)

add_test(NAME raw_transaction_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_wrappers_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group raw)
set_tests_properties(raw_transaction_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME custom_address_send_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_wrappers_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group custom)
set_tests_properties(custom_address_send_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME consolidate_address_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_wrappers_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group consolidate)
set_tests_properties(consolidate_address_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME wallet_consolidation_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group consolidation)
set_tests_properties(wallet_consolidation_command_differential PROPERTIES TIMEOUT 600)

add_test(NAME wallet_balance_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group balance)
set_tests_properties(wallet_balance_command_differential PROPERTIES TIMEOUT 600)

add_test(NAME coin_search_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group coins)
set_tests_properties(coin_search_command_differential PROPERTIES TIMEOUT 600)

add_test(NAME transaction_history_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group history)
set_tests_properties(transaction_history_command_differential PROPERTIES TIMEOUT 600)

add_test(NAME txpow_search_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group txpow)
set_tests_properties(txpow_search_command_differential PROPERTIES TIMEOUT 600)

add_test(NAME node_state_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group node_state)
set_tests_properties(node_state_commands_differential PROPERTIES TIMEOUT 600)

add_test(NAME seed_random_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/wallet_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group seedrandom)
set_tests_properties(seed_random_command_differential PROPERTIES TIMEOUT 600)

add_executable(BOOLTests test/org/minima/tests/kissvm/functions/cast/BOOLTests.cpp)
target_link_libraries(BOOLTests PRIVATE minima_file_port)
add_test(NAME BOOLTests COMMAND BOOLTests)
add_test(NAME bool_cast_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class BOOLTests)

add_executable(HEXTests test/org/minima/tests/kissvm/functions/cast/HEXTests.cpp)
target_link_libraries(HEXTests PRIVATE minima_file_port)
add_test(NAME HEXTests COMMAND HEXTests)
add_test(NAME hex_cast_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class HEXTests)

add_executable(NUMBERTests test/org/minima/tests/kissvm/functions/cast/NUMBERTests.cpp)
target_link_libraries(NUMBERTests PRIVATE minima_file_port)
add_test(NAME NUMBERTests COMMAND NUMBERTests)
add_test(NAME number_cast_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class NUMBERTests)

add_executable(SCRIPTTests test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.cpp)
target_link_libraries(SCRIPTTests PRIVATE minima_file_port)
add_test(NAME SCRIPTTests COMMAND SCRIPTTests)
add_test(NAME script_cast_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class SCRIPTTests)

add_executable(minima_mysql_connect support/mysql_connect.cpp)
add_executable(archive_server_runtime support/archive_server.cpp)
target_link_libraries(archive_server_runtime PRIVATE minima_file_port)
add_test(NAME archive_server_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_server_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(archive_server_differential PROPERTIES TIMEOUT 600)
add_test(NAME archive_server_clean_audit COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_server_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --clean-audit)
set_tests_properties(archive_server_clean_audit PROPERTIES TIMEOUT 120 DEPENDS archive_server_differential)
target_link_libraries(minima_mysql_connect PRIVATE minima_file_port)
add_test(NAME mysql_connect_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/mysql_connect_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(mysql_connect_differential PROPERTIES TIMEOUT 600)

add_test(NAME pulse_runtime_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_db_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group pulse)
set_tests_properties(pulse_runtime_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/Main.cpp)

add_executable(minima_main_runtime support/main_runtime.cpp)
target_link_libraries(minima_main_runtime PRIVATE minima_file_port)

add_test(NAME main_runtime_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(main_runtime_differential PROPERTIES TIMEOUT 600)

add_executable(main_ownership support/main_ownership.cpp)
target_link_libraries(main_ownership PRIVATE minima_file_port)
add_test(NAME main_ownership COMMAND main_ownership)
set_tests_properties(main_ownership PROPERTIES TIMEOUT 45)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/quit.cpp src/org/minima/system/commands/backup/rescue.cpp)

add_test(NAME main_lifecycle_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group commands)
set_tests_properties(main_lifecycle_commands_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/network/disconnect.cpp src/org/minima/system/commands/network/message.cpp src/org/minima/system/commands/network/p2pstate.cpp)

target_sources(minima_main_runtime PRIVATE support/MainNetworkFixture.cpp)

add_test(NAME network_peer_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group network)
set_tests_properties(network_peer_commands_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/network/ping.cpp)

add_test(NAME ping_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/ping_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ping_command_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/network/network.cpp)
add_test(NAME network_status_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group status)
set_tests_properties(network_status_command_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/trace.cpp src/org/minima/system/commands/base/timemilli.cpp)

add_test(NAME trace_clock_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group diagnostics)
set_tests_properties(trace_clock_command_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/healthcheck.cpp)
add_test(NAME healthcheck_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group health)
set_tests_properties(healthcheck_command_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/logs.cpp)
add_test(NAME logs_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group logs)
set_tests_properties(logs_command_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/status.cpp)
add_test(NAME node_status_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group node-status)
set_tests_properties(node_status_command_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/network/rpc.cpp)
add_test(NAME rpc_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group rpc)
set_tests_properties(rpc_command_differential PROPERTIES TIMEOUT 600)

add_executable(json_collections support/json_collections.cpp)
target_link_libraries(json_collections PRIVATE minima_file_port)
add_test(NAME json_collections_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/json_collections_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(json_collections_differential PROPERTIES TIMEOUT 120)
add_executable(shared_json_native support/shared_json_native.cpp)
target_link_libraries(shared_json_native PRIVATE minima_file_port)
add_test(NAME shared_json_native COMMAND shared_json_native)
set_tests_properties(shared_json_native PROPERTIES TIMEOUT 60)
add_executable(rpc_live support/rpc_live.cpp support/MainNetworkFixture.cpp)
target_link_libraries(rpc_live PRIVATE minima_file_port)
add_test(NAME rpc_live_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/rpc_live_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(rpc_live_differential PROPERTIES TIMEOUT 180)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/hashtest.cpp)
# Reuse the Main fixture in a distinct binary while prior full suites finish.
add_executable(minima_hashtest_runtime support/main_runtime.cpp support/MainNetworkFixture.cpp)
target_link_libraries(minima_hashtest_runtime PRIVATE minima_file_port)
add_test(NAME hashtest_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group hashtest --native-target minima_hashtest_runtime)
set_tests_properties(hashtest_command_differential PROPERTIES TIMEOUT 240)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/coinimport.cpp)
add_executable(coin_import_runtime support/wallet_commands.cpp)
target_link_libraries(coin_import_runtime PRIVATE minima_file_port)
add_test(NAME coin_import_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/coin_import_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(coin_import_differential PROPERTIES TIMEOUT 240)


target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/scanchain.cpp)
add_executable(scan_chain_runtime support/wallet_commands.cpp)
target_link_libraries(scan_chain_runtime PRIVATE minima_file_port)
add_test(NAME scan_chain_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scan_chain_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(scan_chain_differential PROPERTIES TIMEOUT 240)


target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/tokenvalidate.cpp)
add_executable(token_validation_runtime support/wallet_commands.cpp)
target_link_libraries(token_validation_runtime PRIVATE minima_file_port)
add_test(NAME token_validation_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/token_validation_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(token_validation_differential PROPERTIES TIMEOUT 240)


target_sources(minima_file_port PRIVATE src/org/minima/system/commands/network/peers.cpp)
add_executable(peers_command_runtime support/main_runtime.cpp support/MainNetworkFixture.cpp)
target_link_libraries(peers_command_runtime PRIVATE minima_file_port)
add_test(NAME peers_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/peers_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(peers_command_differential PROPERTIES TIMEOUT 240)


target_sources(minima_file_port PRIVATE src/org/minima/system/commands/txn/txnauto.cpp)
add_executable(automatic_transaction_runtime support/wallet_commands.cpp)
target_link_libraries(automatic_transaction_runtime PRIVATE minima_file_port)
add_test(NAME automatic_transaction_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/automatic_transaction_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(automatic_transaction_differential PROPERTIES TIMEOUT 300)


target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/burn.cpp)
add_executable(burn_metrics_runtime support/wallet_commands.cpp)
target_link_libraries(burn_metrics_runtime PRIVATE minima_file_port)
add_test(NAME burn_metrics_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/burn_metrics_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(burn_metrics_differential PROPERTIES TIMEOUT 300)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/tokencreate.cpp)
add_executable(token_creation_runtime support/wallet_commands.cpp)
target_link_libraries(token_creation_runtime PRIVATE minima_file_port)
add_test(NAME token_creation_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/token_creation_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(token_creation_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/send/multisig.cpp src/org/minima/system/commands/send/multisigread.cpp src/support/MultisigCommands.cpp)
add_executable(multisig_runtime support/wallet_commands.cpp)
target_link_libraries(multisig_runtime PRIVATE minima_file_port)
add_test(NAME multisig_workflow_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/multisig_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(multisig_workflow_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/network/nodecount.cpp)
add_executable(node_count_runtime support/node_count.cpp)
target_link_libraries(node_count_runtime PRIVATE minima_file_port)
add_test(NAME node_count_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/node_count_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(node_count_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/incentivecash.cpp)
add_executable(incentive_cash_runtime support/wallet_commands.cpp)
target_link_libraries(incentive_cash_runtime PRIVATE minima_file_port)
add_test(NAME incentive_cash_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/incentive_cash_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(incentive_cash_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/utils/json/ItemList.cpp)
add_executable(item_list_runtime support/item_list.cpp)
target_link_libraries(item_list_runtime PRIVATE minima_file_port)
add_test(NAME item_list_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/item_list_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(item_list_differential PROPERTIES TIMEOUT 180)

target_sources(minima_file_port PRIVATE src/org/minima/Minima.cpp src/support/ConsoleInput.cpp src/support/NativeShutdownHooks.cpp)
add_executable(MinimaCoreCpp support/minima_entry.cpp)
set_target_properties(MinimaCoreCpp PROPERTIES OUTPUT_NAME "MinimaCoreC++")
add_executable(minima_archive_server support/archive_server_entry.cpp)
add_executable(minima_entry_boundary support/minima_entry_boundary.cpp)
target_link_libraries(minima_entry_boundary PRIVATE minima_file_port)
add_executable(minima_shutdown_hooks support/minima_shutdown_hooks.cpp)
target_link_libraries(minima_shutdown_hooks PRIVATE minima_file_port)

set(MINIMA_NATIVE_H2_DIR "" CACHE PATH "Directory containing the compiled native H2 bridge and generated headers")
set(MINIMA_MYSQL_FIXTURE_FILE "" CACHE FILEPATH "Optional isolated localhost MySQL fixture JSON for live integration tests")
if(MINIMA_NATIVE_H2_DIR)
    # Changing the provider directory must not reuse a library from the old cache.
    unset(MINIMA_NATIVE_H2_LIBRARY CACHE)
    find_library(MINIMA_NATIVE_H2_LIBRARY NAMES minima_h2 PATHS "${MINIMA_NATIVE_H2_DIR}" NO_DEFAULT_PATH REQUIRED)
    add_library(minima_native_h2 STATIC src/support/NativeH2Connection.cpp)
    target_include_directories(minima_native_h2 PRIVATE "${MINIMA_NATIVE_H2_DIR}")
    target_link_libraries(minima_native_h2 PUBLIC minima_file_port "${MINIMA_NATIVE_H2_LIBRARY}")
    target_link_libraries(MinimaCoreCpp PRIVATE minima_native_h2)
    target_compile_definitions(MinimaCoreCpp PRIVATE MINIMA_NATIVE_H2=1)
    target_link_libraries(minima_archive_server PRIVATE minima_native_h2)
    target_compile_definitions(minima_archive_server PRIVATE MINIMA_NATIVE_H2=1)
    add_executable(native_h2_runtime support/native_h2.cpp)
    target_link_libraries(native_h2_runtime PRIVATE minima_native_h2)
    add_test(NAME native_h2_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_h2_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_h2_differential PROPERTIES TIMEOUT 300)
    add_executable(native_h2_concurrent support/native_h2_concurrent.cpp)
    target_link_libraries(native_h2_concurrent PRIVATE minima_native_h2)
    add_test(NAME native_h2_concurrent COMMAND native_h2_concurrent)
    set_tests_properties(native_h2_concurrent PROPERTIES TIMEOUT 120)
    if(MINIMA_MYSQL_FIXTURE_FILE)
        add_test(NAME native_mysql_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_mysql_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --fixture ${MINIMA_MYSQL_FIXTURE_FILE})
        add_test(NAME native_mysql_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_mysql_startup.py --build ${CMAKE_CURRENT_BINARY_DIR} --fixture ${MINIMA_MYSQL_FIXTURE_FILE})
        set_tests_properties(native_mysql_differential native_mysql_startup PROPERTIES TIMEOUT 1800 RUN_SERIAL TRUE)
        set_tests_properties(native_mysql_startup PROPERTIES DEPENDS native_mysql_differential)
        add_test(NAME archive_server_main_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_server_main_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --fixture ${MINIMA_MYSQL_FIXTURE_FILE})
        set_tests_properties(archive_server_main_differential PROPERTIES TIMEOUT 600 RUN_SERIAL TRUE DEPENDS archive_server_differential)
        add_test(NAME native_mysql_archive_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_mysql_archive_startup.py --build ${CMAKE_CURRENT_BINARY_DIR} --fixture ${MINIMA_MYSQL_FIXTURE_FILE})
        set_tests_properties(native_mysql_archive_startup PROPERTIES TIMEOUT 3600 RUN_SERIAL TRUE)
    endif()
    add_test(NAME native_node_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_node_startup.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_node_startup PROPERTIES TIMEOUT 1200)
else()
    target_link_libraries(MinimaCoreCpp PRIVATE minima_file_port)
    target_link_libraries(minima_archive_server PRIVATE minima_file_port)
endif()
add_test(NAME minima_entry_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/minima_entry_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --native-h2 $<BOOL:${MINIMA_NATIVE_H2_DIR}>)
set_tests_properties(minima_entry_differential PROPERTIES TIMEOUT 240)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/decryptbackup.cpp)
add_executable(decrypt_backup_runtime support/decrypt_backup.cpp)
target_link_libraries(decrypt_backup_runtime PRIVATE minima_file_port)
add_test(NAME decrypt_backup_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/decrypt_backup_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(decrypt_backup_differential PROPERTIES TIMEOUT 300)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/backup.cpp)
if(TARGET minima_native_h2)
    add_executable(backup_runtime support/backup.cpp)
    target_link_libraries(backup_runtime PRIVATE minima_native_h2)
    add_test(NAME backup_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/backup_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(backup_differential PROPERTIES TIMEOUT 300)
endif()

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/restore.cpp)
if(TARGET minima_native_h2)
    add_executable(restore_runtime support/restore.cpp)
    target_link_libraries(restore_runtime PRIVATE minima_native_h2)
    add_test(NAME restore_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/restore_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(restore_differential PROPERTIES TIMEOUT 300)
endif()
if(TARGET minima_native_h2)
    add_test(NAME native_restore_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_restore_startup.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_restore_startup PROPERTIES TIMEOUT 1800)
endif()

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/reset.cpp)
if(TARGET minima_native_h2)
    add_test(NAME reset_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/reset_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(reset_differential PROPERTIES TIMEOUT 300)
endif()

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/archive.cpp)
if(TARGET minima_native_h2)
    add_executable(archive_command_runtime support/archive_command.cpp support/MainNetworkFixture.cpp)
    target_link_libraries(archive_command_runtime PRIVATE minima_native_h2)
    add_test(NAME archive_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(archive_command_differential PROPERTIES TIMEOUT 600)
endif()

if(TARGET minima_native_h2)
    add_test(NAME native_archive_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_archive_startup.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_archive_startup PROPERTIES TIMEOUT 2400)
    add_test(NAME archive_network_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/archive_network_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(archive_network_differential PROPERTIES TIMEOUT 600)
endif()

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/restoresync.cpp)

if(TARGET minima_native_h2)
    add_test(NAME restoresync_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/restore_sync_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(restoresync_differential PROPERTIES TIMEOUT 600)
    add_test(NAME native_restoresync_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_restore_startup.py --build ${CMAKE_CURRENT_BINARY_DIR} --restore-command restoresync)
    set_tests_properties(native_restoresync_startup PROPERTIES TIMEOUT 1800)
    add_test(NAME native_restoresync_remote COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_restore_sync_remote.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_restoresync_remote PROPERTIES TIMEOUT 2400)
endif()


target_sources(minima_file_port PRIVATE
    src/org/minima/system/commands/backup/mmrsync/megammr.cpp
    src/org/minima/system/commands/backup/mmrsync/megammrsync.cpp)

if(TARGET minima_native_h2)
    add_test(NAME mega_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/mega_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(mega_commands_differential PROPERTIES TIMEOUT 1800)
    add_test(NAME native_mega_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_mega_startup.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_mega_startup PROPERTIES TIMEOUT 2400)
endif()

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/systemcheck.cpp)
add_test(NAME systemcheck_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/main_runtime_differential.py --build ${CMAKE_CURRENT_BINARY_DIR} --group systemcheck)
set_tests_properties(systemcheck_commands_differential PROPERTIES TIMEOUT 600)
if(TARGET minima_native_h2)
    add_test(NAME native_systemcheck_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_systemcheck_startup.py --build ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(native_systemcheck_startup PROPERTIES TIMEOUT 1800)
endif()

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/sphincs.cpp)
add_executable(minima_sphincs_command support/sphincs_command.cpp)
target_link_libraries(minima_sphincs_command PRIVATE minima_file_port)
add_test(NAME sphincs_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/sphincs_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(sphincs_command_differential PROPERTIES TIMEOUT 14400)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/base/test.cpp)
add_executable(minima_base_test_demo support/base_test_demo.cpp)
target_link_libraries(minima_base_test_demo PRIVATE minima_file_port)
add_test(NAME base_test_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/base_test_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(base_test_command_differential PROPERTIES TIMEOUT 900)
add_test(NAME native_base_test_startup COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/native_base_test_startup.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(native_base_test_startup PROPERTIES TIMEOUT 1800 DEPENDS base_test_command_differential)

target_sources(minima_file_port PRIVATE
    src/org/minima/system/commands/tutorial.cpp
    src/org/minima/system/commands/base/tutorial.cpp
    src/org/minima/system/commands/whitepaper.cpp)
add_executable(minima_text_commands support/text_commands.cpp)
target_link_libraries(minima_text_commands PRIVATE minima_file_port)
add_test(NAME text_commands_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/text_commands_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(text_commands_differential PROPERTIES TIMEOUT 180)

add_executable(CONCATTests test/org/minima/tests/kissvm/functions/base/CONCATTests.cpp)
target_link_libraries(CONCATTests PRIVATE minima_file_port)
add_test(NAME CONCATTests COMMAND CONCATTests)
add_test(NAME concat_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class CONCATTests)
set_tests_properties(CONCATTests concat_original_tests PROPERTIES TIMEOUT 180 ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1")

add_executable(GETTests test/org/minima/tests/kissvm/functions/base/GETTests.cpp)
target_link_libraries(GETTests PRIVATE minima_file_port)
add_test(NAME GETTests COMMAND GETTests)
add_test(NAME get_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class GETTests)
set_tests_properties(GETTests get_original_tests PROPERTIES TIMEOUT 180 ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1")

add_executable(SUBSETTests test/org/minima/tests/kissvm/functions/base/SUBSETTests.cpp)
target_link_libraries(SUBSETTests PRIVATE minima_file_port)
add_test(NAME SUBSETTests COMMAND SUBSETTests)
add_test(NAME subset_original_tests COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/script_value_original_tests.py --build ${CMAKE_CURRENT_BINARY_DIR} --test-class SUBSETTests)
set_tests_properties(SUBSETTests subset_original_tests PROPERTIES TIMEOUT 180 ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1")

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/mysqlcoins.cpp)
add_executable(minima_mysql_coins support/mysql_coins.cpp)
target_link_libraries(minima_mysql_coins PRIVATE minima_file_port)
add_test(NAME mysql_coins_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/mysql_coins_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(mysql_coins_differential PROPERTIES TIMEOUT 600)

target_sources(minima_file_port PRIVATE src/org/minima/system/commands/backup/mysql.cpp)
add_executable(minima_mysql_command support/mysql_command.cpp)
target_link_libraries(minima_mysql_command PRIVATE minima_file_port)
add_test(NAME mysql_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/mysql_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(mysql_command_differential PROPERTIES TIMEOUT 900)

# Pinned curated help catalogue and actual registered metadata.
target_sources(minima_file_port PRIVATE src/org/minima/system/commands/help.cpp)
add_executable(minima_help_command support/help_command.cpp)
target_link_libraries(minima_help_command PRIVATE minima_file_port)
add_test(NAME help_command_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/help_command_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(help_command_differential PROPERTIES TIMEOUT 180)

# Actual serialization interface and initial concrete integration.
target_sources(minima_file_port PRIVATE src/org/minima/utils/Streamable.cpp)
add_executable(minima_streamable support/streamable.cpp)
target_link_libraries(minima_streamable PRIVATE minima_file_port)
add_test(NAME streamable_differential COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/streamable_differential.py --build ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(streamable_differential PROPERTIES TIMEOUT 300)
