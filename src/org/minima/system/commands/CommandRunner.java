package org.minima.system.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.minima.system.commands.backup.archive;
import org.minima.system.commands.backup.backup;
import org.minima.system.commands.backup.decryptbackup;
import org.minima.system.commands.backup.mysql;
import org.minima.system.commands.backup.mysqlcoins;
import org.minima.system.commands.backup.rescue;
import org.minima.system.commands.backup.reset;
import org.minima.system.commands.backup.restore;
import org.minima.system.commands.backup.restoresync;
import org.minima.system.commands.backup.vault;
import org.minima.system.commands.backup.mmrsync.megammr;
import org.minima.system.commands.backup.mmrsync.megammrsync;
import org.minima.system.commands.base.automine;
import org.minima.system.commands.base.balance;
import org.minima.system.commands.base.benchmark;
import org.minima.system.commands.base.block;
import org.minima.system.commands.base.burn;
import org.minima.system.commands.base.checkaddress;
import org.minima.system.commands.base.coincheck;
import org.minima.system.commands.base.coinexport;
import org.minima.system.commands.base.coinimport;
import org.minima.system.commands.base.coinnotify;
import org.minima.system.commands.base.cointrack;
import org.minima.system.commands.base.consolidate;
import org.minima.system.commands.base.convert;
import org.minima.system.commands.base.debugflag;
import org.minima.system.commands.base.getaddress;
import org.minima.system.commands.base.hash;
import org.minima.system.commands.base.hashtest;
import org.minima.system.commands.base.healthcheck;
import org.minima.system.commands.base.incentivecash;
import org.minima.system.commands.base.jniminingtest;
import org.minima.system.commands.base.jnitest;
import org.minima.system.commands.base.logs;
import org.minima.system.commands.base.maths;
import org.minima.system.commands.base.mempool;
import org.minima.system.commands.base.missingcmd;
import org.minima.system.commands.base.mmrcreate;
import org.minima.system.commands.base.mmrproof;
import org.minima.system.commands.base.newaddress;
import org.minima.system.commands.base.printmmr;
import org.minima.system.commands.base.printtree;
import org.minima.system.commands.base.quit;
import org.minima.system.commands.base.random;
import org.minima.system.commands.base.scanchain;
import org.minima.system.commands.base.seedrandom;
import org.minima.system.commands.base.slavenode;
import org.minima.system.commands.base.sphincs;
import org.minima.system.commands.base.status;
import org.minima.system.commands.base.systemcheck;
import org.minima.system.commands.base.test;
import org.minima.system.commands.base.timemilli;
import org.minima.system.commands.base.tokencreate;
import org.minima.system.commands.base.tokenvalidate;
import org.minima.system.commands.base.trace;
import org.minima.system.commands.network.connect;
import org.minima.system.commands.network.disconnect;
import org.minima.system.commands.network.message;
import org.minima.system.commands.network.network;
import org.minima.system.commands.network.p2pstate;
import org.minima.system.commands.network.peers;
import org.minima.system.commands.network.ping;
import org.minima.system.commands.network.rpc;
import org.minima.system.commands.network.webhooks;
import org.minima.system.commands.scripts.newscript;
import org.minima.system.commands.scripts.removescript;
import org.minima.system.commands.scripts.runscript;
import org.minima.system.commands.scripts.scripts;
import org.minima.system.commands.search.coins;
import org.minima.system.commands.search.history;
import org.minima.system.commands.search.keys;
import org.minima.system.commands.search.tokens;
import org.minima.system.commands.search.txpow;
import org.minima.system.commands.send.multisig;
import org.minima.system.commands.send.multisigread;
import org.minima.system.commands.send.send;
import org.minima.system.commands.send.sendnosign;
import org.minima.system.commands.send.sendpoll;
import org.minima.system.commands.send.sendpost;
import org.minima.system.commands.send.sendsign;
import org.minima.system.commands.send.sendview;
import org.minima.system.commands.send.wallet.consolidatefrom;
import org.minima.system.commands.send.wallet.constructfrom;
import org.minima.system.commands.send.wallet.createfrom;
import org.minima.system.commands.send.wallet.createtokenfrom;
import org.minima.system.commands.send.wallet.postfrom;
import org.minima.system.commands.send.wallet.rawfrom;
import org.minima.system.commands.send.wallet.rawtxnfrom;
import org.minima.system.commands.send.wallet.sendfrom;
import org.minima.system.commands.send.wallet.signfrom;
import org.minima.system.commands.signatures.sign;
import org.minima.system.commands.signatures.verify;
import org.minima.system.commands.txn.txnaddamount;
import org.minima.system.commands.txn.txnauto;
import org.minima.system.commands.txn.txnbasics;
import org.minima.system.commands.txn.txncheck;
import org.minima.system.commands.txn.txnclear;
import org.minima.system.commands.txn.txncoinlock;
import org.minima.system.commands.txn.txncreate;
import org.minima.system.commands.txn.txndelete;
import org.minima.system.commands.txn.txnexport;
import org.minima.system.commands.txn.txnimport;
import org.minima.system.commands.txn.txninput;
import org.minima.system.commands.txn.txnlist;
import org.minima.system.commands.txn.txnlock;
import org.minima.system.commands.txn.txnmine;
import org.minima.system.commands.txn.txnminepost;
import org.minima.system.commands.txn.txnmmr;
import org.minima.system.commands.txn.txnoutput;
import org.minima.system.commands.txn.txnpost;
import org.minima.system.commands.txn.txnscript;
import org.minima.system.commands.txn.txnsign;
import org.minima.system.commands.txn.txnstate;
import org.minima.system.commands.txn.txnview;
import org.minima.utils.MinimaLogger;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;
import org.minima.utils.json.parser.JSONParser;
import org.minima.utils.json.parser.ParseException;

