# Lab: indexed variables and byte slices

This plan is written before executing the mapped GETTests and SUBSETTests.
Both production functions already exist. We reuse their actual native classes,
the registry and Contract, with the assertion/expression adapters from CONCATTests
and LENTests. Java wrappers will invoke the three unchanged methods in each file.

GET turns numeric arguments into a comma-terminated variable name. Arguments
`0, 41, 0` form `0,41,0,`; this is a key in the contract's variable map, not an
array allocation. The original fixture contains eleven numeric keys and checks
each stored value's numeric type and integer value. Its valid fixture also stores
a mixed-type-looking key that it never reads. We retain that setup but do not
count it as evidence that GET accepts nonnumeric arguments. The rejection cases
cover zero arguments, three nonnumeric types, and a missing numeric key.

GETTests executes 31 assertions: four factory/registry checks, 22 successful
lookup checks, and five rejections. It does not check returned-value identity,
mutation through aliases, arbitrary numeric formatting or expression side effects.

SUBSET takes a start index, an exclusive end index, and hexadecimal data. With
bytes `01 23 45 67`, start 0 and end 2 return `01 23`; end 4 returns all four bytes.
The two successful cases each check the HexValue type and exact displayed bytes.
Six cases inside the method labelled “valid” deliberately expect exceptions:
out-of-range positions or script arguments. The labels and old comments are not
the executable specification. Keep the duplicate out-of-range case as written.

The invalid method adds three argument-count, two range and fourteen type-related
rejections. Some cases have multiple invalid types, so an exception alone does not
prove which validation ran first. In total SUBSETTests executes 33 assertions:
four metadata checks, four successful-result checks, and 25 rejections.

The sibling project's SUBSET takes `(hex, start, length)` and therefore cannot
replace the current native function, which follows `(start, end, hex)`. We will
run both mapped tests and unchanged Java methods in normal and sanitizer builds.
These fixtures do not cover integer overflow or the maximum permitted data size;
passing them is evidence for the stated cases, not full API parity.

Both complete builds and all four registered tests pass in each build. The normal
CTest total is 7.93 seconds and the sanitizer total is 7.86 seconds. Each runtime
executes the three original methods per file, with 31 GET and 33 SUBSET assertions.
Native stderr is empty, and UBSAN halts on errors. No production function changed.
