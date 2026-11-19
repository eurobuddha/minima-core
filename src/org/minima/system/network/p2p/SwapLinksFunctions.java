package org.minima.system.network.p2p;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;

import org.minima.objects.base.MiniData;
import org.minima.system.Main;
import org.minima.system.network.minima.NIOClient;
import org.minima.system.network.minima.NIOClientInfo;
import org.minima.system.network.p2p.messages.P2PGreeting;
import org.minima.system.network.p2p.messages.P2PWalkLinks;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONObject;
import org.minima.utils.messages.Message;

public class SwapLinksFunctions {

    public static JSONObject wrapP2PMsg(JSONObject data) {
        JSONObject msg = new JSONObject();
        msg.put("swap_links_p2p", data);
        return msg;
    }

    /**
     * Adds connections to the NoneP2PLinks set whilst waiting for a valid greeting.
     * Generates and send a greeting message to the newly connected client
     * Sends a request for its IP if the node's minima address has not been set yet
     *
     * @param state    the current P2P State
     * @param incoming if the connection is an incoming one
     * @param info     NIOClientInfo for the client that just connected
     * @return a list of messages to be sent (greeting and possible a request IP message)
     */
    public static List<Message> onConnected(P2PState state,boolean incoming, NIOClient info) {
        List<Message> msgs = new ArrayList<>();
        //Get the details
        String uid = info.getUID();
        boolean sendMessages = true;
        
        if (incoming) {
            InetSocketAddress incomingAddress = new InetSocketAddress(info.getHost(), 0);
            if (state.getNoneP2PLinks().containsValue(incomingAddress)) {
                msgs.add(new Message(P2PManager.P2P_SEND_DISCONNECT).addString("uid", uid));
                sendMessages = false;
            }
            state.getNoneP2PLinks().put(uid, new InetSocketAddress(info.getHost(), 0));
        } else {
            state.getNoneP2PLinks().put(uid, new InetSocketAddress(info.getHost(), info.getPort()));
        }

        if (sendMessages) {
            P2PGreeting greeting = new P2PGreeting(state);
            msgs.add(new Message(P2PManager.P2P_SEND_MSG).addString("uid", uid).addObject("json", greeting.toJson()));

            // Ask every peer we greet what address they see us from unless the operator pinned
            // -host: a home node's public address changes (dynamic IP, a VPN coming or going) and a
            // one-time answer at boot left the node advertising a dead address until restart.
            if (!state.isHostSet() || !GeneralParams.IS_HOST_SET) {
                JSONObject requestIp = new JSONObject();
                MiniData secret = MiniData.getRandomData(12);
                state.setIpReqSecret(secret);
                requestIp.put("req_ip", secret.toString());
                msgs.add(new Message(P2PManager.P2P_SEND_MSG).addString("uid", uid).addObject("json", requestIp));
            }
        }
        return msgs;
    }

    /**
     * If the node has more noneP2PNodes that its desired max then
     * Start a Random Walk to find a node that can take excess clients
     *
     * @param state   P2P State
     * @param clients all NIOClients
     * @return list of load balance walk messages to send
     */
    public static List<Message> onConnectedLoadBalanceRequest(P2PState state, List<NIOClientInfo> clients) {
        List<Message> msgs = new ArrayList<>();


        if (state.isAcceptingInLinks() && state.getMyMinimaAddress() != null && state.getNotAcceptingConnP2PLinks().size() > state.getMaxNumNoneP2PConnections() && !state.getInLinks().isEmpty()) {
            InetSocketAddress nextHop = UtilFuncs.selectRandomAddress(new ArrayList<>(state.getInLinks().values()));
            NIOClientInfo minimaClient = UtilFuncs.getClientFromInetAddress(nextHop, state);
            P2PWalkLinks walkLinks = new P2PWalkLinks(true, false, minimaClient.getUID());
            walkLinks.setClientWalk(true);
            // Send out multiple load balance request messages if node is highly overloaded
            int multipleOverMax = state.getInLinks().size() / state.getMaxNumP2PConnections();
            for (int i=0; i < multipleOverMax; i++) {
                msgs.add(new Message(P2PManager.P2P_SEND_MSG).addString("uid", minimaClient.getUID()).addObject("json", walkLinks.toJson()));
            }
        }
        return msgs;
    }

    public static void onDisconnected(P2PState state, Message zMessage) {
        //Get the details
        String uid = zMessage.getString("uid");

        // Remove uid from current connections
        state.getInLinks().remove(uid);
        state.getNotAcceptingConnP2PLinks().remove(uid);
        state.getOutLinks().remove(uid);
        state.getNoneP2PLinks().remove(uid);
    }

