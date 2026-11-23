package org.minima.system.network.rpc;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.URLDecoder;
import java.util.Date;
import java.util.StringTokenizer;

import org.minima.database.MinimaDB;
import org.minima.objects.base.MiniString;
import org.minima.system.commands.CommandRunner;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MinimaLogger;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * This class handles a single request then exits
 * 
 * @author spartacusrex
 *
 */
public class CMDHandler implements Runnable {

	/**
	 * The Net Socket
	 */
	Socket mSocket;
	
	/**
	 * The largest POST body accepted (bytes)
	 */
	static final int MAX_CONTENT_LENGTH = 32 * 1024 * 1024;
	
	/**
	 * Main Constructor
	 * @param zSocket
	 */
	public CMDHandler(Socket zSocket) {
		//Store..
		mSocket = zSocket;
	}

	@Override
	public void run() {
		// we manage our particular client connection
		InputStream in 	 		 		= null; 
		PrintWriter out 	 			= null; 
		String firstline 				= "no first line..";
		boolean quit 					= false;
		
		try {
			// Input Stream
			in = new BufferedInputStream(mSocket.getInputStream());
			
			// Output Stream
			out = new PrintWriter(new OutputStreamWriter(mSocket.getOutputStream(), MiniString.MINIMA_CHARSET));
			
			// get first line of the request from the client
			String input = readLine(in);
			if (input == null){
				input = "";
			}
			
			//Get the first line..
			firstline = new String(input);
			
			// we parse the request with a string tokenizer
			StringTokenizer parse = new StringTokenizer(input);
			String method = parse.nextToken().toUpperCase(); // we get the HTTP method of the client
			
			// we get file requested
			String fileRequested = parse.nextToken();
			
			//Remove slashes..
			if(fileRequested.startsWith("/")) {
				fileRequested = fileRequested.substring(1);
			}
			if(fileRequested.endsWith("/")) {
				fileRequested = fileRequested.substring(0,fileRequested.length()-1);
			}
			
			//And finally URL decode..
			fileRequested = URLDecoder.decode(fileRequested,"UTF-8").trim();
			
			//Are we Authorizing
			JSONObject authuser = new JSONObject();
			authuser.put("valid", true);
			authuser.put("mode", "write");
			
			//MUST have correct details..
			if(GeneralParams.RPC_AUTHENTICATE || MinimaDB.getDB().getUserDB().getRPCUsers().size()>0) {
				authuser.put("valid", false);
			}
			
			//Get the Headers..
			int contentlength = 0;
			String hostheader = null;
			String originheader = null;
			String fetchsiteheader = null;
			String contenttypeheader = null;
			boolean preflighted = false;
			while(input != null && !input.trim().equals("")) {
				
				if(input.toLowerCase().startsWith("host:")) {
					hostheader = input.substring(5).trim();
				}else if(input.toLowerCase().startsWith("origin:")) {
					originheader = input.substring(7).trim();
				}else if(input.toLowerCase().startsWith("sec-fetch-site:")) {
					fetchsiteheader = input.substring(15).trim().toLowerCase();
				}else if(input.toLowerCase().startsWith("content-type:")) {
					contenttypeheader = input.substring(13).trim();
				}
				if(input.toLowerCase().startsWith("x-minima-rpc:") || input.toLowerCase().startsWith("authorization:")) {
					//headers a web page can only send after a CORS preflight
					preflighted = true;
				}
				//MinimaLogger.log("RPC HEADER : "+input);
				
				//Check if Authorised
				int authref = input.indexOf("Authorization:");
				if(authref != -1) {
					
					//Check it..
					authuser = Authorizer.checkAuchCredentials(input);
				}
				
				int ref = input.indexOf("Content-Length:"); 
				if(ref != -1) {
					//Get it..
					int start     = input.indexOf(":");
					contentlength = Integer.parseInt(input.substring(start+1).trim());
				}	
				input = readLine(in);
			}
			
			//SECURITY (review 2026-11-21): DNS rebinding guard - a browser page that rebinds its own name to
			//127.0.0.1 sends ITS name as Host. Only this machine's names / IP literals / named hosts are served.
			if(!hostAllowed(hostheader)) {
				out.println("HTTP/1.1 403 Forbidden");
				out.println("Server: HTTP RPC Server from Minima 1.3");
				out.println();
				out.println("{\"status\":false,\"error\":\"Host "+hostheader.replace("\"","")
						+" is not this node - name it with -rpcallowhost\"}");
				out.flush();
				return;
			}
			
			//SECURITY (review 2026-11-21): on a node with NO password every request is served - and every reply
			//carries Access-Control-Allow-Origin: *, so any web page could run a command AND read the answer
			//(vault..). Then a browser request is served only from the RPC's own origin or an origin named with
			//-rpcalloworigin (Origin / Sec-Fetch-Site, as the Parlons AdminRpc does). Non-browser clients send
			//neither header. With a password set nothing changes: a page cannot authenticate.
			boolean authrequired = GeneralParams.RPC_AUTHENTICATE || MinimaDB.getDB().getUserDB().getRPCUsers().size()>0;
			boolean browserok	 = authrequired || browserAllowed(originheader, fetchsiteheader, hostheader);
			
			//A preflight never runs a command: refused like the request it announces, else answered
			if(method.equals("OPTIONS")) {
				out.println(browserok ? "HTTP/1.1 204 No Content" : "HTTP/1.1 403 Forbidden");
				out.println("Server: HTTP RPC Server from Minima 1.3");
				if(browserok) {
					out.println("Access-Control-Allow-Origin: *");
					out.println("Access-Control-Allow-Methods: GET, POST");
					out.println("Access-Control-Allow-Headers: Authorization, Content-Type, X-Minima-Rpc");
				}
				out.println();
				out.flush();
				return;
			}
			
			if(!method.equals("GET") && !method.equals("POST")) {
				out.println("HTTP/1.1 405 Method Not Allowed");
				out.println("Server: HTTP RPC Server from Minima 1.3");
				out.println();
				out.println("{\"status\":false,\"error\":\"GET or POST only\"}");
				out.flush();
				return;
			}
			
			if(!browserok) {
				out.println("HTTP/1.1 403 Forbidden");
				out.println("Server: HTTP RPC Server from Minima 1.3");
				out.println();
				out.println("{\"status\":false,\"error\":\"Browser request refused - this RPC has no password; "
						+ "allow a page's origin with -rpcalloworigin\"}");
				out.flush();
				return;
			}
			
			//Are we Authorised
			if(!authuser.getBoolean("valid")) {
				
				//Not allowed..
				out.println("HTTP/1.1 401 Unauthorized");
				out.println("Server: HTTP RPC Server from Minima 1.3");
				out.println();
				out.println("{\"error\":\"Authentication failure..\"}");
				out.flush(); // flush character output stream bufferout.flush(); // flush character output stream buffer
				
				throw new IllegalArgumentException("Invalid Authentication at RPC");
			}
			
			//SECURITY (2026-11-23): browserAllowed cannot see a request with neither Origin nor Sec-Fetch-Site - an <img>,
			//a form, any page in a browser that sends no Fetch-Metadata. On an RPC with NO password such a GET or simple
			//POST now runs as a read-mode user (reads only, WRITE commands refused); ADMIN needs a request no page can
			//send without a preflight - see adminRequest. With a password nothing changes.
			boolean readonly = !authrequired && !adminRequest(method, contenttypeheader, preflighted);
			if(readonly) {
				authuser.put("mode", "read");
			}
			
			//Is it a POST request
			if(method.equals("POST")) {
				
				//SECURITY (review 2026-11-21): Content-Length is the client's number - new byte[n] for a huge
				//one is an OutOfMemoryError, which catch(Exception) does not catch
				if(contentlength > MAX_CONTENT_LENGTH) {
					out.println("HTTP/1.1 413 Payload Too Large");
					out.println("Server: HTTP RPC Server from Minima 1.3");
					out.println();
					out.println("{\"status\":false,\"error\":\"Request body too large (max "+MAX_CONTENT_LENGTH+" bytes)\"}");
					out.flush();
					return;
				}
				
				//Get the POST data..
				fileRequested = readBody(in, contentlength);
			}
			
			JSONObject statfalse = new JSONObject();
			statfalse.put("status", false);
			String result = statfalse.toJSONString();
			try {

				//MinimaLogger.log("RPC:"+fileRequested+" User:"+authuser.toString());
				
				//Now run this command - SECURITY (2026-11-21): the read / write mode check was commented out, so a
				//'read' RPC user ran every command as ADMIN. A read user now runs as the non-admin id (0xFF, as a
				//non-admin companion does), so the WRITE gate in CommandRunner applies. minima / write: unchanged.
				JSONArray res = null;
				String mode = authuser.getString("mode");
				if(mode.equals("read")) {
					res = CommandRunner.getRunner().runMultiCommand("0xFF", fileRequested);
				}else if(mode.equals("write")) {
					//Only a command that really runs as ADMIN may stop the JVM (a read user's quit is refused above)
					if(fileRequested.equals("quit")) {
						quit=true;
					}
					res = CommandRunner.getRunner().runMultiCommand(fileRequested);
				}else {
					throw new IllegalArgumentException("Invalid mode for RPC user : "+mode);
				}
				
				//Say how to run it as ADMIN
				if(readonly) {
					for(Object obj : res) {
						JSONObject reply = (JSONObject) obj;
						if(String.valueOf(reply.get("error")).contains("ADMIN")) {
							reply.put("error", reply.get("error")+" - this RPC has no password: send the command with the"
									+ " header X-Minima-Rpc: 1 (or set -rpcpassword)");
						}
					}
				}
				
				//Get the result.. is it a multi command or single.. 
				if(res.size() == 1) {
					result = res.get(0).toString();
				}else {
					result = res.toString();
				}
				
//				MinimaLogger.log("RPC RESULT : "+result);
				
			}catch(Exception exc) {
				MinimaLogger.log("ERROR CMDHANDLER : "+fileRequested+" "+exc);
				
			}
			
			//Calculate the size of the response
			int finallength = result.getBytes(MiniString.MINIMA_CHARSET).length; 
			
			//Are we using CRLF
			if(GeneralParams.RPC_CRLF) {
				
				// send HTTP Headers
				out.println("HTTP/1.1 200 OK\r");
				out.println("Server: HTTP RPC Server from Minima 1.3\r");
				out.println("Date: " + new Date()+"\r");
				out.println("Content-type: text/plain\r");
				out.println("Content-length: " + finallength+"\r");
				out.println("Access-Control-Allow-Origin: *\r");
				out.println("\r"); // blank line between headers and content, very important !
				out.println(result);
				out.flush(); // flush character output stream buffer
				
			}else {
				
				// send HTTP Headers
				out.println("HTTP/1.1 200 OK");
				out.println("Server: HTTP RPC Server from Minima 1.3");
				out.println("Date: " + new Date());
				out.println("Content-type: text/plain");
				out.println("Content-length: " + finallength);
				out.println("Access-Control-Allow-Origin: *");
				out.println(); // blank line between headers and content, very important !
				out.println(result);
				out.flush(); // flush character output stream buffer
			}
			
		} catch (Exception ioe) {
			MinimaLogger.log("CMDHANDLER : "+ioe+" "+firstline);
			//MinimaLogger.log(ioe);
			
		} finally {
			try {
				in.close();
				out.close();
				mSocket.close(); // we close socket connection
			} catch (Exception e) {
				MinimaLogger.log(e);
			} 	
		}	
		
		//Are we shutting down
		if(quit) {
			Runtime.getRuntime().halt(0);
		}
	}
	
