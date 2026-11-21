package org.minima.database.userprefs.txndb;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;

import org.junit.Test;
import org.minima.objects.Transaction;
import org.minima.objects.Witness;

/** addNewTransaction: checked and added in one step  */
public class TxnDBTest {

	@Test
	public void addNewTransactionNeverReplaces() {
		TxnDB db = new TxnDB();
		TxnRow first = new TxnRow("same", new Transaction(), new Witness());
		assertTrue(db.addNewTransaction(first));
		assertFalse(db.addNewTransaction(new TxnRow("same", new Transaction(), new Witness())));
		assertTrue(db.getTransactionRow("same") == first);
	}
	
	@Test
	public void onlyOneOfManyRacersWins() throws Exception {
		for(int round=0;round<50;round++) {
			TxnDB db = new TxnDB();
			AtomicInteger wins = new AtomicInteger();
			Thread[] threads = new Thread[8];
			for(int i=0;i<threads.length;i++) {
				threads[i] = new Thread(() -> {
					if(db.addNewTransaction(new TxnRow("race", new Transaction(), new Witness()))) {
						wins.incrementAndGet();
					}
				});
			}
			for(Thread t : threads) t.start();
			for(Thread t : threads) t.join();
			assertEquals(1, wins.get());
			assertEquals(1, db.listTxns().size());
		}
	}
}
