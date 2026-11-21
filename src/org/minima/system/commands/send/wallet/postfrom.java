package org.minima.system.commands.send.wallet;

import java.util.ArrayList;
import java.util.Arrays;

import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.userprefs.txndb.TxnRow;
import org.minima.objects.base.MiniData;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

public class postfrom extends Command {

	public postfrom() {
		super("postfrom","[data:] (mine:true|false) (mmr:true|false) - Post a signfrom txn ");
	}
	
	@Override
	public ArrayList<String> getValidParams(){
		return new ArrayList<>(Arrays.asList(new String[]{"data","mine","mmr"}));
	}
	
	@Override
	public JSONObject runCommand() throws Exception {
		JSONObject ret = getJSONReply();
	
		TxnDB db = MinimaDB.getDB().getCustomTxnDB();
		
		//Get the HEX data
		MiniData dv = getDataParam("data");
		
		//Convert to a TxnRow
		TxnRow tx 	= TxnRow.convertMiniDataVersion(dv);
		if(existsParam("id")) {
			tx.setID(getParam("id"));
		}
		
		boolean sortmmr = getBooleanParam("mmr", false);
		
		String randomid = tx.getID();
		
		//Add to the DB
		db.addCompleteTransaction(tx);
		try {
		
			//Are we mining
			boolean mine = getBooleanParam("mine", false);
		
			//Do we sort the MMR
			if(sortmmr) {
				runCommand("txnmmr", "id",randomid);
			}
		
			//And POST!
			JSONObject result = runCommand("txnpost", "id",randomid, "mine",mine);
			
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
	
	public JSONObject runCommand(String zName, Object... zNameValues) {
		return CommandRunner.getRunner().runCommand(zName, zNameValues);
	}

	@Override
	public Command getFunction() {
		return new postfrom();
	}	
}