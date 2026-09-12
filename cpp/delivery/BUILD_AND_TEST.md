# Reproducing the native build and tests

Run these commands from the `project` directory of the source delivery, or the
repository root in a Git checkout. The archive includes `upstream-study` with the
pinned original Java sources and dependency JARs. A fresh Git clone instead needs
`git submodule update --init upstream-study` before building.

## Prerequisites

The tested host is Apple Silicon macOS. The project uses a C++20 compiler, CMake,
Python 3, OpenSSL 3.2 or newer, libcurl 7.85 or newer, zlib, and ICU 78. The supplied
runtime contains its libraries and does not need these development tools installed.

Differential tests compile and run Java. The current test environment uses OpenJDK
26.0.1. Exact Java exception prose can vary between JDK versions, so use the tested
JDK when reproducing those comparisons. JUnit and other Java dependencies are
included under the pinned source tree.

The native database builder uses an installed GraalVM toolchain. The tested version
and pinned artifact hashes are documented in [the native H2 guide](../native/h2/README.md).
That builder performs no downloads and uses `--no-fallback`. Do not use a production
MySQL database for optional integration tests.

## Build the database dependency

Use the path to your local GraalVM installation in place of the example path:

```sh
python3 cpp/tools/build_native_h2.py \
  --graalvm /path/to/graalvm/Contents/Home \
  --mysql --output cpp/build/native-h2
```

This compiles the database bridge, H2 and MySQL driver to a native shared library.
It includes no Minima Java classes. Keep its `manifest.json` with the library.

## Build the node and registered test targets

Set `ICU_ROOT` to your actual ICU 78 installation. On the development Mac that is
`/opt/homebrew/opt/icu4c@78`.

```sh
cmake -S cpp -B cpp/build -DCMAKE_BUILD_TYPE=Debug \
  -DICU_ROOT=/opt/homebrew/opt/icu4c@78 \
  -DMINIMA_NATIVE_H2_DIR="$PWD/cpp/build/native-h2"
cmake --build cpp/build -j4
```

The node executable is `cpp/build/MinimaCoreC++`. Probe binaries such as
`minima_commands` and `minima_kissvm` serve tests and are not the complete node.
A build without the native database provider does not reproduce the runnable
node delivered here.

## Run the test inventory

```sh
ctest --test-dir cpp/build --show-only
ctest --test-dir cpp/build --output-on-failure -j4 \
  --output-junit "$PWD/cpp/build/delivery-junit.xml"
```

The delivery candidate has 294 registered tests. Several tests perform real
120-second waits or substantial cryptographic work. Do not shorten those source
operations and report the reduced run as equivalent coverage. Optional MySQL tests
are not registered when `MINIMA_MYSQL_FIXTURE_FILE` is empty. Their absence must
remain explicit in the report.

For native address/undefined-behavior checks, use a separate build directory:

```sh
cmake -S cpp -B cpp/build-sanitized -DCMAKE_BUILD_TYPE=Debug \
  -DICU_ROOT=/opt/homebrew/opt/icu4c@78 \
  -DMINIMA_NATIVE_H2_DIR="$PWD/cpp/build/native-h2" \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
cmake --build cpp/build-sanitized -j4
ctest --test-dir cpp/build-sanitized --output-on-failure -j4 \
  --output-junit "$PWD/cpp/build-sanitized/delivery-junit.xml"
```

Sanitizers instrument C++ code, not the AOT database library. Run the two suites
sequentially if they share external fixtures. Test reports under each build
include scoped differential observations. The delivery evidence preserves terminal
results rather than inferring test success from a successful build.

## Optimized candidate with assertions retained

The earlier normal build uses Debug (`-g`, no optimization). To evaluate the same
source with optimization while retaining C++ assertions, use a separate directory:

```sh
cmake -S cpp -B cpp/build-optimized -DCMAKE_BUILD_TYPE=Release \
  '-DCMAKE_CXX_FLAGS_RELEASE=-O2 -g' \
  -DICU_ROOT=/opt/homebrew/opt/icu4c@78 \
  -DMINIMA_NATIVE_H2_DIR="$PWD/cpp/build/native-h2"
cmake --build cpp/build-optimized -j4
ctest --test-dir cpp/build-optimized --output-on-failure -j4
```

The explicit configuration flags replace CMake's usual Release `-DNDEBUG`; source
assertions stay enabled. There is no fast-math flag. This build requires its own
behavioral validation. Do not relabel earlier Debug/sanitizer outcomes as a full
optimized-suite pass. The runtime manifest records the actual build configuration
and verifies the AOT database manifest against the library being bundled.


## Packaging

The packaging tools require a Git checkout with committed C++ changes, since they
record source identity from Git objects. In the working checkout:

```sh
python3 cpp/tools/package_project_delivery.py \
  --output cpp/deliverables/MinimaCoreCpp-new-checkpoint
```

It exports tracked project files and the pinned source, bundles the existing node
and native libraries, adds a manifest, and checks each payload read back from the
ZIP. It refuses an existing output and excludes untracked runtime data. The source
archive's lack of `.git` does not prevent compilation, but packaging a new source
revision requires creating or restoring a Git checkout first.


## Source snapshot integrity

Keep the exported `project/` directory beside `delivery-manifest.json`. The source
mapping check recognizes that layout without requiring Git metadata. It verifies
the pinned Java inventory and hashes, mapping hashes, every counterpart's presence
and the recorded partial-parity status. A Git checkout still verifies the actual
upstream commit and tracked source state. Historical reuse search leads are retained
as provenance; checking does not depend on the original author's sibling projects.

`check_full_port` deliberately fails while any mapped file lacks verified parity.
A passing `source_mapping` test establishes inventory consistency only. The snapshot
checker also has negative tests for altered, added and missing Java, altered maps,
missing native counterparts, wrong pins and missing manifests. Refreshing the source
inventory requires a pinned Git checkout and an explicit review of changes.