public class CommandRunner {
	
	/**
	 * All the Valid Commands
	 */
	public static final Command[] ALL_COMMANDS = 
		{   new quit(), new status(), new coins(), new txpow(), new connect(), new disconnect(), new network(),
			new message(), new trace(), new help(), new printtree(), new automine(), new printmmr(), new rpc(),
			new send(), new balance(), new tokencreate(), new tokenvalidate(), new tokens(),new getaddress(), new newaddress(), new debugflag(),
			new incentivecash(), new webhooks(), new peers(), new p2pstate(),

			new sendpoll(), new healthcheck(), new mempool(), new block(), new reset(),
			
			new whitepaper(), new sendnosign(), new sendsign(), new sendpost(), new sendview(),
			new sendfrom(), new createfrom(), new rawfrom(), new rawtxnfrom(), new signfrom(), new postfrom(), new constructfrom(), new consolidatefrom(),
			new createtokenfrom(),
			
			new archive(), new logs(), new history(), new convert(),new maths(),
			new restoresync(), new timemilli(),
			
			new decryptbackup(), new megammrsync(), new systemcheck(), new scanchain(),
			
			new multisig(), new multisigread(), new checkaddress(),
			
			new sphincs(), new rescue(),
			
			new ping(), new random(), new seedrandom(), new mysql(), new mysqlcoins(), new slavenode(), 
			//new file(),
			
			new jnitest(), new benchmark(), new jniminingtest(),
			
			new megammr(), 
			
			new vault(), new consolidate(), new coinnotify(),
			new backup(), new restore(), new test(), 
			new runscript(), new tutorial(),new keys(),
			new scripts(), new newscript(), new removescript(),
			new burn(),
			
			new txnbasics(),new txncreate(), new txninput(),new txnlist(), new txnclear(), new txnview(),
			new txnoutput(),new txnstate(),new txnsign(),new txnpost(),new txndelete(),
			new txnexport(),new txnimport(),new txncheck(), new txnscript(), new txnauto(),
			new txnaddamount(),new txnlock(), new txnmmr(), new txnmine(), new txnminepost(), new txncoinlock(),
			
			new coinimport(), new coinexport(),new cointrack(), new coincheck(),
			
			new hash(), new hashtest(), new sign(), new verify(),
			
			new mmrcreate(), new mmrproof()
		};
	
	
	/**
	 * Get a Command RUnner to run / check commands
	 */
	public static CommandRunner getRunner() {
		return new CommandRunner();
	}
	
