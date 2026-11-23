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
	public String getWriteUse() {
		String use = super.getWriteUse();
		if(use != null || isAdminCaller()) {
			return use;
		}
		
		//SECURITY (2026-11-23): the row it adds REPLACES one with the same id - overwriting another caller's draft
		//(between its import and its txnsign) needs WRITE rights, as for postfrom / signfrom
		try {
			String id = readRow().getID();
			if(MinimaDB.getDB().getCustomTxnDB().getTransactionRow(id) != null) {
				return "a transaction with this id already exists : "+id;
			}
		}catch(Exception exc) {
			//no file, invalid data.. fails in the command itself
		}
		return null;
	}
	
	@Override
	public JSONObject runCommand() throws Exception {
		JSONObject ret = getJSONReply();

		TxnDB db = MinimaDB.getDB().getCustomTxnDB();
		
		TxnRow txnrow = readRow();
		
		//Add to the DB
		addRow(db, txnrow);
		
		ret.put("response", txnrow.toJSON());
		
		return ret;
	}
	
	/** The row to import - from file: or data:, under id: if given */
	private TxnRow readRow() throws Exception {
		TxnRow txnrow;
		if(existsParam("file")) {
			File ff = getFileParam("file");
			if(!ff.exists()) {
				throw new CommandException("File does not exist : "+ff.getAbsolutePath());
			}
			
			//Load it in.. and convert
			txnrow = TxnRow.convertMiniDataVersion(new MiniData(MiniFile.readCompleteFile(ff)));
			
		}else if(existsParam("data")){
			
			//Convert the HEX data to a TxnRow
			txnrow = TxnRow.convertMiniDataVersion(getDataParam("data"));
			
		}else {
			throw new CommandException("Must specify file or data");
		}
		
		if(existsParam("id")) {
			txnrow.setID(getParam("id"));
		}
		return txnrow;
	}

	/**
	 * SECURITY (final review 2026-11-21): CommandRunner lets a non-admin txnimport through only for a NEW id; the
	 * add must not then replace a row created in between - checked and added in one step. ADMIN: as before.
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
