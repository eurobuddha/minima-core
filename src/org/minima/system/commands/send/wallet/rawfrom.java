package org.minima.system.commands.send.wallet;

import java.util.ArrayList;
import java.util.Arrays;

import org.minima.objects.StateVariable;
import org.minima.objects.base.MiniData;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandException;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

public class rawfrom extends Command {
	
	public rawfrom() {
		super("rawfrom","[inputs:] [outputs:] (state:) - [DEPRECATED USE rawtxnfrom ] Create an unsigned transaction from a set of inputs, outputs and state");
	}
	
	@Override
	public ArrayList<String> getValidParams(){
		return new ArrayList<>(Arrays.asList(new String[]{"inputs","outputs","state"}));
	}
	
	@Override
	public String getFullHelp() {
		return "\nrawfrom\n"
				+ "\n"
				+ "Create an unsigned transaction with a list of input and output JSON coins.\n"
				+ "\n"
				+ "inputs:\n"
				+ "    A JSONArray of input JSON coins with a coinid and script value.\n"
				+ "\n"
				+ "outputs:\n"
				+ "    A JSONArray of output JSON coins with an address, amount and optional tokenid and storestate.\n"
				+ "\n"
				+ "state: (optional)\n"
				+ "    The JSON state.\n"
				+ "\n"
				+ "Examples:\n"
				+ "\n"
				+ "rawfrom inputs:[{\"coinid\":\"0x859EC55D1DC0DD66E72C1D8B6CDBD5E12D0EC5CB56255F629BAEC590047C22C4\",\"script\":\"RETURN SIGNEDBY(0x677228489AD4C14AC0D04F9BA054974A2B1D60C5A8E8D710CF9F2BF64D1EE81E)\"}] outputs:[{\"address\":\"0x6A3A060CE9D8E876E9B12DBE5D8F6A6864183BA1A63EA8CBCC14D668B5BECACF\",\"amount\":\"9.9996\",\"storestate\":false,\"tokenid\":\"0x00\"}] state:{\"0\":\"98\",\"1\":\"[MESSAGE]\"}\n"
				;
	}
	
	@Override
	public JSONObject runCommand() throws Exception {
		JSONObject ret = getJSONReply();
	
		//Get a list of the input coinID
		JSONArray inputs = getJSONArrayParam("inputs");
		
		//Get a list of output coins
		JSONArray outputs = getJSONArrayParam("outputs");
		
		//Is there a state
		JSONObject state = null;
		if(existsParam("state")) {
			state = getJSONObjectParam("state");
		}
		
		//ID of the custom transaction
		String randomid 	= MiniData.getRandomData(32).to0xString();
				
		//Now construct the transaction..
		JSONObject result 	= runCommand("txncreate", "id",randomid);
		try {
		
			//Add the Inputs
			for(Object input : inputs) {
				JSONObject in = (JSONObject)input;
			
				String coinid = in.getString("coinid");
				String script = in.getString("script");
			
				//Now add this coin..
				runCommand("txninput", "id",randomid, "coinid",coinid);
			
				//And add the script
				runCommand("txnscript", "id",randomid, "scripts",CommandRunner.scripts(script));
			}
		
			//Add the outputs
			for(Object output : outputs) {
				JSONObject out = (JSONObject)output;
			
				String address = out.getString("address");
				String amount  = out.getString("amount");
			
				//Is there a tokenid
				String tokenid = "0x00";
				if(out.containsKey("tokenid")) {
					tokenid = out.getString("tokenid");
				}
			
				//Is there a storestate
				boolean storestate = true;
				if(out.containsKey("storestate")) {
					storestate = out.getBoolean("storestate");
				}
			
				//Now add this coin..
				runCommand("txnoutput", "id",randomid, "address",address, "amount",amount, "tokenid",tokenid, "storestate",storestate);
			}
		
			//Add the state is exists
			if(state != null) {
			
				//Cycle..
				for(Object key : state.keySet()) {
				
					//The Key is a String
					String portstr = (String)key; 
				
					//The port
					int port = Integer.parseInt(portstr);
				
					//Get the state var..
					String var = state.get(key)+"";

					//Create a state variable..
					StateVariable sv = new StateVariable(port, var);
				
					//Add to the transaction..
					runCommand("txnstate", "id",randomid, "port",sv.getPort(), "value",sv.getData().toString());
				}
			}
		
			//Now export the txn..
			result = runCommand("txnexport", "id",randomid, "showtxn","true");
				
			//And delete..
			runCommand("txndelete", "id",randomid);
		
			//And return..
			ret.put("response", result.get("response"));
		
			return ret;
		}catch(Exception exc) {
			//A failed build never leaves its txn row behind (house rule: every txn error path runs txndelete)
			runCommand("txndelete", "id",randomid);
			throw exc;
		}
	}
	
	/**
	 * One step of the built transaction. SECURITY (2026-11-23): a failed step ends the build - it throws, and the
	 * catch around the build deletes the row. Before, a failed txninput / txnoutput / txnscript / txnstate / txnmmr /
	 * txnsign was ignored and the build went on to export or post what was left.
	 */
	public JSONObject runCommand(String zName, Object... zNameValues) throws CommandException {
		JSONObject result = CommandRunner.getRunner().runCommand(zName, zNameValues);
		if(!zName.equals("txndelete") && !Boolean.TRUE.equals(result.get("status"))) {
			throw new CommandException(String.valueOf(result.get("error")));
		}
		return result;
	}

	@Override
	public Command getFunction() {
		return new rawfrom();
	}	
}