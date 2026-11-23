package org.minima.system.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Set;
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
	 * SECURITY (command injection 2026-11-21): run ONE command the node builds itself, from parts that are
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
		cmd.setWriteAllowed(true);

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
	
	/**
	 * SECURITY (2026-11-23): the commands in a command line - split at each ';' that is NOT inside a value. A
	 * "quoted" value runs to its closing quote (a backslash escapes the next character, so \" does not close
	 * it); a {..} or [..] JSON value runs to its closing bracket, and its own "strings" may hold anything.
	 * The old StringTokenizer split at EVERY ';', so a ';' inside a runscript / newscript script:"..", a
	 * txnstate value:".." or a JSON parameter - text apps often take from chain data - started a new command
	 * that ran with the caller's rights. Pieces are trimmed and empty ones skipped, as before.
	 *
	 * Returns null when a quote or bracket is never closed: where that value ends, and so where the next
	 * command starts, is unknown - the caller must run nothing.
	 */
	public static ArrayList<String> splitCommands(String zCommand) {
		ArrayList<String> commands = new ArrayList<>();
		int start 		= 0;
		int depth 		= 0;
		boolean quoted 	= false;
		int len = zCommand.length();
		for(int i=0;i<len;i++) {
			char cc = zCommand.charAt(i);
			if(quoted) {
				if(cc == '\\') {
					i++;
				}else if(cc == '"') {
					quoted = false;
				}
			}else if(cc == '"') {
				quoted = true;
			}else if(cc == '{' || cc == '[') {
				depth++;
			}else if(cc == '}' || cc == ']') {
				//A stray closing bracket outside any JSON value is just text
				if(depth > 0) {
					depth--;
				}
			}else if(cc == ';' && depth == 0) {
				addCommand(commands, zCommand.substring(start, i));
				start = i+1;
			}
		}
		
		if(quoted || depth != 0) {
			return null;
		}
		
		addCommand(commands, zCommand.substring(start));
		return commands;
	}
	
	/**
	 * SECURITY (2026-11-23): run zCommand as exactly ONE command, for a caller that never means a chain. The ';'
	 * splitter keeps a ';' inside a "quoted" or [..] / {..} value, but it cannot know that an UNQUOTED value was
	 * meant to be one: a STRING state is [..], so a coin state "[1];send ..;[2]" pasted unquoted after value: is
	 * [1] - a closed value - then a ';' and a send. Here a line that holds more than one command (or an unclosed
	 * quote / bracket) runs NOTHING and returns one error. runSingleCommand, by contrast, runs every command of a
	 * chain and returns only the first reply - callers send chains through it on purpose.
	 */
	public JSONObject runOneCommand(String zMiniDAPPID, String zCommand) {
		ArrayList<String> commands = splitCommands(zCommand);
		if(commands == null || commands.size() != 1) {
			JSONObject result = new JSONObject();
			result.put("command", zCommand);
			result.put("status", false);
			result.put("pending", false);
			result.put("error", commands == null ? "A quote or bracket is never closed - nothing was run"
					: "Exactly one command expected - a ';' outside a quoted or [..] value starts another - nothing was run");
			return result;
		}
		return (JSONObject) runMultiCommand(zMiniDAPPID, commands.get(0)).get(0);
	}
	
	private static void addCommand(ArrayList<String> zCommands, String zCommand) {
		String command = zCommand.trim();
		if(!command.isEmpty()) {
			zCommands.add(command);
		}
	}
	
	public JSONArray runMultiCommand(String zMiniDAPPID, String zCommand) {
		
		JSONArray finalresult = new JSONArray();
		
		//First break it up.. at the ';' between commands, never at one inside a value
		ArrayList<String> commands = splitCommands(zCommand);
		if(commands == null) {
			JSONObject result = new JSONObject();
			result.put("command", zCommand);
			result.put("status", false);
			result.put("pending", false);
			result.put("error", "A quote or bracket is never closed - nothing was run");
			finalresult.add(result);
			return finalresult;
		}
		
		for(String command : commands) {
			
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
			
			//SECURITY (review 2026-11-21): some uses of an open command need WRITE rights (a file outside the
			//base folder, txnpost burn:, a postfrom over an existing row..) - gate them like WRITE commands
			String writeuse = cmd.getWriteUse();
			if(writeuse != null) {
				allowed = false;
			}
			
			//SECURITY (Dexicon review 2026-11-07; DexiconPro line 2026-11-23): a non-ADMIN txnimport may only create a NEW row under an explicit id -
			//never overwrite another app's draft (TxnDB.addCompleteTransaction replaces an existing id silently).
			if(allowed && !zMiniDAPPID.equals("0x00") && comname.equals("txnimport")) {
				Object idp = cmd.getParams().get("id");
				String tid = idp == null ? "" : idp.toString().trim();
				if(tid.isEmpty() || org.minima.database.MinimaDB.getDB().getCustomTxnDB().getTransactionRow(tid) != null) {
					allowed = false;
				}
			}
			
			//Is this an ADMIN User
			if(!zMiniDAPPID.equals("0x00")) {
				
				//Public MiniDAPPs cannot add to pending..
				if(!allowed) {
					result=  new JSONObject();
					result.put("command", command);
					result.put("status", false);
					result.put("pending", false);
					result.put("error", "You need ADMIN privileges to run this command"+(writeuse == null ? "" : " ("+writeuse+")"));
					
					//Add to the List..
					finalresult.add(result);
					
					//And that's all folks..
					break;
				}
			}
			
			//Past the gate: may it use the node's keys and files
			cmd.setWriteAllowed(zMiniDAPPID.equals("0x00"));

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
				//A failed non-admin write frees the file it claimed - a retry under the same name is not refused
				cmd.releaseClaims();
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
		 "reset",
		 // SECURITY (2026-11-23): a non-admin caller (a read-mode RPC user, a companion) may not add a webhook - every
		 // block, txpow and MiniDapp event posted to its own URL - nor connect the node to its peer or drop a peer
		 "webhooks","connect","disconnect",
		 // SECURITY (2026-11-21): rescue starts a MegaMMR resync from any host: and the node halts afterwards
		 "rescue",
		 // SECURITY (Dexicon review 2026-11-07): the in-progress transaction table is ONE shared table with no owner.
		 // A non-ADMIN companion (userid 0xFF) could txnlist another app's draft and rewrite it (txnimport over its id,
		 // txninput/txnoutput) between that app's import and its txnsign, then read the signed copy back - stealing
		 // whatever the victim's key unlocks. Listing, exporting and editing rows now needs ADMIN. Left open for
		 // non-admin companions (apks/wallet signs locally and posts via txnimport -> txnbasics -> txnpost): txnimport
		 // (a NEW id only - see runMultiCommand), txnbasics and txnpost on a row, which cannot redirect funds;
		 // txnview / txncheck / txnutils stay readable (decode only).
		 "txnaddamount","txnclear","txncoinlock","txncreate","txndelete","txnexport",
		 "txninput","txnlist","txnlock","txnmine","txnminepost","txnmmr","txnoutput","txnscript","txnstate"};
	
	private final ArrayList<String> ALL_WRITE_COMMANDS_ARRAY = new ArrayList<String>(Arrays.asList(ALL_WRITE_COMMANDS));
	
	public boolean isCommandAllowed(String zCommand) {
		
		//Is it a simple READ command
		if(ALL_WRITE_COMMANDS_ARRAY.contains(zCommand.trim())) {
			return false;
		}
		
		return true;
	}
	
}