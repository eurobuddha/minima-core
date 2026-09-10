# Wallet keys: two checks answer different questions

The pinned [keys.java](../../../upstream-study/src/org/minima/system/commands/search/keys.java) lists key records, creates stored keys, generates independent keys, checks public/private pairs and requests background default-key creation.

Listing prefers a public-key filter over a modifier filter and stops at the first match. maxuses means the greatest observed use count, starting at zero; it is not the maximum lifetime capacity. KeyRow.toJSON omits the private key.

checkkeys reconstructs each row's TreeKey and checks its public key. The static checkKey helper first checks that the private key derives from the wallet's base seed and modifier, then checks its public root. A private/public pair can agree with each other but still fail the seed check. These are different invariants, so do not replace one operation with the other.

genkey hashes the phrase returned by Command.getParam, which trims outer characters at or below U+0020, without BIP39 word cleaning, derives modifier zero and constructs a default TreeKey. It does not insert a wallet record. The no-phrase branch uses the existing random BIP39 word generator. new delegates to the actual Wallet.createNewKey. createallkeys parses the use count, posts MAIN_LOAD_ALL_KEYS and returns immediately; that reply does not prove the background work finished.

The port reuses Wallet, KeyRow, BIP39, TreeKey, Address and the existing Command/Message adapters. Tests use synthetic seed material and recording SQL, preserving returned records, logs, messages, failure categories and SQL order. Meaningful assertions distinguish a valid derived key from a valid but unrelated key pair. The Main receiver remains an explicit integration boundary. The first comparison found a null-filter failure: C++ exposed bad_variant_access where Java threw NullPointerException on the String receiver. The explicit receiver check fixes that path; the retained failure is a regression example. The corrected normal and ASAN/UBSAN runs both pass 132 programs, 262 operations and 7,895 recorded SQL calls (51.46 and 265.36 seconds respectively). The no-phrase random genkey branch is not exercised by this group.

An extra phrase assertion initially expected outer spaces to survive. Both implementations correctly removed them through Command.getParam before genkey used the string. The expectation now distinguishes parameter trimming from BIP39 cleaning and checks that an empty phrase is rejected by Command.getParam. No production change was needed.
