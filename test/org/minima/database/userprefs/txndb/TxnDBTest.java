package org.minima.database.userprefs.txndb;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;

import org.junit.Test;
import org.minima.objects.Transaction;
import org.minima.objects.Witness;

/** addNewTransaction: checked and added in one step (third review MINOR 3) */
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
	
	@Test
	public void listTxnsIsACopy() throws Exception {
		//txnlist iterates the list while other threads add / delete rows: it must not be the live list
		TxnDB db = new TxnDB();
		db.createTransaction("one");
		java.util.ArrayList<TxnRow> list = db.listTxns();
		db.createTransaction("two");
		db.deleteTransaction("one");
		assertEquals(1, list.size());
		assertEquals("one", list.get(0).getID());
	}
	
	@Test
	public void latchStartedRaceHasExactlyOneWinner() throws Exception {
		//All threads released at once by one latch, many rounds: exactly one addNewTransaction wins each round
		int threads = 16;
		for(int round=0;round<500;round++) {
			TxnDB db = new TxnDB();
			java.util.concurrent.CountDownLatch ready = new java.util.concurrent.CountDownLatch(threads);
			java.util.concurrent.CountDownLatch go    = new java.util.concurrent.CountDownLatch(1);
			AtomicInteger wins = new AtomicInteger();
			Thread[] all = new Thread[threads];
			for(int i=0;i<threads;i++) {
				all[i] = new Thread(() -> {
					ready.countDown();
					try { go.await(); } catch(InterruptedException e) { return; }
					if(db.addNewTransaction(new TxnRow("race"+"", new Transaction(), new Witness()))) {
						wins.incrementAndGet();
					}
				});
				all[i].start();
			}
			ready.await();
			go.countDown();
			for(Thread t : all) t.join();
			assertEquals("round "+round, 1, wins.get());
			assertEquals(1, db.listTxns().size());
		}
	}
}