    public static boolean processGreeting(P2PState state, P2PGreeting greeting, NIOClientInfo client, boolean noconnect) {

        if (client != null) {
            String uid = client.getUID();
            String host = client.getHost();
            int port = greeting.getMyMinimaPort();
            InetSocketAddress minimaAddress = new InetSocketAddress(host, port);

            boolean addtoknown = !host.contains("127.0.0.1");
//            P2PFunctions.log_debug("P2P GREETING UID:"+uid+" valid:"+state.getNoneP2PLinks().containsKey(uid)+" @ "+minimaAddress+" addtoknown:"+addtoknown);
            state.getNoneP2PLinks().remove(uid);

            //The NIOClient has received a P2Pgreeting.. Check if NULL or Tests fail
            if(Main.getInstance() != null) {
	            NIOClient nioclient = Main.getInstance().getNIOManager().getNIOServer().getClient(uid);
                if (nioclient != null){
	                nioclient.setReceivedP2PGreeting();
                }
            }

            if (greeting.isAcceptingInLinks()) {

                // Peers are assumed to not be P2P Links until we get a valid P2P Greeting

                if (client.isIncoming()) {
                    state.getInLinks().put(uid, minimaAddress);
                } else {
                    state.getOutLinks().put(uid, minimaAddress);
                    if (state.getOutLinks().size() > state.getMaxNumP2PConnections()) {
                        P2PFunctions.disconnect(uid);
                    }
                }

                // Disable no connect once we get a p2p connection
                if (noconnect) {
                    noconnect = false;
                }

                if (state.isDoingDiscoveryConnection()) {
                    state.setDoingDiscoveryConnection(false);
                    P2PFunctions.disconnect(uid);
                }
            } else {
                state.getNotAcceptingConnP2PLinks().put(uid, minimaAddress);
            }
        } else {
            P2PFunctions.log_debug("[-] ERROR Client is null when processing greeting: " + greeting.toJson());
        }
        return noconnect;
    }

    public static JSONObject processRequestIPMsg(JSONObject swapLinksMsg, String host) {
        MiniData secret = new MiniData((String) swapLinksMsg.get("req_ip"));
        JSONObject responseMsg = new JSONObject();
        JSONObject IpResponse = new JSONObject();
        IpResponse.put("res_ip", host);
        IpResponse.put("secret", secret.toString());
        responseMsg.put("swap_links_p2p", IpResponse);
        return responseMsg;
    }

    public static void processResponseIPMsg(P2PState state, JSONObject swapLinksMsg) {
        MiniData secret = new MiniData((String) swapLinksMsg.get("secret"));
        if (state.getIpReqSecret().isEqual(secret)) {
            String hostIP = (String) swapLinksMsg.get("res_ip");
            String current = state.getMyMinimaAddress() == null ? "" : state.getMyMinimaAddress().getHostString();
            if (state.isHostSet()) {
                // A later answer only REPLACES a learned address when it is a different public one:
                // a peer on our own LAN answers with our private address and must not demote us.
                if (hostIP == null || hostIP.equals(current) || !isPublicIp(hostIP)) {
                    return;
                }
                P2PFunctions.log_info("[+] My IP changed: " + current + " -> " + hostIP);
            }
            state.setMyMinimaAddress(hostIP);
            state.setHostSet(true);
            state.getKnownPeers().remove(state.getMyMinimaAddress());
            P2PFunctions.log_debug("[+] Setting My IP: " + hostIP);

        }

    }

    /** A literal IP that is routable from the internet (not loopback, link-local, RFC 1918, CGNAT). */
    static boolean isPublicIp(String zHost) {
        try {
            java.net.InetAddress a = java.net.InetAddress.getByName(zHost);   // literal: no lookup
            if (a.isLoopbackAddress() || a.isLinkLocalAddress() || a.isSiteLocalAddress() || a.isAnyLocalAddress()) {
                return false;
            }
            byte[] b = a.getAddress();
            return !(b.length == 4 && (b[0] & 0xFF) == 100 && (b[1] & 0xC0) == 64);
        } catch (Exception e) {
            return false;
        }
    }

    public static List<Message> joinScaleOutLinks(P2PState state, int targetNumLinks, ArrayList<NIOClientInfo> clients) {
        List<Message> sendMsgs = new ArrayList<>();
        if (state.getOutLinks().size() < targetNumLinks) {
            P2PFunctions.log_debug("Attempting to scale outlinks");
            InetSocketAddress nextHop = UtilFuncs.selectRandomAddress(new ArrayList<>(state.getOutLinks().values()));
            NIOClientInfo minimaClient = UtilFuncs.getClientFromInetAddress(nextHop, state);
            if (minimaClient != null) {
                P2PWalkLinks walkLinksMsg = new P2PWalkLinks(true, true, minimaClient.getUID());
                sendMsgs.add(new Message(P2PManager.P2P_SEND_MSG).addString("uid", minimaClient.getUID()).addObject("json", walkLinksMsg.toJson()));
            }
        }
        return sendMsgs;
    }

    public static List<Message> requestInLinks(P2PState state, int targetNumLinks, ArrayList<NIOClientInfo> clients) {
        List<Message> sendMsgs = new ArrayList<>();
        if (state.isAcceptingInLinks() && state.getInLinks().size() < targetNumLinks) {
            P2PFunctions.log_debug("Attempting to scale inlinks");
            InetSocketAddress nextHop = UtilFuncs.selectRandomAddress(new ArrayList<>(state.getOutLinks().values()));
            NIOClientInfo minimaClient = UtilFuncs.getClientFromInetAddress(nextHop, state);
            if (minimaClient != null) {
                P2PWalkLinks walkLinksMsg = new P2PWalkLinks(false, false, minimaClient.getUID());
                sendMsgs.add(new Message(P2PManager.P2P_SEND_MSG).addString("uid", minimaClient.getUID()).addObject("json", walkLinksMsg.toJson()));
            }
        }
        return sendMsgs;
    }

}
