package org.minima.system.network.minima;

import java.nio.channels.SocketChannel;

import org.minima.objects.Greeting;

/**
 * One port for the chain AND a Maxima relay: an embedding host (the Parlons Node) may register a
 * handler that takes over an INCOMING connection whose first message is a greeting carrying the
 * {@code "parlons"} key in its extra data - a Parlons/Maxima client attaching to the relay - the
 * moment that greeting has been read. The connection leaves this NIO layer (its selection key is
 * cancelled, the channel becomes blocking) and the handler owns the socket from then on. Every
 * other connection is a chain peer as before. No handler registered = nothing changes, which is
 * what a plain minima.jar does.
 */
public final class NIOHandoff {

    /** The extra-data key our own relay clients put in their greeting. */
    public static final String KEY = "parlons";

    public interface Handler {
        /**
         * @param zChannel   the connection, blocking mode, still open
         * @param zGreeting  the raw greeting frame body (type byte + payload) already consumed here
         * @param zLeftover  bytes the NIO reader had buffered after that frame (usually none)
         * @param zSourceIp  the peer's address
         */
        void take(SocketChannel zChannel, byte[] zGreeting, byte[] zLeftover, String zSourceIp);
    }

    public static volatile Handler HANDLER;

    private NIOHandoff() {
    }

    /** True when a handler is registered and this greeting asks for the relay. */
    public static boolean wants(Greeting zGreeting) {
        Handler h = HANDLER;
        return h != null && zGreeting != null && zGreeting.getExtraData() != null
                && zGreeting.getExtraData().containsKey(KEY);
    }
}
