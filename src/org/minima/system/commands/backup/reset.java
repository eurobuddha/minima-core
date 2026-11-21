package org.minima.system.commands.backup;

import java.util.ArrayList;
import java.util.Arrays;

import org.minima.system.Main;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandException;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

public class reset extends Command {

	public reset() {
		super("reset","[archivefile:] [action:] (file:) (password:) (keys:) (keyuses:) - Reset the entire system using an Archive Backup file...");
	}
	
	@Override
	public String getFullHelp() {
		return "\nreset\n"
				+ "\n"
				+ "Reset your node in various ways. You MUST wait until all your original keys are created before this is allowed.\n"
				+ "\n"
				+ "archivefile:\n"
				+ "    Specify the the archive gzip file. Should be recently exported from an archive node.\n"
				+ "\n"
				+ "action:\n"
				+ "    chainsync: Re-sync all blocks from the archivefile to get back onto the right chain.\n"
				+ "               Seed phrase is not required, the private keys will remain unchanged.\n"
				+ "    seedsync: Wipe the wallet and re-generate your keys from your seed phrase. Your coins will be restored.\n"
				+ "    restore: Restore a backup and re-sync the entire chain from the archivefile.\n"
				+ "\n"
				+ "file:\n"
				+ "    Specify the filename or local path of the backup to restore\n"
				+ "\n"
				+ "password: (optional)\n"
				+ "    Enter the password of the backup \n"
				+ "\n"
				+ "phrase: (optional)\n"
				+ "    Your 24 word seed phrase in double quotes. Use with 'action:seedsync'.\n"
				+ "\n"
				+ "keys: (optional)\n"
				+ "    Number of keys to create if you need to do a seed re-sync. Default is 64. Use with 'action:seedsync'.\n"
				+ "\n"
				+ "keyuses: (optional)\n"
				+ "    How many times at most you used your keys. Use with 'action:seedsync'.\n"
				+ "    Every time you re-sync with seed phrase this needs to be higher as Minima Signatures are stateful.\n"
				+ "    Defaults to 1000 - the max is 262144 for normal keys.\n"
				+ "\n"
				+ "Examples:\n"
				+ "\n"
				+ "reset archivefile:archiveexport-jul23.gz action:chainsync\n"
				+ "\n"
				+ "reset archivefile:archiveexport-jul23.gz action:seedsync keyuses:1000 phrase:\"ENTER 24 WORDS HERE\"\n"
				+ "\n"
				+ "reset archivefile:archiveexport-jul23.gz action:restore file:backup-jul23.bak password:Longsecurepassword456\n"; 
	}
	
	@Override
	public ArrayList<String> getValidParams(){
		return new ArrayList<>(Arrays.asList(new String[]{"action","archivefile","file","password","phrase","keys","keyuses"}));
	}
	
	@Override
	public JSONObject runCommand() throws Exception {
		JSONObject ret = getJSONReply();
		
		//Can only do this if all keys created..
		vault.checkAllKeysCreated();
		
		//Get the archive file..
		String archivefile = getParam("archivefile");
		
		//Which action
		String action = getParam("action"); 
		
		if(action.equals("chainsync")) {
			
			//Refactor the command
			JSONObject result 	= CommandRunner.getRunner().runCommand("archive", "action","import", "file",archivefile);
			ret.put("response", result);
			
		}else if(action.equals("seedsync")) {
			
			//Get the phrase
			String phrase = getParam("phrase");
			
			//Refactor the command - SECURITY (command injection): from separated parameters
			ArrayList<Object> command = new ArrayList<>(Arrays.asList(new Object[] {"action","import", "file",archivefile, "phrase",phrase}));

			//Add extra params
			if(existsParam("keys")) {
				command.add("keys");
				command.add(getParam("keys"));
			}
			
			if(existsParam("keyuses")) {
				command.add("keyuses");
				command.add(getParam("keyuses"));
			}
			
			JSONObject result 	= CommandRunner.getRunner().runCommand("archive", command.toArray());
			ret.put("response", result);
			
		
		}else if(action.equals("restore")) {
			
			//Get the backup file
			String backupfile = getParam("file");
			
			//Now do a restore..
			ArrayList<Object> command = new ArrayList<>(Arrays.asList(new Object[] {"shutdown","false", "file",backupfile}));
			
			//Password ?
			if(existsParam("password")) {
				command.add("password");
				command.add(getParam("password"));
			}
			
			JSONObject result 	= CommandRunner.getRunner().runCommand("restore", command.toArray());
			
			//Check worked..
			if(!(boolean)result.get("status")) {
				throw new CommandException("Error restoring.. "+result.toJSONString());
			}
			
			JSONObject allres = new JSONObject();
			allres.put("restore", result);
			
			//Now reopen the required SQL Dbs..
			Main.getInstance().restoreReadyForSync();
			
			//And NOW do a chain resync..
			result 	 = CommandRunner.getRunner().runCommand("archive", "action","import", "file",archivefile);
			allres.put("chainsync", result);
			
			//The final results..
			ret.put("response", allres);
			
		}else {
			throw new CommandException("Invalid action : "+action);
		}
		
		return ret;
	}
	
	@Override
	public Command getFunction() {
		return new reset();
	}

}