	/**
	 * May this browser request run on an RPC with no password? Origin must be the RPC's own origin or one named
	 * with -rpcalloworigin; a Sec-Fetch-Site other than none / same-origin needs a named Origin. No Origin and no
	 * Sec-Fetch-Site (curl, scripts, the desktops' Node client): served.
	 */
	static boolean browserAllowed(String zOrigin, String zFetchSite, String zHost) {
		boolean named = false;
		if(zOrigin != null) {
			for(String allowed : GeneralParams.RPC_ALLOWED_ORIGINS.split(",")) {
				if(!allowed.trim().isEmpty() && zOrigin.equals(allowed.trim())) {
					named = true;
				}
			}
			boolean own = zHost != null && zOrigin.equalsIgnoreCase("http://"+zHost);
			if(!named && !own) {
				return false;
			}
		}
		if(zFetchSite != null && !zFetchSite.equals("none") && !zFetchSite.equals("same-origin") && !named) {
			return false;
		}
		return true;
	}
	
	/**
	 * On an RPC with no password, may this request run as ADMIN? Only one a web page cannot send without a CORS
	 * preflight (which browserAllowed refuses for a foreign page): one with X-Minima-Rpc or Authorization (GET or
	 * POST), or a POST with a Content-Type other than the three a form / simple fetch can send. A plain GET, or a POST
	 * with no Content-Type or text/plain, application/x-www-form-urlencoded or multipart/form-data, runs read-only.
	 */
	static boolean adminRequest(String zMethod, String zContentType, boolean zPreflightHeader) {
		if(zPreflightHeader) {
			return true;
		}
		if(!zMethod.equals("POST")) {
			return false;
		}
		if(zContentType == null) {
			return false;
		}
		String type = zContentType.split(";")[0].trim().toLowerCase();
		return !type.isEmpty() && !type.equals("text/plain") && !type.equals("application/x-www-form-urlencoded")
				&& !type.equals("multipart/form-data");
	}
	
