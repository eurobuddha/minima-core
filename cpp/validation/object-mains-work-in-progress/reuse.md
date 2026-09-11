# Reuse evidence

Pinned source: upstream-study at 5564b9d78978efcaf23679190d05253961fc6e69.
Read source Address.java and MiniData.java, TxPoWGenerator.java main, native
Address/MiniData/TxPoWGenerator headers and implementations, BaseConverter.cpp,
JavaExceptions.hpp, MiniNumber comparison/declaration surface, original Address
and MiniData tests and native mapped tests. Existing utility_mains.cpp,
UtilityMainsOracle.java and utility_mains_differential.py supplied fixture patterns.
The original_base_tests.py and original_object_tests.py runners supply regression
comparisons with real JUnit assertions.

Sibling search: /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src.
Read types/MiniData.hpp and getRandomData body, objects/Address.hpp/.cpp; no matching
main entry points or Base32 password helper were present. That older MiniData uses
mt19937_64, while the current implementation uses OS randomness. Its Address wire
format also lacks the current script field. Reuse the current tested implementation.

Production reuse: MiniData::getRandomData, withMinLength, to0xString, getDataValue;
BaseConverter::encode32; MiniNumber constants/compareTo/toString; existing
JavaTypedException/JavaNegativeArraySizeException; existing stable_sort pattern.
Only main bodies, two private post-random-draw helper boundaries and the negative
random-length exception changed. No new random implementation or public test mode.

Further caller review: system/commands/base/random.cpp uses the same five-group
Base32 loop, but its aggregate size<26 check loses the first failing substring and
throws out_of_range. That exception path cannot be reused unchanged for Address;
the main checks each original range. The command already rejects negative sizes
with JavaNegativeArraySizeException. PasswordCrypto requests a fixed positive size.
