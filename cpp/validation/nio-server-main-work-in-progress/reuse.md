# Reuse evidence

Read the pinned NIOServer.java, including its full main and server run loop;
read native NIOServer.hpp/.cpp, MessageProcessor.hpp/.cpp and the source finally
cleanup path, MinimaDB getDB/cleanup declarations, NativeShutdownHooks.cpp and its
existing fixture. Read existing nio_server.cpp, nio_server_differential.py and
nio_server_errors_differential.py; existing NativeThread and NIOManager connector
thread ownership supply the new demo worker boundary. Minima.mainStarter confirms
std::exit use for existing native shutdown hooks.

Searched/read the relevant sibling at
/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/system/MessageProcessor.hpp
and inspected network/NIOClient.hpp references. It has a simpler queue with different
exception/ownership behavior and no matching standalone server demo. The current
project's server, message worker, NativeThread and native shutdown hook implementation
are reused directly. The sole new production behavior is the omitted main and its
small anonymous-subclass equivalent. No replacement network stack or database setup.

A compiled javap check found 24 further live Java public static main(String[]) methods
whose mapped native body/header contain no main token. The saved list is explicitly
an omission-candidate inventory; alternative adapters still require source review.