	/**
	 * May a request with this Host header be served? No Host (a non-browser client), localhost, an IP literal
	 * (a rebinding page always sends a NAME), the -rpcbind address, or a host named with -rpcallowhost.
	 */
	static boolean hostAllowed(String zHost) {
		if(zHost == null || zHost.isEmpty()) {
			return true;
		}
		String host = zHost.trim().toLowerCase();
		if(host.startsWith("[")) {
			//[v6]:port - an IPv6 literal
			return host.indexOf(']') > 0;
		}
		int colon = host.lastIndexOf(':');
		if(colon != -1) {
			host = host.substring(0, colon);
		}
		if(host.equals("localhost") || host.matches("[0-9]{1,3}(\\.[0-9]{1,3}){3}")
				|| host.equalsIgnoreCase(GeneralParams.RPC_BIND_HOST)) {
			return true;
		}
		for(String allowed : GeneralParams.RPC_ALLOWED_HOSTS.split(",")) {
			if(!allowed.trim().isEmpty() && host.equals(allowed.trim().toLowerCase())) {
				return true;
			}
		}
		return false;
	}
	
	/**
	 * One request line - up to LF, a trailing CR dropped - or null at the end of the stream.
	 * Read as BYTES so the body that follows is still unread.
	 */
	static String readLine(InputStream zIn) throws IOException {
		ByteArrayOutputStream line = new ByteArrayOutputStream();
		int b;
		while((b = zIn.read()) != -1 && b != '\n') {
			line.write(b);
		}
		if(b == -1 && line.size() == 0) {
			return null;
		}
		String str = new String(line.toByteArray(), MiniString.MINIMA_CHARSET);
		return str.endsWith("\r") ? str.substring(0, str.length()-1) : str;
	}
	
	/**
	 * The POST body. Content-Length counts BYTES: reading that many CHARACTERS waited forever for a body with
	 * any non-ASCII text in it (fewer characters than bytes ever arrive). Read the bytes, then decode UTF-8.
	 */
	static String readBody(InputStream zIn, int zContentLength) throws IOException {
		byte[] body = new byte[zContentLength];
		int total = 0;
		while(total < zContentLength) {
			int len = zIn.read(body, total, zContentLength-total);
			if(len == -1) {
				break;
			}
			total += len;
		}
		
		if(total != zContentLength) {
			MinimaLogger.log("CMDHANDLER : Read wrong amount "+total+"/"+zContentLength);	
		}
		
		return new String(body, 0, total, MiniString.MINIMA_CHARSET);
	}
}
