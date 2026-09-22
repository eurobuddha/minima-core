package org.minima.database.txpowdb.sql;

import java.io.File;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;

import org.minima.system.params.GeneralParams;

/**
 * Runnable check for the txpow retention fix.
 *
 * Until this fix the ONLY delete on the txpow table was
 * "DELETE FROM txpow WHERE timemilli < ? AND isrelevant=0", so a row once marked relevant could
 * never be removed by anything - not the routine clean, not cleanDB(true), not a restart. A table
 * designed as a rolling window grew without bound; 583 MB was measured on a phone whose whole node
 * process is capped at 512 MB of Dalvik heap.
 *
 * Not JUnit: the fork's Gradle is 6.7.1 and caps at JDK 15, while the only JDK here is 26. This is
 * a plain main() with asserts, the same shape as TxPoWSqlDB's own main().
 *
 *   javac -cp jar/minima.jar:<h2-2.1.214.jar> -d /tmp/c src/... test/...
 *   java  -cp /tmp/c:jar/minima.jar:<h2-2.1.214.jar> org.minima.database.txpowdb.sql.TxPoWSqlDBCleanCheck
 *
 * Test against H2 2.1.214 - the version the APK pins from Maven - not the 2.4.240 in the fat jar.
 */
public class TxPoWSqlDBCleanCheck {

	private static final long DAY = 1000L * 60 * 60 * 24;
	private static int checks = 0;

	public static void main(String[] zArgs) throws Exception {

		File dir = new File(System.getProperty("java.io.tmpdir"), "txpowclean-" + System.nanoTime());
		File dbf = new File(dir, "txpowdb");

		TxPoWSqlDB db = new TxPoWSqlDB();
		db.loadDB(dbf);

		// Second connection to the same embedded file, so rows can be seeded with a chosen age -
		// addTxPoW always stamps System.currentTimeMillis().
		String url = "jdbc:h2:" + dbf.getAbsolutePath() + ";MODE=MySQL;DB_CLOSE_ON_EXIT=FALSE";
		Connection seed = DriverManager.getConnection(url, "SA", "");

		long now = System.currentTimeMillis();
		// id, isrelevant, age in days
		insert(seed, "irrelevant_fresh",   0, 1);
		insert(seed, "irrelevant_old",     0, 10);
		insert(seed, "relevant_fresh",     1, 1);
		insert(seed, "relevant_midwindow", 1, 10);
		insert(seed, "relevant_ancient",   1, 400);

		check("seeded 5 rows", count(seed) == 5);

		db.cleanDB();

		// Irrelevant: the existing 3-day window (NUMBER_DAYS_SQLTXPOWDB).
		check("irrelevant inside the window survives", exists(seed, "irrelevant_fresh"));
		check("irrelevant past the window is deleted", !exists(seed, "irrelevant_old"));

		// Relevant: the new, longer window (NUMBER_DAYS_RELEVANT_SQLTXPOWDB, default 30).
		check("relevant inside the window survives", exists(seed, "relevant_fresh"));
		check("relevant well inside the window survives", exists(seed, "relevant_midwindow"));
		check("relevant past the window IS deleted - the whole point of the fix",
				!exists(seed, "relevant_ancient"));

		// A hard clean is a memory-pressure sweep (MinimaDB.fullDBRestartMemFree). It drops all
		// irrelevant traffic regardless of age - that is recoverable from the chain. The user's own
		// history is not, so it must still age out on elapsed time only, never wholesale.
		insert(seed, "irrelevant_fresh2", 0, 1);
		insert(seed, "relevant_fresh2",   1, 1);
		db.cleanDB(true);
		check("hard clean drops fresh irrelevant rows", !exists(seed, "irrelevant_fresh2"));
		check("hard clean does NOT wipe the user's recent history", exists(seed, "relevant_fresh2"));
		check("hard clean leaves in-window relevant rows alone", exists(seed, "relevant_fresh"));

		// The window must never retire relevant rows sooner than ordinary traffic, whatever the
		// params say - cleanRelevant() clamps to max(relevant, plain).
		long saveRel = GeneralParams.NUMBER_DAYS_RELEVANT_SQLTXPOWDB;
		try {
			GeneralParams.NUMBER_DAYS_RELEVANT_SQLTXPOWDB = 1;   // below the 3-day plain window
			insert(seed, "relevant_2days", 1, 2);
			db.cleanDB();
			check("a relevant window below the plain one is clamped, not honoured",
					exists(seed, "relevant_2days"));
		} finally {
			GeneralParams.NUMBER_DAYS_RELEVANT_SQLTXPOWDB = saveRel;
		}

		seed.close();
		db.saveDB(false);
		deleteAll(dir);

		System.out.println("\nALL PASS - " + checks + " checks");
	}

	private static void insert(Connection zConn, String zId, int zRelevant, long zAgeDays) throws Exception {
		Statement st = zConn.createStatement();
		st.execute("INSERT INTO txpow ( txpowid, isblock, istransaction, parentid, timemilli, txpowdata, isrelevant ) "
				+ "VALUES ( '" + zId + "', 0, 1, '0x00', "
				+ (System.currentTimeMillis() - (zAgeDays * DAY)) + ", X'00', " + zRelevant + " )");
		st.close();
	}

	private static boolean exists(Connection zConn, String zId) throws Exception {
		Statement st = zConn.createStatement();
		ResultSet rs = st.executeQuery("SELECT COUNT(*) AS tot FROM txpow WHERE txpowid='" + zId + "'");
		rs.next();
		boolean found = rs.getInt("tot") > 0;
		st.close();
		return found;
	}

	private static int count(Connection zConn) throws Exception {
		Statement st = zConn.createStatement();
		ResultSet rs = st.executeQuery("SELECT COUNT(*) AS tot FROM txpow");
		rs.next();
		int tot = rs.getInt("tot");
		st.close();
		return tot;
	}

	private static void check(String zWhat, boolean zPass) {
		checks++;
		System.out.println((zPass ? "  OK   " : "  FAIL ") + zWhat);
		if (!zPass) {
			throw new AssertionError(zWhat);
		}
	}

	private static void deleteAll(File zFile) {
		File[] kids = zFile.listFiles();
		if (kids != null) {
			for (File k : kids) {
				deleteAll(k);
			}
		}
		zFile.delete();
	}
}
