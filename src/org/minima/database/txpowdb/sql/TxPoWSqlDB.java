package org.minima.database.txpowdb.sql;

import java.io.File;
import java.io.IOException;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

import org.minima.objects.TxPoW;
import org.minima.objects.base.MiniByte;
import org.minima.objects.base.MiniData;
import org.minima.objects.base.MiniNumber;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MinimaLogger;
import org.minima.utils.SqlDB;

public class TxPoWSqlDB extends SqlDB {

	/**
	 * How long does data remain the SQL DB in milli seconds
	 */
	public static long MAX_SQL_MILLI = 1000 * 60 * 60 * 24 * GeneralParams.NUMBER_DAYS_SQLTXPOWDB;
	
	/**
	 * The default MAX Relevant TxPoW
	 */
	public static MiniNumber MAX_RELEVANT_TXPOW = MiniNumber.HUNDRED;
	
	/**
	 * Prepared SQL statements
	 */
	PreparedStatement SQL_DELETE_RELEVANT_TXPOW = null;
	PreparedStatement SQL_INSERT_TXPOW 		= null;
	PreparedStatement SQL_SELECT_TXPOW 		= null;
	PreparedStatement SQL_SELECT_CHILDREN 	= null;
	PreparedStatement SQL_TOTAL_TXPOW 		= null;
	PreparedStatement SQL_DELETE_TXPOW 		= null;
	PreparedStatement SQL_EXISTS 			= null;
	
	PreparedStatement SQL_SELECT_TOPTXPOW 		= null;
	PreparedStatement SQL_SELECT_TOPTXPOW_SIZE 	= null;
	
	PreparedStatement SQL_SELECT_RELEVANT 		= null;
	PreparedStatement SQL_SELECT_RELEVANTSIZE 	= null;
	
	PreparedStatement SQL_SELECT_OLDEST 		= null;
	
	/**
	 * Show when a new block is added
	 */
	public static boolean LOG_ADD_BLOCKS = false;
	
	/**
	 * Show when a new block is added
	 */
	public static boolean LOG_ADD_TRANSACTION = false;
	
	public TxPoWSqlDB() {
		super();
	}
		
