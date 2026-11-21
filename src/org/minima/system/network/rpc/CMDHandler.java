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
			while(input != null && !input.trim().equals("")) {
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
			
			//Is it a POST request
			if(method.equals("POST")) {
				
				//SECURITY: Content-Length is the client's number - new byte[n] for a huge
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
				if(fileRequested.equals("quit")) {
					quit=true;
				}
				
				//MinimaLogger.log("RPC:"+fileRequested+" User:"+authuser.toString());
				
				//Now run this command
				JSONArray res = CommandRunner.getRunner().runMultiCommand(fileRequested);;
				
				/*//MDS STUFF
				JSONArray res = null;
				if(authuser.getString("mode").equals("read")) {
					res = CommandRunner.getRunner().runMultiCommand(Main.getInstance().getMDSManager().getUntrustedMiniDAPPID(), fileRequested);
				}else if(authuser.getString("mode").equals("write")) {
					res = CommandRunner.getRunner().runMultiCommand(fileRequested);
				}else {
					throw new IllegalArgumentException("Invalid mode for RPC user : "+authuser.getString("mode"));
				}*/
				
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
