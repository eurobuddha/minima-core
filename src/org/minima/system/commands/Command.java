package org.minima.system.commands;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.minima.objects.Address;
import org.minima.objects.base.MiniData;
import org.minima.objects.base.MiniNumber;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MiniFile;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

public abstract class Command {
	
	String mName;
	
	String mHelp;
	
	JSONObject mParams = new JSONObject();
	
	String mCompleteCommand = new String("");
	
	String mMiniDAPPID = "";
	
	/** Set by CommandRunner: this caller passed the WRITE gate (ADMIN, or an MDS MiniDapp with WRITE permission) */
	boolean mWriteAllowed = false;
	
	/** Files a non-admin caller claimed (getNewFileParam) - removed again if the command fails (releaseClaims) */
	ArrayList<File> mClaimed = new ArrayList<>();
	
	public Command(String zName, String zHelp) {
		mName = zName;
		mHelp = zHelp;
	}
	
	public void setMiniDAPPID(String zMiniDAPPID) {
		mMiniDAPPID = zMiniDAPPID;
	}
	
	public String getMiniDAPPID() {
		return mMiniDAPPID;
	}
	
	public void setWriteAllowed(boolean zWriteAllowed) {
		mWriteAllowed = zWriteAllowed;
	}
	
	/**
	 * May this caller use the node's own keys and files - ADMIN (0x00: terminal, RPC minima / write users, admin
	 * companions, an accepted MDS pending command), or a caller CommandRunner let through its WRITE gate (an MDS
	 * MiniDapp with WRITE permission)?
	 */
	public boolean isAdminCaller() {
		return mWriteAllowed || mMiniDAPPID.equals("0x00");
	}
	
	/**
	 * Does THIS use of the command need WRITE rights although the command itself does not? Returns why, or null.
	 * CommandRunner then treats it as a WRITE command: refused for a non-admin caller, pending for an MDS
	 * MiniDapp with READ permission, run for ADMIN / a WRITE MiniDapp. The command's own checks must match.
	 * Default: a file: that is not a plain .txn name (see getFileParam).
	 */
	public String getWriteUse() {
		Object file = mParams.get("file");
		if(file instanceof String && !isPlainTxnName(((String) file).trim())) {
			return "a non-admin caller can only use files in the node's base folder, by a plain name ending in .txn";
		}
		return null;
	}
	
	public void setCompleteCommand(String zCommand) {
		mCompleteCommand = zCommand;
	}
	
	public ArrayList<String> getValidParams(){
		return new ArrayList<>();
	}
	
	public String getCompleteCommand() {
		return mCompleteCommand;
	}
	
	public String getHelp() {
		return mHelp;
	}
	
	public String getFullHelp() {
		return mHelp;
	}
	
	public JSONObject getJSONReply() {
		JSONObject json = new JSONObject();
		json.put("command", getName());
		
		//Are they empty..
		if(!getParams().isEmpty()) {
			json.put("params", getParams());
		}
		
		json.put("status", true);
		json.put("pending", false);
		
		return json;
	}
	
	public String getName() {
		return mName;
	}
	
	public JSONObject getParams() {
		return mParams;
	}
	
	public boolean existsParam(String zParamName) {
		return mParams.containsKey(zParamName);
	}
	
	public String getParam(String zParamName) throws CommandException {
		if(!existsParam(zParamName)) {
			throw new CommandException("param not specified : "+zParamName);
		}
		
		//Check for blank
		String pp = (String)mParams.get(zParamName);
		pp = pp.trim();
		if(pp.equals("")) {
			throw new CommandException("BLANK param not allowed : "+zParamName);
		}
		
		return pp;
	}
	
	public String getParam(String zParamName, String zDefault) throws CommandException {
		if(existsParam(zParamName)) {
			//Check for blank
			String pp = (String)mParams.get(zParamName);
			pp = pp.trim();
			if(pp.equals("")) {
				throw new CommandException("BLANK param not allowed : "+zParamName);
			}
			
			return pp;
		}
		
		return zDefault;
	}
	
	/**
	 * A file: param as a File. An ADMIN caller: MiniFile.createBaseFile, any path, as before.
	 * 
	 * SECURITY (reviews 2026-11-21): a non-admin caller may only name a transaction file - a PLAIN name ending in
	 * .txn (letters, digits, '.', '_', '-'; no path separator of either kind, no '..', no drive letter) - and it
	 * is always the file of that name in the base folder. In every shipped product the base folder IS the node's
	 * data folder (wallet database under 1.1/, Parlons keys and tokens beside it), and none of the node's own files
	 * is a .txn, so a non-admin name can never reach one. No base folder set, or a base folder at the filesystem
	 * root: refused. A .txn that exists as a folder: refused. Rewriting the caller's own .txn stays possible (the
	 * shipped send / view / sign / post flows re-write handle.txn on a retry), and ADMIN steps of the same flow
	 * resolve the same plain name to the same file.
	 */
	public File getFileParam(String zParamName) throws CommandException {
		return confinedFile(getParam(zParamName));
	}
	