	/**
	 * Perform the Create SQL
	 * @throws SQLException 
	 */
	protected void createSQL() throws SQLException {
		
			//Create the various tables..
			Statement stmt = mSQLConnection.createStatement();
			
			//Create main table
			String create = "CREATE TABLE IF NOT EXISTS `txpow` ("
							+ "  `id` bigint auto_increment,"
							+ "  `txpowid` varchar(80) NOT NULL UNIQUE,"
							+ "  `isblock` tinyint NOT NULL,"
							+ "  `istransaction` tinyint NOT NULL,"
							+ "  `parentid` varchar(80) NOT NULL,"
							+ "  `timemilli` bigint NOT NULL,"
							+ "  `txpowdata` blob NOT NULL,"
							+ "  `isrelevant` tinyint NOT NULL"
							+ ")";
			
			//Run it..
			stmt.execute(create);
			
			//Create some fast indexes..
			String index = "CREATE INDEX IF NOT EXISTS fastsearch ON txpow ( txpowid, parentid )";

			//Run it..
			stmt.execute(index);

			//timemilli was unindexed, so every ORDER BY / range scan below was a full table scan
			//that dragged each row's txpowdata BLOB through the sort. All these methods are
			//synchronized on this instance, so one 'history' call stalled every other node DB
			//read - including the single-threaded IPC command executor. Measured 2026-09-11 on a
			//500MB+ txpow store: 47s monitor holds against a 30s companion read timeout.
			//Serves getLatestTxPoW's ORDER BY, getLatestTxPoWSize's COUNT, and the cleanup DELETE.
			String indextime = "CREATE INDEX IF NOT EXISTS txpowtime ON txpow ( timemilli )";
			stmt.execute(indextime);

			//Composite, isrelevant first: serves getAllRelevant's WHERE isrelevant=1 ORDER BY
			//timemilli DESC, getRelevantSize's COUNT, and the isrelevant=0 half of the DELETE.
			String indexrelevant = "CREATE INDEX IF NOT EXISTS txpowrelevant ON txpow ( isrelevant, timemilli )";
			stmt.execute(indexrelevant);

			//All done..
			stmt.close();
			
			//Create some prepared statements..
			String insert 		= "INSERT IGNORE INTO txpow ( txpowid, isblock, istransaction, parentid, timemilli, txpowdata, isrelevant ) VALUES ( ?, ? ,? ,? ,? ,? ,? )";
			SQL_INSERT_TXPOW 	= mSQLConnection.prepareStatement(insert);
			
			//Select 
			SQL_SELECT_TXPOW 	= mSQLConnection.prepareStatement("SELECT txpowdata FROM txpow WHERE txpowid=?");
			SQL_SELECT_CHILDREN	= mSQLConnection.prepareStatement("SELECT txpowid FROM txpow WHERE isblock=1 AND parentid=?");
			SQL_TOTAL_TXPOW		= mSQLConnection.prepareStatement("SELECT COUNT(*) AS tot FROM txpow");
			SQL_DELETE_TXPOW	= mSQLConnection.prepareStatement("DELETE FROM txpow WHERE timemilli < ? AND isrelevant=0");
			
			//A relevant row is the user's own history, so it gets its OWN (longer) window rather
			//than the 3 days ordinary traffic gets - but it does now get one. Before this, the
			//statement above was the only DELETE on this table and its `AND isrelevant=0` meant a
			//relevant row was immortal: the routine clean skipped it, cleanDB(true) ("wipe clean")
			//skipped it, and a restart does not touch disk. A rolling window grew without bound -
			//583 MB measured on a phone 2026-09-22 whose whole node process has a 512 MB ceiling.
			//Served by the existing txpowrelevant ( isrelevant, timemilli ) index, so no migration.
			SQL_DELETE_RELEVANT_TXPOW = mSQLConnection.prepareStatement("DELETE FROM txpow WHERE timemilli < ? AND isrelevant=1");
			SQL_EXISTS			= mSQLConnection.prepareStatement("SELECT txpowid FROM txpow WHERE txpowid=?");
		
			SQL_SELECT_RELEVANT 	= mSQLConnection.prepareStatement("SELECT * FROM txpow WHERE isrelevant=1 ORDER BY timemilli DESC LIMIT ? OFFSET ?");
			SQL_SELECT_RELEVANTSIZE = mSQLConnection.prepareStatement("SELECT Count(*) AS tot FROM txpow WHERE isrelevant=1");
			
			//The `timemilli > 0` looks redundant - every row has a real epoch time - but it is
			//LOAD BEARING and must not be "tidied away". H2 will not use an index for a bare
			//ORDER BY .. LIMIT with no WHERE, so without it this stays a full table scan that
			//sorts every row's txpowdata BLOB. Measured on H2 2.1.214 (the version the APK
			//ships) with 60k rows: 25,535 page reads as a tableScan versus 3,755 using
			//txpowtime - and the gap widens with the table, because the scan is O(rows).
			SQL_SELECT_TOPTXPOW 	 = mSQLConnection.prepareStatement("SELECT * FROM txpow WHERE timemilli > 0 ORDER BY timemilli DESC LIMIT ? OFFSET ?");
			SQL_SELECT_TOPTXPOW_SIZE = mSQLConnection.prepareStatement("SELECT Count(*) AS tot FROM txpow WHERE timemilli > ?");
	}
	
	public synchronized void wipeDB() throws SQLException {
		
		//Make sure..
		checkOpen();
		
		//One last statement
		Statement stmt = mSQLConnection.createStatement();
	
		//First wipe everything..
		stmt.execute("DROP ALL OBJECTS");
		
		//Create main table
		String create = "CREATE TABLE IF NOT EXISTS `txpow` ("
						+ "  `id` bigint auto_increment,"
						+ "  `txpowid` varchar(80) NOT NULL UNIQUE,"
						+ "  `isblock` tinyint NOT NULL,"
						+ "  `istransaction` tinyint NOT NULL,"
						+ "  `parentid` varchar(80) NOT NULL,"
						+ "  `timemilli` bigint NOT NULL,"
						+ "  `txpowdata` blob NOT NULL,"
						+ "  `isrelevant` tinyint NOT NULL"
						+ ")";
		
		//Run it..
		stmt.execute(create);
		
		//That's it..
		stmt.close();
	}
	
