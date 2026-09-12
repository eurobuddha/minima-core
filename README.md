# MinimaCoreC++

A native C++ MinimaCore development project with source, Java/C++ comparison tests,
one student lesson for each of the 457 mapped Java files, and a runnable Apple
Silicon macOS package.

**Development status:** 456 of 457 counterparts exist. Full behavioral parity is
unverified; the external JNI implementation is missing, and reliable mainnet startup
and reopen remain open issues. Read the [validation report](cpp/delivery/VALIDATION_STATUS.md)
and [known gaps](cpp/delivery/KNOWN_GAPS.md) before using the node.

## Downloads and documentation

- [Development release and downloads](https://github.com/eurobuddha/MinimaCoreCpp/releases/tag/dev-2026-09-12): complete project ZIP, standalone macOS executable, checksums and verification record.
- [Team handover](cpp/delivery/START_HERE.md), [build instructions](cpp/delivery/BUILD_AND_TEST.md), and [delivery changes](cpp/delivery/CHANGES.md).
- [All 457 student lessons](cpp/student-guide/INDEX.md) and [study route](cpp/student-guide/how-to-study-this-port.md).
- [Presentation outline and speaker notes](cpp/delivery/PRESENTATION.md), [offline HTML presentation](cpp/delivery/presentation.html), and [demo script](cpp/delivery/DEMO.md).
- [Java/C++ file mapping](cpp/SOURCE_MAP.md), [source status](cpp/README.md), and [checkpoint workflow](cpp/GIT_WORKFLOW.md).

The runtime targets Apple Silicon macOS 26+, uses port 21001 by default, and creates
its own data directory. H2/MySQL is compiled ahead of time from Java; the node does
not launch a JVM. The release is a development preview, not a complete parity release.
Download the full project ZIP for a self-contained source snapshot, including the
pinned Java reference and all teaching files.

## Clone the source

```sh
git clone --recurse-submodules https://github.com/eurobuddha/MinimaCoreCpp.git
cd MinimaCoreCpp
```

If already cloned without submodules, run `git submodule update --init upstream-study`.
The reference is pinned to `5564b9d78978efcaf23679190d05253961fc6e69`. Follow the build
instructions for the native database provider and developer dependencies. The Git
repository preserves the original Java and port history. Executable packages are
release assets; runtime wallets, local databases and uncommitted files are excluded.

## Original Java application

Minima full node application 

This is an implementation of a Minima node running on the Minima network

It does not include the MDS system or Maxima - JUST Minima.. Clean.

Run from the command line with 

```
java -jar minima.jar
```

You can find all the cli params with

```
java -jar minima.jar -help
```

The main ones being ( these are the default values )

```
java -jar minima.jar -port 9001 -data ~/.minima
```

You will need to add a valid Minima peer to get started.. none are added by default 

You can do this by running a command in Minima once it starts

```
peers action:addpeers peerslist:spartacusrex.com:9001
```

Or you can specify that from the cli.. 

```
java -jar minima.jar -p2pnodes spartacusrex.com:9001
```

You can also provide a file with a list of nodes.. (preferred option)

```
peers action:addpeers peerslist:https://spartacusrex.com/minimapeers.txt
```

```
java -jar minima.jar -p2pnodes https://spartacusrex.com/minimapeers.txt
```

Once Minima has started use help to get a full list of functions available to you.. 

```
help
```

And to breakdown how to use a function

```
help command:send
```

To wipe all data and start again use -clean

### Build

You can build the project with

```
./gardlew clean build
```

Or use the helper script

```
./buildjars.sh
```

Which will build the jars and copy them to the jar folder

### Testing on private network

The best way to test Minima is to run a private network

```
java -jar minima.jar -solo
```

And to wipe the node clean and start again.. 

```
java -jar minima.jar -solo -clean
```

### MegaMMR node

Minima has 2 modes.. a simple 1 user node that works for a single user or a megammr node that stores all data - an exchange would use this. 

This allows other nodes to resync from it.. and sending from any account using the sendfrom functions.

```
java -jar minima.jar -solo -megammr
```