	public File getFileParam(String zParamName, String zDefault) throws CommandException {
		return confinedFile(getParam(zParamName, zDefault));
	}
	
	/**
	 * A file: the command WRITES. SECURITY (final review 2026-11-21): a non-admin caller only CREATES a new file
	 * (claimed atomically) - it can never overwrite one, e.g. an admin flow's unsigned handle.txn between its
	 * check and its sign. ADMIN: as before (getFileParam, overwrite allowed).
	 */
	public File getNewFileParam(String zParamName) throws CommandException {
		return claimNew(getFileParam(zParamName));
	}
	
	public File getNewFileParam(String zParamName, String zDefault) throws CommandException {
		return claimNew(getFileParam(zParamName, zDefault));
	}
	
	private File claimNew(File zFile) throws CommandException {
		if(!isAdminCaller()) {
			try {
				if(!zFile.createNewFile()) {
					throw new CommandException("A non-admin caller only creates new files - "+zFile.getName()+" exists");
				}
				mClaimed.add(zFile);
			}catch(IOException exc) {
				throw new CommandException("Cannot create "+zFile.getName()+" : "+exc.getMessage());
			}
		}
		return zFile;
	}
	
	/** A failed command: remove the files this non-admin caller claimed, so a retry under the same name is not refused */
	public void releaseClaims() {
		for(File claimed : mClaimed) {
			claimed.delete();
		}
		mClaimed.clear();
	}
	
	/** The file: param as given, or null */
	protected String getFileName() {
		Object file = mParams.get("file");
		return file instanceof String ? ((String) file).trim() : null;
	}
	
	/** For getWriteUse of a writing command: overwriting an existing plain .txn needs WRITE rights */
	protected String overwriteWriteUse(String zName) {
		if(zName != null && isPlainTxnName(zName) && !GeneralParams.BASE_FILE_FOLDER.equals("")
				&& new File(GeneralParams.BASE_FILE_FOLDER, zName).exists()) {
			return "a non-admin caller only creates new files - "+zName+" exists";
		}
		return null;
	}
	
	/** A plain transaction file name a non-admin caller may use */
	public static boolean isPlainTxnName(String zName) {
		return zName.length() <= 128 && zName.matches("[A-Za-z0-9][A-Za-z0-9._-]*\\.txn") && !zName.contains("..")
				//Windows device names (CON.txn, com1.txn..) open the device, not a file
				&& !zName.matches("(?i)(CON|PRN|AUX|NUL|COM[0-9]|LPT[0-9])(\\..*)?");
	}
	
	private File confinedFile(String zName) throws CommandException {
		if(isAdminCaller()) {
			return MiniFile.createBaseFile(zName);
		}
		
		if(!isPlainTxnName(zName)) {
			throw new CommandException("A non-admin caller can only use files in the node's base folder, "
					+ "by a plain name ending in .txn : "+zName);
		}
		
		try {
			if(GeneralParams.BASE_FILE_FOLDER.equals("")) {
				throw new CommandException("A non-admin caller can only use files in the node's base folder - none is set");
			}
			File base = new File(GeneralParams.BASE_FILE_FOLDER).getCanonicalFile();
			if(base.getParentFile() == null) {
				throw new CommandException("A non-admin caller can only use files in the node's base folder - not the filesystem root");
			}
			File file = new File(base, zName);
			if(file.isDirectory()) {
				throw new CommandException("A non-admin caller can only use files in the node's base folder - "+zName+" is a folder");
			}
			return file;
		}catch(IOException exc) {
			throw new CommandException("Invalid file : "+zName);
		}
	}
	
	public boolean getBooleanParam(String zParamName) throws CommandException {
		String bool = getParam(zParamName);
		if(bool.equals("true")){
			return  true;
		}
		return false;
	}
	
	public boolean getBooleanParam(String zParamName, boolean zDefault) throws CommandException {
		if(existsParam(zParamName)) {
			if(getParam(zParamName).equals("true")){
				return  true;
			}else {
				return false;
			}
		}
		
		return zDefault;
	}
	