	private CommandRunner() {}
	
	public JSONArray runMultiCommand(String zCommand) {
		return runMultiCommand("0x00", zCommand);
	}
	
	public JSONObject runSingleCommand(String zCommand) {
		JSONArray res 		= runMultiCommand(zCommand);
		if(res.size()>0) {
			JSONObject result 	= (JSONObject) res.get(0);
			return result;
		}
		return new JSONObject();
	}

	/**
	 * SECURITY (command injection): run ONE command the node builds itself, from parts that are
	 * already separated - the command name, then name, value, name, value.. A value is NEVER parsed as command
	 * text, so a ';', quote, space or 'name:' inside it (a JSON-decoded param, a coin's on-chain state, a stored
	 * login..) cannot start another command or add / replace a parameter. A String value is typed exactly as a
	 * typed value is ({..} JSON object, [..] JSON array except for txnstate, surrounding quotes dropped); a
	 * JSONObject / JSONArray is used as it is; anything else as its toString(). Runs as ADMIN (0x00), as the
	 * text these commands used to build always did.
	 */
	public JSONObject runCommand(String zName, Object... zNameValues) {

		Command cmd = getCommandOnly(zName);
		if(cmd == null || zNameValues.length % 2 != 0) {
			cmd = new missingcmd(zName, "Invalid built command : "+zName);
		}else {
			for(int i=0;i<zNameValues.length;i+=2) {
				String name  = (String)zNameValues[i];
				Object value = zNameValues[i+1];
				if(value instanceof String) {
					try {
						value = getParamValue(zName, (String)value);
					} catch (ParseException e) {
						cmd = new missingcmd(zName,"Invalid JSON parameter for "+zName+" @ "+name+" "+e.toString());
						break;
					}
				}else if(!(value instanceof JSONObject) && !(value instanceof JSONArray)) {
					value = String.valueOf(value);
				}
				cmd.getParams().put(name, value);
			}
		}
		cmd.setMiniDAPPID("0x00");

		//Same parameter check as a typed command
		ArrayList<String> validparams = cmd.getValidParams();
		for(Object key : cmd.getParams().keySet()) {
			if(!validparams.contains(key)) {
				JSONObject result = new JSONObject();
				result.put("command", zName);
				result.put("params", cmd.getParams());
				result.put("status", false);
				result.put("pending", false);
				result.put("error", "Invalid parameter : "+key);
				return result;
			}
		}

		JSONObject result = null;
		try {
			result = cmd.runCommand();
		}catch(CommandException cexc) {
			result = cmd.getJSONReply();
			result.put("status", false);
			result.put("error", cexc.getMessage());
		}catch(Exception exc) {
			MinimaLogger.log(exc);
			result = cmd.getJSONReply();
			result.put("status", false);
			result.put("error", exc.toString());
		}

		return result;
	}

	/**
	 * The txnscript 'scripts' parameter for one script with no proof - what scripts:{"SCRIPT":""} parsed to
	 */
	public static JSONObject scripts(String zScript) {
		JSONObject scripts = new JSONObject();
		scripts.put(zScript, "");
		return scripts;
	}

	/**
	 * Several built commands in order - each { name, name, value, .. } as for runCommand - stopping after the
	 * first that fails, as runMultiCommand did for the ';' chains these replace.
	 */
	public JSONArray runCommands(Object[]... zCommands) {
		JSONArray results = new JSONArray();
		for(Object[] command : zCommands) {
			JSONObject result = runCommand((String)command[0], Arrays.copyOfRange(command, 1, command.length));
			results.add(result);
			if(!(boolean)result.get("status")) {
				break;
			}
		}
		return results;
	}

