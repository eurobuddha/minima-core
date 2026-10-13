package org.minima.system.commands.network;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Base64;

import org.minima.objects.base.MiniData;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandException;
import org.minima.system.network.keets.KeetsAPI;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;
import org.minima.utils.json.parser.JSONParser;

/**
 * MiniDapp / RPC gateway to the keets sidecar services ( media hosting +
 * pubkey messages - see keets/SERVICES.md ). The node holds the API token, so
 * dapps just call this command - no token hand-off, no CORS, no server to
 * configure. Inbound messages arrive as MDS event "KEETS" ( KeetsManager ).
 */
public class keets extends Command {

	public keets() {
		super("keets","[action:] - keets services : send/inbox/ack messages by pubkey, publish/fetch/pin media");
	}

	@Override
	public String getFullHelp() {
		return "\nkeets\n"
				+ "\n"
				+ "Keets services : pubkey-addressed messages ( the Maxima alternative ) and\n"
				+ "P2P media hosting - handled by the keets sidecar, no server required.\n"
				+ "Needs -keets enabled. Inbound messages are posted as MDS event KEETS\n"
				+ "( data : id, from, ts, payload base64, message utf8, via direct|mailbox ).\n"
				+ "\n"
				+ "action:\n"
				+ "    info : your keets pubkey ( your address ) and media drive key.\n"
				+ "    send : send a message to a keets pubkey. Delivered direct if the peer\n"
				+ "           is online, else sealed into a mailbox for their next start.\n"
				+ "    inbox : list undelivered inbox messages ( normally empty - the node\n"
				+ "            drains the inbox into KEETS events every 5 seconds ).\n"
				+ "    ack : remove a message from the inbox by id.\n"
				+ "    publish : host data P2P from this node - returns a keets:// url with\n"
				+ "              a pinned content hash. Anyone who fetches it re-seeds it.\n"
				+ "    fetch : fetch a keets:// url from the network, returns hex.\n"
				+ "    pin : permanently mirror a drive on this node.\n"
				+ "    pins : list pinned drives.\n"
				+ "    unpin : stop mirroring a drive.\n"
				+ "\n"
				+ "to: ( send )\n"
				+ "    The recipient's keets pubkey - 0x + 64 hex, from their 'keets action:info'.\n"
				+ "\n"
				+ "message: ( send ) UTF8 text to send. OR datahex: raw 0x hex ( max 256KB ).\n"
				+ "\n"
				+ "path: ( publish )  The published path, e.g. /nft/art.png\n"
				+ "datahex: ( publish ) The content as 0x hex.\n"
				+ "\n"
				+ "url: ( fetch / pin )  A keets://<drivekey>/<path>?hash=<sha256> url.\n"
				+ "file: ( publish / fetch )  Local file path for the bytes - avoids huge\n"
				+ "    datahex params. TRUST NOTE: the path is not sandboxed; anything that\n"
				+ "    can reach this RPC can already spend, but file: additionally reads and\n"
				+ "    writes as the node user. Keep RPC loopback-only / authenticated.\n"
				+ "drive: ( unpin )  The drive key.\n"
				+ "id: ( ack )  The inbox message id.\n"
				+ "\n"
				+ "Examples:\n"
				+ "\n"
				+ "keets action:info\n"
				+ "\n"
				+ "keets action:send to:0xF0B090272C36908E3EAD8E3654AD0FE56EE09CEE12150195460912465C900957 message:\"hello\"\n"
				+ "\n"
				+ "keets action:publish path:/nft/art.png datahex:0x89504E47..\n"
				+ "\n"
				+ "keets action:fetch url:keets://0bf08db..38/nft/art.png?hash=9aef8c..51\n";
	}

	@Override
	public ArrayList<String> getValidParams(){
		return new ArrayList<>(Arrays.asList(new String[]{
				"action","to","message","datahex","path","url","drive","id","file"}));
	}

	@Override
	public JSONObject runCommand() throws Exception {
		JSONObject ret = getJSONReply();

		if(!GeneralParams.KEETS_ENABLED) {
			throw new CommandException("keets is not enabled - start with -keets");
		}

		String action = getParam("action","info");

		if(action.equals("info")) {
			ret.put("response", parse(KeetsAPI.getStr("/info")));

		}else if(action.equals("send")) {
			String to = getParam("to");

			byte[] payload;
			if(existsParam("datahex")) {
				payload = new MiniData(getParam("datahex")).getBytes();
			}else {
				payload = getParam("message").getBytes("UTF-8");
			}

			JSONObject body = new JSONObject();
			body.put("to", to);
			body.put("payloadb64", Base64.getEncoder().encodeToString(payload));
			ret.put("response", parse(KeetsAPI.postStr("/message", body.toString().getBytes("UTF-8"))));

		}else if(action.equals("inbox")) {
			JSONArray msgs = (JSONArray) new JSONParser().parse(KeetsAPI.getStr("/messages"));
			for(Object mo : msgs) {
				JSONObject msg = (JSONObject) mo;
				try {
					msg.put("message", new String(Base64.getDecoder().decode((String)msg.get("payload")), "UTF-8"));
				}catch(Exception exc) {
					msg.put("message", "");
				}
			}
			ret.put("response", msgs);

		}else if(action.equals("ack")) {
			ret.put("response", parse(KeetsAPI.deleteStr("/message/"+getParam("id"))));

		}else if(action.equals("publish")) {
			String path = getParam("path");
			if(!path.startsWith("/")) {
				path = "/"+path;
			}
			//file: avoids megabyte command strings (the tokenizer drops oversized params)
			byte[] data;
			if(existsParam("file")) {
				data = org.minima.utils.MiniFile.readCompleteFile(new java.io.File(getParam("file")));
			}else {
				data = new MiniData(getParam("datahex")).getBytes();
			}
			ret.put("response", parse(KeetsAPI.postStr("/media"+path, data)));

		}else if(action.equals("fetch")) {
			String url = getParam("url");
			byte[] data = KeetsAPI.call("GET", "/media?url="+java.net.URLEncoder.encode(url, "UTF-8"), null);
			JSONObject resp = new JSONObject();
			resp.put("url", url);
			resp.put("size", data.length);
			if(existsParam("file")) {
				//write to disk instead of returning megabytes of hex through the parser
				java.io.File out = new java.io.File(getParam("file"));
				org.minima.utils.MiniFile.writeDataToFile(out, data);
				resp.put("file", out.getAbsolutePath());
			}else {
				resp.put("datahex", new MiniData(data).to0xString());
			}
			ret.put("response", resp);

		}else if(action.equals("pin")) {
			JSONObject body = new JSONObject();
			body.put("url", getParam("url"));
			ret.put("response", parse(KeetsAPI.postStr("/pin", body.toString().getBytes("UTF-8"))));

		}else if(action.equals("pins")) {
			ret.put("response", new JSONParser().parse(KeetsAPI.getStr("/pins")));

		}else if(action.equals("unpin")) {
			ret.put("response", parse(KeetsAPI.deleteStr("/pin/"+getParam("drive"))));

		}else {
			throw new CommandException("Unknown action : "+action);
		}

		return ret;
	}

	private JSONObject parse(String zJson) throws Exception {
		return (JSONObject) new JSONParser().parse(zJson);
	}

	@Override
	public Command getFunction() {
		return new keets();
	}
}
