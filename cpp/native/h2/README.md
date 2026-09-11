# Native H2 dependency

The Minima application remains C++. This optional provider compiles the pinned
H2 2.4.240 dependency and `NativeH2Bridge.java` into a native shared library.
It contains no Java Minima classes and launches no JVM. H2 retains its own
SQL implementation, MVStore file format, AES encryption and JDBC conversions.
The bridge translates calls from `support/SqlConnection.hpp`; it is not one of
the 457 mapped Minima files.

Build on the tested macOS ARM64 platform:

```sh
python3 cpp/tools/build_native_h2.py --graalvm /path/to/graalvm/Contents/Home
cmake -S cpp -B cpp/build -DMINIMA_NATIVE_H2_DIR="$PWD/cpp/build/native-h2"
cmake --build cpp/build -j 4
ctest --test-dir cpp/build -R '^(native_h2_differential|native_h2_concurrent|native_node_startup)$' --output-on-failure
```

The tested compiler is GraalVM CE 25.3.4.1+1.1, native-image 25.0.4.1.
The official macOS ARM64 archive's SHA-256 is
`ebfab1d74420f355a459076162012d6835fa6068bd9d2f230f1fcaf7ee0dd923`.
H2's SHA-256 is checked by the builder:
`b80baaf431ce458c926d72ec2b8cc33b120c76b73abe4301278bb2044ddde9f5`.
The builder records source/library hashes and the compiler command in its
output `manifest.json`. It requires a locally installed toolchain and performs
no downloads. Obtain the compiler from its
[official release](https://github.com/graalvm/graalvm-ce-builds/releases/tag/graal-25.3.4.1).

The resulting `MinimaCoreC++` executable supplies the connector through
`MainServices.sql`. Without `MINIMA_NATIVE_H2_DIR`, the launcher retains the
explicit missing-provider failure. Embedded users can construct NativeH2Runtime
and retain its connector; connections and children preserve isolate ownership.

Validation uses real JDBC and native storage, not recording drivers. See the
[student lab](../../student-guide/new-tests/native-h2-storage.md) and
[saved evidence](../../validation/native-h2-work-in-progress).
The 512-row concurrent test exercises native threads entering one H2 isolate.
The startup test uses disposable solo nodes and cross-opens their complete data
directories in Java and C++, including encrypted wallets and compact shutdown.

Limits: H2 describes native-image support as experimental. Arbitrary Java
extensions, dynamically loaded classes, public-network synchronization,
daemon/RPC node control, resource exhaustion and full Minima behavior are not
certified. The current macOS shared library has a build-directory install name;
rebuild it in a recovery checkout. Relocatable binary packaging and other
operating systems have not been validated. ASAN/UBSAN instruments the C++ adapter
and application, not the AOT-compiled H2 dependency. Preserve H2 and GraalVM's
applicable licenses when redistributing those dependencies.

References: [H2 native build documentation](https://h2database.com/html/build.html#native_image)
and [GraalVM shared-library interface](https://www.graalvm.org/dev/reference-manual/native-image/guides/build-native-shared-library/).

## Optional pinned MySQL driver

The same C ABI can include Connector/J 8.0.24. Its pinned jar and reviewed
[reachability metadata](mysql-metadata/reachability-metadata.json) are compiled
with H2; the native library still includes no Minima Java classes and no JVM
fallback. This extends the dependency provider, not the 457-file mapping.

```sh
python3 cpp/tools/build_native_h2.py --graalvm /path/to/graalvm/Contents/Home --mysql --output cpp/build/native-h2-mysql
cmake -S cpp -B cpp/build -DMINIMA_NATIVE_H2_DIR="$PWD/cpp/build/native-h2-mysql"
cmake --build cpp/build -j 4
```

The launcher supplies this isolate through both `MainServices.sql` and
`MainServices.mysql`. The latter preserves null credentials via
`NativeH2Runtime::nullableConnector()`. Build without `--mysql` for the H2-only
dependency; attempting a MySQL connection then reports a missing driver.

Live tests require a **disposable, empty** MySQL database: they create, populate
and drop fixture tables. Use the tested local image digest from
[the profile report](../../validation/native-mysql-work-in-progress/profile-results.json),
bind its port only to `127.0.0.1`, and use a tmpfs `/var/lib/mysql` with no host
volume. Do not point these tests at an existing archive or coin database.
The fixture JSON has `name` (beginning `minima-cpp-mysql-`), `image` (digest),
`address` (`127.0.0.1:PORT`), `database` (`minima_cpp`), `user`, and `password`.
The saved tests used synthetic credentials solely for that disposable container.

```sh
python3 cpp/tools/native_mysql_differential.py --build cpp/build --fixture /tmp/isolated-mysql.json
python3 cpp/tools/native_mysql_startup.py --build cpp/build --fixture /tmp/isolated-mysql.json
```

Run different build directories sequentially when sharing one MySQL fixture.
Optionally configure `-DMINIMA_MYSQL_FIXTURE_FILE=/tmp/isolated-mysql.json` to
register both in CTest; they run serially and startup depends on the driver test.
Remove the owned fixture container after testing, then clear this CMake option.

To reproduce metadata collection, use the same GraalVM installation for javac and
its tracing agent (the default system compiler may produce newer class files):

```sh
python3 cpp/tools/native_mysql_differential.py --build cpp/build --fixture /tmp/isolated-mysql.json --profile cpp/build/mysql-profile --graalvm /path/to/graalvm/Contents/Home
```

Review its JSON before replacing the checked-in metadata. This profile covers the
observed single-server driver paths, not arbitrary JDBC URL modes, plugins or
platforms. Preserve Connector/J's bundled `META-INF/LICENSE` when distributing
its compiled code. The student [MySQL lab](../../student-guide/new-tests/native-mysql-storage.md)
separates driver agreement from actual node-command validation.

## Standalone archive service

`minima_archive_server` translates the pinned ArchiveServer main and serves its
binary MiniData/IBD protocol. Build the optional provider with `--mysql` before
using it. Its explicit driver check uses the same compiled driver as connections.
The executable accepts `-mysqlhost`, `-mysqldb`, `-mysqluser`, `-mysqlpassword`,
`-port`, and `-help`, plus the source MYSQL_HOST/DB/USER/PASSWORD and ARCHIVE_PORT
environment variables. The source default port is 8888; console `quit` stops it.

The opt-in `archive_server_main_differential` test uses the same
MINIMA_MYSQL_FIXTURE_FILE setting and an isolated disposable database. It wipes
and seeds its fixture archive through pinned Java, then exercises both actual
servers. Ordinary `archive_server_differential` and `archive_server_clean_audit`
need no MySQL server. See the [student lab](../../student-guide/new-tests/archive-server.md)
and [recorded scope](../../validation/archive-server-work-in-progress/progress.json).