	/**
	 * runCommands for a chain that works on the in-progress transaction zTxnID: when the chain stops on a
	 * failure, the row is deleted (house rule: every txn error path runs txndelete - the old ';' chains
	 * left it behind).
	 */
	public JSONArray runTxnCommands(String zTxnID, Object[]... zCommands) {
		JSONArray results = runCommands(zCommands);
		JSONObject last   = (JSONObject) results.get(results.size()-1);
		if(!(boolean) last.get("status")) {
			runCommand("txndelete", "id", zTxnID);
		}
		return results;
	}
	
	public JSONArray runMultiCommand(String zMiniDAPPID, String zCommand) {
		
		JSONArray finalresult = new JSONArray();
		
		//First break it up..
		StringTokenizer strtok = new StringTokenizer(zCommand, ";");
		while(strtok.hasMoreTokens()) {
			String command = strtok.nextToken().trim();
			
			//Get this command..
			Command cmd = getCommand(command);
			
			//Set who called it
			cmd.setMiniDAPPID(zMiniDAPPID);
			
			//The final result
			JSONObject result = null;
			
			//Check the Parameters
			ArrayList<String> validparams 	= cmd.getValidParams();
			
			JSONObject allparams 			=  cmd.getParams();
			Set<String> keys 				= allparams.keySet(); 
			
			boolean validp=true;
			for(String key : keys) {
				if(!validparams.contains(key)) {
					
					//Invalid Param
					result=  new JSONObject();
					result.put("command", command);
					result.put("params", allparams);
					result.put("status", false);
					result.put("pending", false);
					result.put("error", "Invalid parameter : "+key);
					
					//Add to the List..
					finalresult.add(result);
					
					//And that's all folks..
					validp=false;
					break;
				}
			}
			
			//Are we valid..
			if(!validp) {
				break;
			}
			
			//What is the command
			String comname = cmd.getName();
			
			//Check this MiniDAPP can make this call..
			boolean allowed = isCommandAllowed(comname);
			
			//Is this an ADMIN User
			if(!zMiniDAPPID.equals("0x00")) {
				
				//Public MiniDAPPs cannot add to pending..
				if(!allowed) {
					result=  new JSONObject();
					result.put("command", command);
					result.put("status", false);
					result.put("pending", false);
					result.put("error", "You need ADMIN privileges to run this command");
					
					//Add to the List..
					finalresult.add(result);
					
					//And that's all folks..
					break;
				}
			}

			//Is this a MiniDAPP..
			/*if(zMiniDAPPID.equals(Main.getInstance().getMDSManager().getPublicMiniDAPPID())) {
				
				//Public MiniDAPPs cannot add to pending..
				if(!allowed) {
					result=  new JSONObject();
					result.put("command", command);
					result.put("status", false);
					result.put("pending", false);
					result.put("error", "Public MDS cannot run WRITE commands");
					
					//Add to the List..
					finalresult.add(result);
					
					//And that's all folks..
					break;
				}
				
			}else if(!zMiniDAPPID.equals("0x00")) {
				
				if(!allowed) {
					
					//Get the MiniDAPP
					MiniDAPP md = Main.getInstance().getMDSManager().getMiniDAPP(zMiniDAPPID);
					
					//Does it have WRITE permission..
					if(!md.getPermission().equalsIgnoreCase("write")) {
					
						//Add to pending..
						String puid = Main.getInstance().getMDSManager().addPendingCommand(md, command);
						
						//And return..
						result=  new JSONObject();
						result.put("command", command);
						result.put("status", false);
						result.put("pending", true);
						result.put("pendinguid", puid);
						result.put("error", "This command needs to be confirmed and is now pending..");
						
						//Add to the List..
						finalresult.add(result);
						
						//And that's all folks..
						break;
					}
				}
			}*/
			
			try {
				result = cmd.runCommand();
				
			}catch(CommandException cexc) {
				result = cmd.getJSONReply();
				result.put("status", false);
				result.put("error", cexc.getMessage());
				
			}catch(Exception exc) {
				//Print the full error
				MinimaLogger.log(exc);
				
				result = cmd.getJSONReply();
				result.put("status", false);
				result.put("error", exc.toString());
			}
			
			//Add it..
			finalresult.add(result);
			
			//Stop at a false..
			if((boolean)result.get("status") == false) {
				break;
			}
		}
		
		return finalresult;
	}
	
