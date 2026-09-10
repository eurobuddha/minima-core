import java.io.*;
import java.math.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import org.minima.database.MinimaDB;
import org.minima.database.txpowtree.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.keys.*;
import org.minima.objects.mmr.*;
import org.minima.system.brains.*;
import org.minima.system.network.minima.RelayPolicy;
import org.minima.system.params.*;
import org.minima.utils.*;
public class ConsensusOracle {
    interface Operation {Object run() throws Exception;}
    static String result(Operation op) {try {return ""+op.run();}catch(Exception e){return "ERROR";}}
    static String wire(Streamable obj) throws Exception {var b=new ByteArrayOutputStream();obj.writeDataStream(new DataOutputStream(b));return BaseConverter.encode16(b.toByteArray());}
    static DataInputStream data(String s) {return new DataInputStream(new ByteArrayInputStream(BaseConverter.decode16(s)));}
    static TxPoW txpow(String s) throws Exception {return TxPoW.ReadFromStream(data(s));}
    static String flags(TxPoW t) {return t.getTransaction().mHaveCheckedMonotonic+":"+t.getTransaction().mIsMonotonic+":"+t.getTransaction().mIsValid+":"+t.getBurnTransaction().mHaveCheckedMonotonic+":"+t.getBurnTransaction().mIsMonotonic+":"+t.getBurnTransaction().mIsValid;}
    static String fixture(int mode,String script,int outputs,int ops) throws Exception {
        var p=new ScriptProof(script);var t=new TxPoW();t.setTimeMilli(new MiniNumber(0));t.getTxBody().mPRNG=new MiniData("0x1234");t.setBlockNumber(new MiniNumber(20));t.setTxDifficulty(Magic.MIN_TXPOW_WORK);
        var coin=new Coin(new MiniData("0x1234"),p.getAddressData(),new MiniNumber(10),Token.TOKENID_MINIMA,true);coin.setBlockCreated(new MiniNumber(2));coin.setMMREntryNumber(new MMREntryNumber(0));coin.getState().add(new StateVariable(0,"5"));
        if(mode==22||mode==23) {var tok=new Token(new MiniData("0x11"),MiniNumber.ZERO,new MiniNumber(100),new MiniString("test"),new MiniString(mode==22?" RETURN TRUE ":"RETURN FALSE"));coin.setToken(tok);coin.resetTokenID(tok.getTokenID());}
        var mmr=new MMR();mmr.setBlockTime(new MiniNumber(19));mmr.addEntry(MMRData.CreateMMRDataLeafNode(coin,coin.getAmount()));mmr.finalizeSet();
        var input=coin.deepCopy();var proofcoin=coin.deepCopy();
        if(mode==1)input.resetCoinID(Coin.COINID_ELTOO);
        if(mode==5)input=new Coin(input.getCoinID(),input.getAddress(),new MiniNumber(11),input.getTokenID());
        if(mode==6)input=new Coin(input.getCoinID(),new MiniData("0x99"),input.getAmount(),input.getTokenID());
        if(mode==7)input.resetCoinID(new MiniData("0x8888"));
        if(mode==8)proofcoin.setSpent(true);
        if(mode==15){input.resetTokenID(new MiniData("0x123456"));proofcoin.resetTokenID(new MiniData("0x123456"));}
        var txn=t.getTransaction();var wit=t.getWitness();
        if(mode!=2&&mode!=20&&mode!=21)txn.addInput(input);
        if(mode!=3&&mode!=20)wit.addCoinProof(new CoinProof(proofcoin,mmr.getProof(new MMREntryNumber(0))));
        if(mode!=9)wit.addScript(p);
        if(mode!=20&&mode!=21)for(int i=0;i<outputs;i++){var output=new Coin(new MiniData("0x1122"),new MiniNumber(mode==18?11:mode==19?-1:1),coin.getTokenID(),i%2==0);output.setToken(coin.getToken());if(mode==16)output.resetTokenID(new MiniData("0x9988"));if(mode==17)output.resetTokenID(Token.TOKENID_CREATE);txn.addOutput(output);}
        txn.addStateVariable(new StateVariable(0,"5"));txn.addStateVariable(new StateVariable(1,"[hello 🐼]"));
        if(mode==4){txn.addInput(input.deepCopy());wit.addCoinProof(new CoinProof(proofcoin,mmr.getProof(new MMREntryNumber(0))));}
        if(mode==10)t.getTxHeader().mChainID=new MiniData("0x01");
        if(mode==11)txn.setLinkHash(new MiniData("0x22"));
        if(mode==12||mode==14){var bt=new Transaction();bt.readDataStream(data(wire(txn)));t.setBurnTransaction(bt);var bw=new Witness();bw.readDataStream(data(wire(wit)));t.setBurnWitness(bw);}
        t.calculateTransactionID();
        if(mode==12)t.getBurnTransaction().setLinkHash(txn.getTransactionID());
        if(mode==25)t.setTxDifficulty(new MiniData(Crypto.MAX_VAL));
        if(mode==27)wit.getAllSignatures().add(new Signature());
        if(mode==28||mode==29){var key=new TreeKey(new MiniData("0x00112233445566778899AABBCCDDEEFF"),2,2);wit.addSignature(key.sign(mode==28?txn.getTransactionID():new MiniData("0x01")));}
        if(mode==26)t.clearBody();else t.setHeaderBodyHash();t.calculateTXPOWID();
        var block=new TxPoW();block.setTimeMilli(new MiniNumber(100000));block.setBlockNumber(new MiniNumber(20));block.getTxBody().mPRNG=new MiniData("0x12");block.getMagic().mCurrentMaxKISSVMOps=new MiniNumber(ops);block.setHeaderBodyHash();block.calculateTXPOWID();
        return wire(mmr)+" "+wire(t)+" "+wire(block);
    }
    static class FixtureDB extends org.minima.database.txpowdb.TxPoWDB {
        ArrayList<TxPoW> pool=new ArrayList<>();ArrayList<String> removed=new ArrayList<>();
        public ArrayList<TxPoW> getAllUnusedTxns(){return new ArrayList<>(pool);}
        public void removeMemPoolTxPoW(String id){removed.add(id);}
    }
    static void field(Object object,String name,Object value) throws Exception {var f=object.getClass().getDeclaredField(name);f.setAccessible(true);f.set(object,value);}
    static String generate(Scanner scan) throws Exception {
        var mmr=MMR.ReadFromStream(data(scan.next()));var source=txpow(scan.next());int count=scan.nextInt(),own=scan.nextInt(),reject=scan.nextInt();var hash=new MiniNumber(scan.next());var time=new MiniNumber(scan.next());var max=new MiniNumber(scan.next());
        var parent=new TxPoW();parent.setBlockNumber(mmr.getBlockTime());parent.setTimeMilli(time);parent.setBlockDifficulty(Magic.MIN_TXPOW_WORK);parent.getMagic().mCurrentMaxTxnPerBlock=max;parent.getMagic().mDesiredMaxTxnPerBlock=max;parent.getTxBody().mPRNG=new MiniData("0x12");for(int i=0;i<GlobalParams.MINIMA_CASCADE_LEVELS;i++)parent.setSuperParent(i,MiniData.ZERO_TXPOWID);parent.setHeaderBodyHash();parent.calculateTXPOWID();var tip=new TxPoWTreeNode(parent);field(tip,"mMMR",mmr);
        MinimaDB.createDB();MinimaDB.getDB().getTxPoWTree().setRoot(tip);MinimaDB.getDB().getUserDB().setHashRate(hash);var db=new FixtureDB();field(MinimaDB.getDB(),"mTxPoWDB",db);
        for(int i=0;i<count;i++){var t=txpow(wire(source));t.setNonce(new MiniNumber(i));t.calculateTXPOWID();for(int j=0;j<reject;j++)t.incrementCheckRejectNumber();db.pool.add(t);}
        var generated=own!=0?TxPoWGenerator.generateTxPoW(source.getTransaction(),source.getWitness(),source.getBurnTransaction(),source.getBurnWitness()):TxPoWGenerator.generateTxPoW(new Transaction(),new Witness());
        generated.getTxBody().mPRNG=new MiniData("0x12");generated.calculateTXPOWID();
        String out=wire(generated)+"|"+TxPoWGenerator.isMempoolFull()+"|"+TxPoWGenerator.getMinMempoolBurn()+"|";
        for(var t:db.pool)out+=t.getCheckRejectNumber()+":"+flags(t)+",";
        out+="|";for(var id:db.removed)out+=id+",";return out;
    }
    public static void main(String[] args) throws Exception {
        GeneralParams.USE_SQL_COINDB=false;MinimaDB.createDB();
        var in=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=in.readLine())!=null)try {
            var scan=new Scanner(line);String op=scan.next();String out;
            if(op.equals("fixture")){int mode=scan.nextInt(),count=scan.nextInt(),ops=scan.nextInt();out=fixture(mode,new String(BaseConverter.decode16(scan.next()),StandardCharsets.UTF_8),count,ops);}
            else if(op.equals("generate"))out=generate(scan);
            else if(op.equals("check")) {
                var mmr=MMR.ReadFromStream(data(scan.next()));var t=txpow(scan.next());var b=txpow(scan.next());int max=scan.nextInt();long state=scan.nextLong();GeneralParams.MAX_RELAY_OUTPUTCOINS=max;
                out=result(()->TxPoWChecker.checkTxPoWBasic(t))+"|"+result(()->TxPoWChecker.checkSignatures(t))+"|"+result(()->TxPoWChecker.checkMMR(mmr,t,false))+"|"+result(()->RelayPolicy.checkAllPolicies(t,state));
                out+="|"+result(()->TxPoWChecker.checkTxPoWScripts(mmr,t,b))+"|"+result(()->flags(t));
                b.setBlockNumber(new MiniNumber(21));out+="|"+result(()->TxPoWChecker.checkTxPoWScripts(mmr,t,b))+"|"+result(()->flags(t));
                out+="|"+result(()->TxPoWChecker.checkTxPoWSimple(mmr,t,b,false))+"|"+result(()->{TxPoWGenerator.precomputeTransactionCoinID(t.getTransaction());return wire(t.getTransaction());});
            } else if(op.equals("difficulty"))out=result(()->TxPoWGenerator.calculateDifficultyData(new MiniNumber(scan.next())).to0xString());
            else if(op.equals("divide")){var a=new BigDecimal(scan.next());var b=new BigDecimal(scan.next());int precision=scan.nextInt();out=a.divide(b,new MathContext(precision,RoundingMode.HALF_EVEN)).toBigInteger().toString();}
            else if(op.equals("chain")){
                int back=scan.nextInt(),count=scan.nextInt(),mutate=scan.nextInt();var nodes=new ArrayList<TxPoWTreeNode>();
                for(int i=0;i<count;i++){var t=new TxPoW();t.setBlockNumber(new MiniNumber(scan.next()));t.setTimeMilli(new MiniNumber(scan.next()));t.setBlockDifficulty(new MiniData(scan.next()));t.getTxBody().mPRNG=new MiniData("0x12");t.setHeaderBodyHash();t.calculateTXPOWID();var node=new TxPoWTreeNode(t);if(back!=-7)node.getMMR().addEntry(MMRData.CreateMMRDataLeafNode(new MiniNumber(1),new MiniNumber(1)));if(i>0)nodes.get(i-1).addChildNode(node);nodes.add(node);}
                var tip=nodes.get(count-1);MinimaDB.getDB().getTxPoWTree().setRoot(nodes.get(0));
                out=result(()->TxPoWGenerator.getMedianTimeBlock(tip,back).getBlockNumber().toString())+"|"+result(()->TxPoWGenerator.getChainSpeed(tip,new MiniNumber(back)).toString())+"|"+result(()->TxPoWGenerator.getBlockDifficulty(tip).to0xString());
                var b=new TxPoW();b.setBlockNumber(tip.getBlockNumber().increment());b.setTimeMilli(tip.getTxPoW().getTimeMilli());b.getTxBody().mPRNG=new MiniData("0x12");b.setMagic(tip.getTxPoW().getMagic().calculateNewCurrent());
                for(int i=0;i<GlobalParams.MINIMA_CASCADE_LEVELS;i++)b.setSuperParent(i,MiniData.ZERO_TXPOWID);
                int level=0;for(int i=count-1;i>=0;i--)while(nodes.get(i).getTxPoW().getSuperLevel()>=level)b.setSuperParent(level++,nodes.get(i).getTxPoW().getTxPoWIDData());
                if(mutate>=0)b.setSuperParent(mutate,new MiniData("0x1234"));
                out+="|"+result(()->TxPoWChecker.checkParents(tip,b));
                try{b.setBlockDifficulty(TxPoWGenerator.getBlockDifficulty(tip));}catch(Exception e){}
                out+="|"+result(()->TxPoWChecker.checkTxBlockOnly(tip,new TxBlock(b)));
                out+="|"+result(()->{var node=new TxPoWTreeNode(new TxBlock(tip.getMMR(),b,new ArrayList<>()),false);b.setMMRRoot(node.getMMR().getRoot().getData());b.setMMRTotal(node.getMMR().getRoot().getValue());return TxPoWChecker.checkTxPoWBlock(tip,b,new ArrayList<>());});
            } else throw new IllegalArgumentException(op);
            System.out.println(out);
        } catch(Exception e){System.out.println("ERROR");}
    }
}
