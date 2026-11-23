package org.minima.system.commands.txn;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;

import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.userprefs.txndb.TxnRow;
import org.minima.objects.base.MiniData;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandException;
import org.minima.utils.MiniFile;
import org.minima.utils.json.JSONObject;

public class txnimport extends Command {

	public txnimport() {
		super("txnimport","(id:) (file:) (data:) - Import a transaction as a file or HEX data. Optionally specify the ID");
	}
	
	@Override
	public String getFullHelp() {
		return "\ntxnimport\n"
				+ "\n"
				+ "Import a transaction from previously exported HEX data or a .txn file.\n"
				+ "\n"
				+ "Optionally specify an ID to use for the new transaction.\n"
				+ "\n"
				+ "id: (optional)\n"
				+ "    Choose an ID for the transaction you are importing.\n"
				+ "\n"
				+ "file: (optional)\n"
				+ "    File name/path to the previously exported .txn file.\n"
				+ "\n"
				+ "data: (optional)\n"
				+ "    HEX data of the previously exported transaction.\n"
				+ "\n"
				+ "Examples:\n"
				+ "\n"
				+ "txnimport data:0x0000..\n"
				+ "\n"
				+ "txnimport id:simpletxn data:0x0000..\n"
				+ "\n"
				+ "txnimport id:multisig file:multisig.txn\n";
	}
	
	@Override
	public ArrayList<String> getValidParams(){
		return new ArrayList<>(Arrays.asList(new String[]{"id","file","data"}));
	}
	
	@Override
	public JSONObject runCommand() throws Exception {
		JSONObject ret = getJSONReply();

		TxnDB db = MinimaDB.getDB().getCustomTxnDB();
		
		if(existsParam("file")) {
			String file = getParam("file");
			File ff = MiniFile.createBaseFile(file);
			if(!ff.exists()) {
				throw new CommandException("File does not exist : "+ff.getAbsolutePath());
			}
			
			//Load it in..
			byte[] txndata = MiniFile.readCompleteFile(ff);
			
			//Convert to MiniData
			MiniData minitxn = new MiniData(txndata);
			
			//Convert this..
			TxnRow txnrow = TxnRow.convertMiniDataVersion(minitxn);
			if(existsParam("id")) {
				txnrow.setID(getParam("id"));
			}
			
			addRow(db, txnrow);
			
			JSONObject resp = new JSONObject();
			ret.put("response", txnrow.toJSON());
			
		}else if(existsParam("data")){
			
			//Get the HEX data
			MiniData dv = getDataParam("data");
			
			//Convert to a TxnRow
			TxnRow tx 	= TxnRow.convertMiniDataVersion(dv);
			if(existsParam("id")) {
				tx.setID(getParam("id"));
			}
			
			//Add to the DB
			addRow(db, tx);
			
			JSONObject resp = new JSONObject();
			ret.put("response", tx.toJSON());
			
		}else {
			throw new CommandException("Must specify file or data");
		}
		
		return ret;
	}

	/**
	 * CommandRunner lets a non-admin txnimport through only for a NEW id; the add must not then replace a row
	 * created in between - checked and added in one step. ADMIN: as before.
	 */
	private void addRow(TxnDB zDB, TxnRow zRow) throws CommandException {
		if(isAdminCaller()) {
			zDB.addCompleteTransaction(zRow);
		}else if(!zDB.addNewTransaction(zRow)) {
			throw new CommandException("A transaction with this id already exists : "+zRow.getID());
		}
	}
	
	@Override
	public Command getFunction() {
		return new txnimport();
	}

}