	public Command getCommandOnly(String zCommandName) {
		int commandlen = ALL_COMMANDS.length;
		
		Command comms = null;
		for(int i=0;i<commandlen;i++) {
			if(ALL_COMMANDS[i].getName().equals(zCommandName)) {
				comms = ALL_COMMANDS[i].getFunction();
				break;
			}
		}
	
		return comms;
	}
	
	public Command getCommand(String zCommand) {
		int commandlen = ALL_COMMANDS.length;
		
		//Get the first word..
		String[] split = splitStringJSON(false,zCommand);
		
		//The first is the command..
		String command = split[0].toLowerCase();
		
		Command comms = null;
		for(int i=0;i<commandlen;i++) {
			if(ALL_COMMANDS[i].getName().equals(command)) {
				comms = ALL_COMMANDS[i].getFunction();
				break;
			}
		}
		
		//If not found return error
		if(comms == null) {
			return new missingcmd(command,"Command not found");
		}
		
		//Set the Complete Command - for reference..
		comms.setCompleteCommand(zCommand);
		
		//get the parameters if any
		int len = split.length;
		for(int i=1;i<len;i++) {
			String token = split[i];
			
			//Find the :
			int index 	 = token.indexOf(":");
			if(index == -1) {
				return new missingcmd(command,"Invalid parameters for "+command+" @ "+token);
			}
			
			String name  = token.substring(0, index).trim();
			String value = token.substring(index+1).trim();

			try {
				comms.getParams().put(name, getParamValue(command, value));
			} catch (ParseException e) {
				String type = value.startsWith("{") ? "JSON" : "JSONArray";
				return new missingcmd(command,"Invalid "+type+" parameter for "+command+" @ "+token+" "+e.toString());
			}
		}

		return comms;
	}

	/**
	 * How a parameter value is typed - shared by typed commands and runCommand()
	 */
	private static Object getParamValue(String zCommand, String zValue) throws ParseException {
		String value = zValue.trim();

		//Is the value a JSON..or JSONArray..
		if(value.startsWith("{") && value.endsWith("}")) {
			//It's a JSON..!
			return (JSONObject) new JSONParser().parse(value);

		}else if(value.startsWith("[") && value.endsWith("]")) {

			//Is this a state variable - could be a String variable..
			if(zCommand.equals("txnstate")) {
				return value;
			}

			//It's a JSONArray..!
			return (JSONArray) new JSONParser().parse(value);
		}

		//Should not have any "..
		if(value.startsWith("\"")) {
			value = value.substring(1);
		}

		if(value.endsWith("\"")) {
			value = value.substring(0,value.length()-1);
		}

		return value;
	}
	
