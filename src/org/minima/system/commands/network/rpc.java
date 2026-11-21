package org.minima.system.commands.network;

import java.util.ArrayList;
import java.util.Arrays;

import org.minima.database.MinimaDB;
import org.minima.database.userprefs.UserDB;
import org.minima.system.Main;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandException;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

public class rpc extends Command {

	public rpc() {
		super("rpc","(enable:) (ssl:) (password:) - Enable and disable RPC on port "+GeneralParams.RPC_PORT+" (default is off)");
	}
	
	@Override
	public String getFullHelp() {
		return "\nrpc\n"
				+ "\n"
				+ "Enable and disable RPC on port "+GeneralParams.RPC_PORT+" (default is off).\n"
				+ "\n"
				+ "Ensure your RPC port is secured behind a firewall before enabling.\n"
				+ "\n"
				+ "The Deafult user is minima. That has write mode access. You can add other rpc users with read access.\n"
				+ "\n"
				+ "enable:\n"
				+ "    true or false, true to enable rpc or false to disable.\n"
				+ "\n"
				+ "ssl:\n"
				+ "    true or false, true to enable Self signed SSL - you can use stunnel yourself.\n"
				+ "\n"
				+ "password:\n"
				+ "    the Basic Auth password used in headers - ONLY secure if used with SSL.\n"
				+ "    Needed the first time RPC is enabled. Without it the current password stays - authentication is never switched off.\n"
				+ "\n"
				+ "username:\n"
				+ "    the Username of an RPC user.\n"
				+ "\n"
				+ "mode:\n"
				+ "    the read or write mode of an RPC user.\n"
				+ "\n"
				+ "action:\n"
				+ "    adduser - add an RPC user.\n"
				+ "    removeuser - remove an RPC user.\n"
				+ "    listusers - list RPC users.\n"
				+ "\n"
				+ "Examples:\n"
				+ "\n"
				+ "rpc enable:true ssl:true password:minimarpcpassword\n"
				+ "\n"
				+ "rpc enable:true password:minima\n"
				+ "\n"
				+ "rpc action:adduser username:rpcuser password:rpcpassword mode:read\n"
				+ "\n"
				+ "rpc action:removeuser username:rpcuser\n"
				+ "\n"
				+ "rpc action:listusers\n"
				+ "\n"
				+ "rpc enable:false\n";
	}
	
	@Override
	public ArrayList<String> getValidParams(){
		return new ArrayList<>(Arrays.asList(new String[]{"enable","ssl","password","action","username","password","mode"}));
	}
	
	@Override
	public JSONObject runCommand() throws Exception{
		JSONObject ret = getJSONReply();
		
		UserDB userdb 		= MinimaDB.getDB().getUserDB();
		boolean listusers	= false;
		
		if(existsParam("enable")) {
		
			//Enable or Disable
			boolean enable = getBooleanParam("enable");
			
			//SECURITY (2026-11-21): enabling without a password used to switch authentication OFF - an open
			//admin RPC. Without password: the current password stays; with none ever set, refuse.
			if(enable && !existsParam("password") && !GeneralParams.RPC_AUTHENTICATE) {
				throw new CommandException("Set a password to enable RPC : rpc enable:true password:..");
			}
			
			//Stop the Old
			Main.getInstance().getNetworkManager().stopRPC();
			Thread.sleep(1000);
		
			//Store
			GeneralParams.RPC_ENABLED = enable;
			
			//Is there a password
			if(existsParam("password")) {
				GeneralParams.RPC_PASSWORD = getParam("password");
				GeneralParams.RPC_AUTHENTICATE = true;
			}
			
			//Now start or stop..
			if(enable) {
				Main.getInstance().getNetworkManager().startRPC();
				
				//Wait for it to listen - or fail - so the reply below says what really happened
				for(int i=0;i<30;i++) {
					if(Main.getInstance().getNetworkManager().isRPCListening()
							|| Main.getInstance().getNetworkManager().getRPCStartError() != null) {
						break;
					}
					Thread.sleep(100);
				}
			}else {
				Main.getInstance().getNetworkManager().stopRPC();
			}
		
		}else if(existsParam("action")) {
			
			//What else can we do..
			String action = getParam("action");
			
			if(action.equals("adduser")) {
			
				if(!GeneralParams.RPC_AUTHENTICATE) {
					throw new CommandException("You must set a default RPC password (for user minima) via -rpcpassword to add extra users");
				}
				
				String username = getParam("username");
				if(username.equals("minima")) {
					throw new CommandException("Cannot add User minima - this is the default RPC user.");
				}
				
				String password = getParam("password");
				String mode 	= getParam("mode").toLowerCase();
				if(!mode.equals("read") && !mode.equals("write")) {
					throw new CommandException("RPC User mode MUST be either read or write");
				}
				
				JSONObject newuser = new JSONObject();
				newuser.put("username", username);
				newuser.put("password", password);
				newuser.put("mode", mode);
				
				//Add it..!
				userdb.getRPCUsers().add(newuser);
				
			}else if(action.equals("removeuser")) {
				
				String username = getParam("username");
				
				JSONArray users 	= userdb.getRPCUsers();
				JSONArray newusers 	= new JSONArray();
				
				for(Object userobj : users) {
					JSONObject user = (JSONObject)userobj;
					
					//Is it the one to be removed..
					if(!user.getString("username").equals(username)) {
						newusers.add(user);
					}
				}
				
				//And set these..
				userdb.setRPCUsers(newusers);
				
			}else if(action.equals("listusers")) {
				listusers = true;
			}
		}
		
		JSONObject rpcdets = new JSONObject();
		rpcdets.put("enabled", GeneralParams.RPC_ENABLED);
		//enabled is the setting; listening is the truth (a bad -rpcbind or a taken port leaves it down)
		rpcdets.put("listening", Main.getInstance().getNetworkManager().isRPCListening());
		rpcdets.put("bind", GeneralParams.RPC_BIND_HOST.equals("") ? "*" : GeneralParams.RPC_BIND_HOST);
		String rpcerror = Main.getInstance().getNetworkManager().getRPCStartError();
		if(rpcerror != null) {
			rpcdets.put("error", rpcerror);
		}
		rpcdets.put("port", GeneralParams.RPC_PORT);
		rpcdets.put("authenticate", GeneralParams.RPC_AUTHENTICATE);
		rpcdets.put("username", "minima");
		rpcdets.put("password", "***");
		
		//Show the extra Users
		if(!listusers) {
			rpcdets.put("rpcusers", userdb.getRPCUsers().size());
		}else {
			rpcdets.put("rpcusers", userdb.getRPCUsers());
		}
		
		ret.put("response", rpcdets);
		
		return ret;
	}

	@Override
	public Command getFunction() {
		return new rpc();
	}

}