	public synchronized int customSizeQuery(String zWhereCondition) {
		try {
			
			//Make sure..
			checkOpen();
			
			//Sanitize
			String where = zWhereCondition.toLowerCase().replaceAll(";", "");
			where = where.replaceAll("update", "");
			where = where.replaceAll("delete", "");
			where = where.replaceAll("insert", "");
			
			//Create the SQL
			String sql = "SELECT Count(*) AS tot FROM txpow WHERE "+where;
			
			//Create the various tables..
			Statement stmt = mSQLConnection.createStatement();
			
			//Execute the SQL..
			boolean res = stmt.execute(sql);
			
			//Get the results
			ResultSet rs = stmt.getResultSet();
			
			//Could be multiple results
			if(rs.next()) {
				//Get the total numer of rows
				return rs.getInt("tot");
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return -1;
	}
	
	public synchronized boolean addTxPoW(TxPoW zTxPoW, boolean zIsRelevant) {
		try {
			
			if(LOG_ADD_BLOCKS) {
				if(zTxPoW.isBlock()) {
					MinimaLogger.log("[+] BLOCK ADDED TO TXPOWDB : "+zTxPoW.getBlockNumber());
				}
			}
			
			if(LOG_ADD_TRANSACTION) {
				if(zTxPoW.isTransaction()) {
					MinimaLogger.log("[+] TRANSACTION ADDED TO TXPOWDB : "+zTxPoW.getBlockNumber()+" - "+zTxPoW.getTransaction().toJSON().toString());
				}
			}
			
			//Make sure..
			checkOpen();
			
			//get the MiniData version..
			MiniData txdata = MiniData.getMiniDataVersion(zTxPoW);
			
			//Get the Query ready
			SQL_INSERT_TXPOW.clearParameters();
		
			//Set main params
			SQL_INSERT_TXPOW.setString(1, zTxPoW.getTxPoWID());
			SQL_INSERT_TXPOW.setInt(2, new MiniByte(zTxPoW.isBlock()).getValue());
			SQL_INSERT_TXPOW.setInt(3, new MiniByte(zTxPoW.isTransaction()).getValue());
			SQL_INSERT_TXPOW.setString(4, zTxPoW.getParentID().to0xString());
			SQL_INSERT_TXPOW.setLong(5, System.currentTimeMillis());
			
			//The actual bytes
			SQL_INSERT_TXPOW.setBytes(6, txdata.getBytes());
			
			//Is it relevant
			if(zIsRelevant) {
				SQL_INSERT_TXPOW.setInt(7, 1);
			}else {
				SQL_INSERT_TXPOW.setInt(7, 0);
			}
			
			//Do it.
			SQL_INSERT_TXPOW.execute();
			
			return true;
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return false;
	}

	public synchronized TxPoW getTxPoW(String zTxPoWID) {
		try {
			
			//Make sure..
			checkOpen();
			
			//Get the query ready
			SQL_SELECT_TXPOW.clearParameters();
			
			//Set the txpowid we are searching for
			SQL_SELECT_TXPOW.setString(1, zTxPoWID);
		
			//Run the query
			ResultSet rs = SQL_SELECT_TXPOW.executeQuery();
			
			//Is there a result..
			if(rs.next()) {
				
				//Get the blob of data
				byte[] txpdata 	= rs.getBytes("txpowdata");
				
				//Create MiniData version
				MiniData minitxp = new MiniData(txpdata);
				
				//Convert into a TxPoW..
				TxPoW txpow = TxPoW.convertMiniDataVersion(minitxp);
				
				return txpow;
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return null;
	}
	
	public synchronized ArrayList<String> getChildBlocks(String zParentTxPoWID) {
		ArrayList<String> txpows = new ArrayList<>();

		try {
			//Make sure..
			checkOpen();
			
			//Get the query ready
			SQL_SELECT_CHILDREN.clearParameters();
			
			//Set the txpowid we are seraching for
			SQL_SELECT_CHILDREN.setString(1, zParentTxPoWID);
		
			//Run the query
			ResultSet rs = SQL_SELECT_CHILDREN.executeQuery();
			
			//Could be multiple results
			while(rs.next()) {
				
				//Get the txpowid
				String txpowid = rs.getString("txpowid");
				
				//Add to our list
				txpows.add(txpowid);
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return txpows;
	}
	
	public synchronized int getRelevantSize() {
		try {
			//Make sure..
			checkOpen();
			
			//Run the query
			ResultSet rs = SQL_SELECT_RELEVANTSIZE.executeQuery();
			
			//Could be multiple results
			if(rs.next()) {
				//Get the total numer of rows
				return rs.getInt("tot");
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		//Error has occurred
		return -1;
	}
	
	public ArrayList<TxPoW> getAllRelevant(int zLimit) {
		return getAllRelevant(zLimit, 0);
	}
	
	public synchronized ArrayList<TxPoW> getAllRelevant(int zLimit, int zOffset) {
		ArrayList<TxPoW> txpows = new ArrayList<>();

		try {
			
			//Make sure..
			checkOpen();
			
			//Get the query ready
			SQL_SELECT_RELEVANT.clearParameters();
			
			//Set the Limit..
			SQL_SELECT_RELEVANT.setInt(1, zLimit);
			SQL_SELECT_RELEVANT.setInt(2, zOffset);
			
			//Run the query
			ResultSet rs = SQL_SELECT_RELEVANT.executeQuery();
			
			//Could be multiple results
			while(rs.next()) {
				
				//Get the blob of data
				byte[] txpdata 	= rs.getBytes("txpowdata");
				
				//Create MiniData version
				MiniData minitxp = new MiniData(txpdata);
				
				//Convert into a TxPoW..
				TxPoW txpow = TxPoW.convertMiniDataVersion(minitxp);
				
				//Add to our list
				txpows.add(txpow);
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return txpows;
	}

	public synchronized ArrayList<TxPoW> getLatestTxPoW(int zLimit, int zOffset) {
		ArrayList<TxPoW> txpows = new ArrayList<>();

		try {
			
			//Make sure..
			checkOpen();
			
			//Get the query ready
			SQL_SELECT_TOPTXPOW.clearParameters();
			
			//Set the Limit..
			SQL_SELECT_TOPTXPOW.setInt(1, zLimit);
			SQL_SELECT_TOPTXPOW.setInt(2, zOffset);
			
			//Run the query
			ResultSet rs = SQL_SELECT_TOPTXPOW.executeQuery();
			
			//Could be multiple results
			while(rs.next()) {
				
				//Get the blob of data
				byte[] txpdata 	= rs.getBytes("txpowdata");
				
				//Create MiniData version
				MiniData minitxp = new MiniData(txpdata);
				
				//Convert into a TxPoW..
				TxPoW txpow = TxPoW.convertMiniDataVersion(minitxp);
				
				//Add to our list
				txpows.add(txpow);
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return txpows;
	}
	
	public synchronized int getLatestTxPoWSize(long zMinMilliTime) {
		try {
			//Make sure..
			checkOpen();
			
			//Get the query ready
			SQL_SELECT_TOPTXPOW_SIZE.clearParameters();
			SQL_SELECT_TOPTXPOW_SIZE.setLong(1, zMinMilliTime);
			
			//Run the query
			ResultSet rs = SQL_SELECT_TOPTXPOW_SIZE.executeQuery();
			
			//Could be multiple results
			if(rs.next()) {
				//Get the total numer of rows
				return rs.getInt("tot");
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		//Error has occurred
		return -1;
	}
	
	public synchronized int getSize() {
		try {
			//Make sure..
			checkOpen();
			
			//Run the query
			ResultSet rs = SQL_TOTAL_TXPOW.executeQuery();
			
			//Could be multiple results
			if(rs.next()) {
				//Get the total numer of rows
				return rs.getInt("tot");
			}
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		//Error has occurred
		return -1;
	}

	public synchronized boolean exists(String zTxPoWID) {
		try {
			
			//Make sure..
			checkOpen();
			
			//Set the params..
			SQL_EXISTS.clearParameters();
			SQL_EXISTS.setString(1, zTxPoWID);
			
			//Run the query
			ResultSet rs = SQL_EXISTS.executeQuery();
			
			//Is there a row
			return rs.next();
			
		} catch (SQLException e) {
			//MinimaLogger.log(e);
		}
		
		return false;
	}
	
	/**
	 * Age out RELEVANT rows - the user's own transaction history - on their own window.
	 * 
	 * Until this existed the only DELETE on this table carried `AND isrelevant=0`, so a row once
	 * marked relevant could never be removed by anything: not the routine clean, not
	 * cleanDB(true), not a restart (the rows are on disk, a restart only resets the heap). A
	 * table whose whole design is a rolling window therefore grew without bound. Measured on a
	 * phone 2026-09-22: 583 MB, in a node process capped at 512 MB of Dalvik heap, with the
	 * oversized relevant set also inflating TxPoWTreeNode.mComputedRelevantCoins and every
	 * cascade's copyParentRelevantCoins() deep-copy.
	 * 
	 * The window is read from GeneralParams at CALL time, not cached in a static like
	 * MAX_SQL_MILLI - that field is initialised at class load, which can be before
	 * ParamConfigurer has parsed -txpowdbstore, so a configured value can be silently ignored.
	 * Do not "tidy" this into a static.
	 * 
	 * @return how many rows were deleted
	 */
	private synchronized int cleanRelevant() throws SQLException {
		
		//Never retire relevant rows sooner than ordinary traffic, whatever the params say
		long days = Math.max(GeneralParams.NUMBER_DAYS_RELEVANT_SQLTXPOWDB,
							 GeneralParams.NUMBER_DAYS_SQLTXPOWDB);
		
		long maxtime = System.currentTimeMillis() - (1000L * 60 * 60 * 24 * days);
		
		SQL_DELETE_RELEVANT_TXPOW.clearParameters();
		SQL_DELETE_RELEVANT_TXPOW.setLong(1, maxtime);
		
		int num = SQL_DELETE_RELEVANT_TXPOW.executeUpdate();
		if(LOG_ADD_BLOCKS && num>0) {
			MinimaLogger.log("[+] TxPoWDB relevant clean up deleted : "+num+" (older than "+days+" days)");
		}
		
		return num;
	}
	
	/**
	 * Returns how many rows were deleted
	 */
	public synchronized int cleanDB() {
		return cleanDB(false);
	}
	
	public synchronized int cleanDB(boolean zHard) {
		try {
			
			//Make sure..
			checkOpen();
			
			//Current MAX time..
			long maxtime = System.currentTimeMillis() - MAX_SQL_MILLI;
			if(zHard) {
				
				if(LOG_ADD_BLOCKS) {
					MinimaLogger.log("[+] TxPoWDB wipe clean..");
				}
				
				maxtime = System.currentTimeMillis() + 100000;
			}
			
			//Set the parameters
			SQL_DELETE_TXPOW.clearParameters();
			
			//Set the time milli
			SQL_DELETE_TXPOW.setLong(1, maxtime);
			
			//Run the query
			int num = SQL_DELETE_TXPOW.executeUpdate();
			if(LOG_ADD_BLOCKS) {
				MinimaLogger.log("[+] TxPoWDB clean up deleted : "+num);
			}
			
			//And now the RELEVANT rows, on their own longer window.
			//
			//Deliberately NOT driven by zHard. A hard clean is a memory-pressure sweep
			//(MinimaDB.fullDBRestartMemFree) and it drops ALL irrelevant traffic regardless of
			//age - correct, that is recoverable from the chain. The user's own history is not,
			//so it is retired on elapsed time only, never wholesale. wipeDB() remains the way
			//to actually empty this table.
			num += cleanRelevant();
			
			return num;
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
		
		return 0;
	}
	
	/**
	 * This cleans the JDBC Connection.. so it can start and shutdown quicker
	 */
	public synchronized void closeAndReopen() {
		
		try {
			
			//First close the COnnection and save DB.. no comapct
			saveDB(false);
			
			//And now reopen the DB..
			mSQLConnection = null;
			
			//And now re-open..
			checkOpen(false);
			
		} catch (SQLException e) {
			MinimaLogger.log(e);
		}
	}
	
	public static void main(String[] zArgs) throws IOException, SQLException {
		
		File testdb = new File(System.getProperty("user.home"),"testsql");
		
		TxPoWSqlDB db = new TxPoWSqlDB();
		
		db.loadDB(testdb);

		//test insert..
		TxPoW txp = new TxPoW();
		txp.setTimeMilli();
		txp.setSuperParent(0, new MiniData("0xFFEEFF"));
		txp.calculateTXPOWID();
		
		System.out.println("IN :"+txp.toJSON().toString());
		
		String id = txp.getTxPoWID();
		
		db.addTxPoW(txp, true);
		
//		Runnable rr = new Runnable() {
//			@Override
//			public void run() {
//				TxPoW txpread =  db.getTxPoW(id);
//				System.out.println("OUT:"+txpread.toJSON().toString());
//			}
//		};
//		Thread tt = new Thread(rr);
//		tt.start();
		
//		//Does it exis..
//		boolean exist = db.exists(id);
//		System.out.println("Excits TRUE : "+exist);
//		exist = db.exists("sdsdd");
//		System.out.println("Excits FALSE : "+exist);
//		
//		//get the children
//		ArrayList<String> children = db.getChildBlocks("0xFFEEFF");
//		System.out.println("Children : "+children);
//		
//		//Delete some rows..
//		db.MAX_SQL_MILLI = 20000;
//		long maxtime = System.currentTimeMillis() - 20000;
//		int deleted  = db.cleanDB();
//		System.out.println("Deleted : "+deleted);
//		
//		int total = db.getSize();
//		System.out.println("Total rows : "+total);
//		
//		System.out.println("File : "+db.getSQLFile());
//		System.out.println("File Size : "+db.getSQLFile().length());
		
		//Shutdown..
		db.saveDB(false);
	}
}