	private String[] splitStringJSON(boolean zForceNormal, String zInput) {
		
		//Are there any JSON in this.. if not use super fast method..
		if(!zForceNormal) {
			
			//Check for windows files.. as : screws up
			boolean iswindowsfile = (zInput.indexOf(":\\")!=-1);
			
			//If it's big use the fast one.. but can have issues if : used weirdly.. :( 
			if(!iswindowsfile && zInput.indexOf("{") == -1 && zInput.indexOf("[") == -1) {
				String[] fastres = splitterQuotedPattern(zInput);
				if(fastres != null) {
					return fastres;
				}
			}
		}
		
		//Normal method..
		ArrayList<String> token = new ArrayList<>();
		String ss = zInput.trim();
		
		//Cycle through looking for spaces or quotes..
		String current = new String();
		
		int jsoned 		= 0;
		boolean quoted 	= false;
		
		int len = ss.length();
		for(int i=0;i<len;i++) {
			char cc = ss.charAt(i);
			
			if(cc == ' ') {
				//End of the line..
				if(!quoted && jsoned==0) {
					
					//Add current
					if(!current.equals("")) {
						token.add(current.trim());
					}
						
					//New Current
					current = new String();
					
				}else {
					current += cc;
				}
			
			}else if(cc == '{') {
				jsoned++;
				current += cc;
				
			}else if(cc == '}') {
				jsoned--;
				current += cc;
			
			}else if(cc == '[') {
				jsoned++;
				current += cc;
				
			}else if(cc == ']') {
				jsoned--;
				current += cc;
			
			
			}else if(cc == '\"') {
				if(jsoned>0) {
					
					//It's in a JSON.. so keep it..
					current += cc;
					
				}else {
					//it's a quote!
					if(quoted) {
						//It's finished..
						quoted=false;
					}else {
						quoted=true;
					}
				}
				
			}else {
				current += cc;
			}
		}
		
		//Add the last bit..
		if(!current.equals("")) {
			token.add(current.trim());
		}
		
		return token.toArray(new String[0]);
	}
	
	/**
	 * Very Fast splitter that doesn't work for JSON inputs.. just normal name value pairs..
	 * 
	 * If something weird happens return null .. and other method used..
	 */
	public String[] splitterQuotedPattern(String zInput) {
		ArrayList<String> token = new ArrayList<>();
		
		//Split the : name value pairs aswell..
		String ss = zInput.trim().replaceAll(":", " : ");
		
		//Search for quoted strings
		String regex 	= "\"([^\"]*)\"|(\\S+)";
		Matcher m 		= Pattern.compile(regex).matcher(ss);
	    while (m.find()) {
	        if (m.group(1) != null) {
	        	token.add(m.group(1));
	        } else {
	        	token.add(m.group(2));
	        }
	    }
		
	    //Now cycle and add up the name:value pairs
	    ArrayList<String> finaltokens = new ArrayList<>();
	    boolean first 		= true; 
	    boolean namefound	= false;
	    String nvpair 		= new String("");
	    
	    for(String tok : token) {
	    	
	    	if(first){
	    		//First one is just the command
	    		first = false;
	    		finaltokens.add(tok);
	    		
	    	}else {
		    	//Have we found the name yet
		    	if(!namefound) {
		    		
		    		//CHECK ..
		    		if(tok.trim().equals(":")) {
		    			//Hmm. should not HAPPEN!.. use slow method
		    			return null;
		    		}
		    		
		    		namefound 	 = true;
		    		nvpair 		= new String(tok);
		    		
		    	}else {
		    		if(tok.trim().equals(":")) {
		    			nvpair+=":";
		    		}else {
		    			namefound 	 = false;
		    			nvpair		+=tok;
		    			
		    			//Add to the final list
		    			finaltokens.add(nvpair);
		    		}
		    	}
	    	}
	    }
	    
		return finaltokens.toArray(new String[0]);
	}
	
	/**
	 * Which Commands are WRITE commands..
	 */
	private final String[] ALL_WRITE_COMMANDS = 
		{"send","sendpoll","sendsign","multisig","tokencreate","consolidate",
		 "cointrack","sign","txnauto","txnsign","mds","backup","removescript",
		 "restore","restoresync","vault","archive","mysql","mysqlcoins",
		 "rpc","magic","quit","seedrandom","megammrsync","megammr","decryptbackup",
		 "reset"};
	
	private final ArrayList<String> ALL_WRITE_COMMANDS_ARRAY = new ArrayList<String>(Arrays.asList(ALL_WRITE_COMMANDS));
	
	public boolean isCommandAllowed(String zCommand) {
		
		//Is it a simple READ command
		if(ALL_WRITE_COMMANDS_ARRAY.contains(zCommand.trim())) {
			return false;
		}
		
		return true;
	}
	
}