	/**
	 * A key-use count: the index of the NEXT one-time (Winternitz) leaf a key signs with, so valid values are the
	 * whole numbers 0 .. zMaxUses-1. SECURITY (2026-11-21): compared as a MiniNumber BEFORE any int conversion -
	 * getAsInt keeps the low 32 bits (4294967299 became 3) and TreeKey.sign restarts at leaf 0 at or past the
	 * capacity, both of which re-sign with a used leaf and leak the key. Anything else is refused.
	 */
	/**
	 * OWNER MAY DROP (round-5 review 2026-11-21): the node wallet's keys are default TreeKeys - 64 x 64 x 64 one-time
	 * leaves. A wallet-wide keyuses: (vault resetkeys, archive / megammrsync / mysql resync, keys createallkeys)
	 * at or past this would put every key at or past its last leaf (restore and restoresync ADD, capped per key).
	 */
	public static final long WALLET_KEY_CAPACITY = (long) Math.pow(org.minima.objects.keys.TreeKey.DEFAULT_KEYSPERLEVEL,
			org.minima.objects.keys.TreeKey.DEFAULT_LEVELS);
	
	public int getKeyUsesParam(String zParamName, MiniNumber zDefault, long zMaxUses) throws CommandException {
		MiniNumber uses = zDefault == null ? getNumberParam(zParamName) : getNumberParam(zParamName, zDefault);
		if(!uses.floor().isEqual(uses) || uses.isLess(MiniNumber.ZERO) || uses.isMoreEqual(new MiniNumber(zMaxUses))) {
			throw new CommandException(zParamName+" must be a whole number from 0 to "+(zMaxUses-1)
					+" (the next unused leaf of the key) : "+uses);
		}
		return uses.getAsInt();
	}
	
	public MiniNumber getNumberParam(String zParamName) throws CommandException {
		String num = getParam(zParamName);
		return new MiniNumber(num);
	}
	
	public MiniNumber getNumberParam(String zParamName, MiniNumber zDefault) throws CommandException {
		if(existsParam(zParamName)) {
			return getNumberParam(zParamName);
		}
		return zDefault;
	}
	
	
	public MiniData getDataParam(String zParamName, MiniData zDefault) throws CommandException {
		if(existsParam(zParamName)){
			return getDataParam(zParamName);
		}
		
		return zDefault;
	}
	
	public MiniData getDataParam(String zParamName) throws CommandException {
		String hex = getParam(zParamName);
		
		try {
			if(hex.toLowerCase().startsWith("mx")) {
				//Convert back to normal hex..
				String datahex = Address.convertMinimaAddress(hex).to0xString();
				return new MiniData(datahex);
			}
			
			//Else it's normal HEX
			return new MiniData(hex);
			
		}catch(Exception exc) {
			throw new CommandException("Invalid Data param specified : "+zParamName+" "+hex);
		}
	}
	
	public JSONObject getJSONObjectParam(String zParamName) throws CommandException{
		if(!existsParam(zParamName)) {
			throw new CommandException("param not specified : "+zParamName);
		}
		
		return (JSONObject) mParams.get(zParamName);
	}
	
	public JSONObject getJSONObjectParam(String zParamName, JSONObject zDefault) throws CommandException{
		if(!existsParam(zParamName)) {
			return zDefault;
		}
		
		return (JSONObject) mParams.get(zParamName);
	}
	
	public JSONArray getJSONArrayParam(String zParamName) throws CommandException {
		if(!existsParam(zParamName)) {
			throw new CommandException("param not specified : "+zParamName);
		}
		
		return (JSONArray) mParams.get(zParamName);
	}
	
	public String getAddressParam(String zParamName, String zDefault) throws CommandException {
		if(existsParam(zParamName)) {
			return getAddressParam(zParamName);
		}
		
		return zDefault;
	}
	
	public String getAddressParam(String zParamName) throws CommandException {
		if(!existsParam(zParamName)) {
			throw new CommandException("param not specified : "+zParamName);
		}

		String address = getParam(zParamName);
		if(address.toLowerCase().startsWith("mx")) {
			//Convert back to normal hex..
			try {
				address = Address.convertMinimaAddress(address).to0xString();
			}catch(IllegalArgumentException exc) {
				throw new CommandException(exc.toString());
			}
		}
		
		//If it's an 0x address check converts to MiniData correctly
		if(address.startsWith("0x")) {
			try {
				MiniData data 	= new MiniData(address);
				address 		= data.to0xString();
			}catch(Exception exc) {
				throw new CommandException(exc.toString());
			}
		}
		
		return address;
	}
	
	public boolean isParamJSONObject(String zParamName) {
		if(existsParam(zParamName)) {
			Object obj = mParams.get(zParamName);
			if(obj instanceof JSONObject) {
				return true;
			}
		}
		
		return false;
	}
	
	public boolean isParamJSONArray(String zParamName) {
		if(existsParam(zParamName)) {
			Object obj = mParams.get(zParamName);
			if(obj instanceof JSONArray) {
				return true;
			}
		}
		
		return false;
	}
	
	/**
	 * These function need to be implemented..
	 */
	
	public abstract JSONObject runCommand() throws Exception;
	
	public abstract Command getFunction();	
}
