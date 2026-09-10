import java.io.*;
import java.math.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.database.txpowtree.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.*;
public class TreeOracle {
    static String hex(String s) {return BaseConverter.encode16(s.getBytes(StandardCharsets.UTF_8));}
    static String wire(Streamable obj) throws Exception {var out=new ByteArrayOutputStream();obj.writeDataStream(new DataOutputStream(out));return BaseConverter.encode16(out.toByteArray());}
    static String id(TxPoWTreeNode n) {return n==null?"null":n.getTxPoW().getTxPoWID();}
    static String result(TxPowTree t,ArrayList<TxPoWTreeNode> nodes,int depth) {
        String s=id(t.getRoot())+"|"+id(t.getTip())+"|"+t.getSize()+"|"+t.getHeaviestBranchLength();
        for(var n:nodes) s+="|"+id(n)+":"+n.getTotalWeight().unscaledValue()+":"+n.getTotalWeight().scale()+":"+id(t.findNode(id(n)))+":"+id(n.getParent())+":"+id(n.getParent(-1))+":"+id(n.getParent(2))+":"+id(n.getPastNode(new MiniNumber(0)));
        s+="|";for(var n:t.getHeaviestBranch())s+=id(n)+",";
        s+="|"+t.getPulseList().size()+"|"+hex(t.printTree(depth));return s;
    }
    public static void main(String[] args) throws Exception {
        GeneralParams.USE_SQL_COINDB=false;var in=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=in.readLine())!=null) {try {
            var scan=new Scanner(line);var op=scan.next();
            if(op.equals("readnode")) {var bytes=new ByteArrayInputStream(BaseConverter.decode16(scan.next()));var n=TxPoWTreeNode.ReadFromStream(new DataInputStream(bytes));System.out.println(wire(n)+"|"+n.getMMR().toJSON()+"|"+n.getAllCoins().size()+"|"+n.getRelevantCoins().size()+"|"+bytes.available());continue;}
            if(op.equals("blocknode")) {int count=scan.nextInt(),mode=scan.nextInt();var t=new TxPoW();t.setTimeMilli(new MiniNumber(0));t.setNonce(new MiniNumber(count));t.setBlockNumber(new MiniNumber(7));t.getTxBody().mPRNG=new MiniData("0x1234");t.setHeaderBodyHash();t.calculateTXPOWID();var b=new TxBlock(t);for(int i=0;i<count;i++)b.getOutputCoins().add(new Coin(new MiniData("0x"+Integer.toHexString(i+1)),new MiniData("0x1122"),new MiniNumber(i+1),new MiniData("0x00"),i%2==0));var n=new TxPoWTreeNode(b,false);for(int i=0;i<count;i++){var coin=b.getOutputCoins().get(i);coin.setMMREntryNumber(new org.minima.objects.mmr.MMREntryNumber(i));n.addCoinToNode(coin);if((i+mode)%2==0)n.getRelevantCoinsEntries().add(coin.getMMREntryNumber());}n.calculateRelevantCoins();if((mode&2)!=0)n.removeRelevantCoin(new org.minima.objects.mmr.MMREntryNumber(0));if((mode&4)!=0)n.calculateRelevantCoins();System.out.println(wire(n)+"|"+n.getMMR().toJSON()+"|"+n.getAllCoins().size()+"|"+n.getRelevantCoins().size());continue;}
            int count=scan.nextInt(),trim=scan.nextInt(),depth=scan.nextInt();var nodes=new ArrayList<TxPoWTreeNode>();var tree=new TxPowTree();
            for(int i=0;i<count;i++) {int parent=scan.nextInt(),height=scan.nextInt(),weight=scan.nextInt();String id=scan.next();var txp=new TxPoW(id,height,weight);txp.setTimeMilli(new MiniNumber(i));var n=new TxPoWTreeNode(txp);if(parent>=0)nodes.get(parent).addChildNode(n);nodes.add(n);}
            if(count>0)tree.setRoot(nodes.get(0));else tree.recalculateTree();String removed="";
            if(trim!=-999)for(var n:tree.setLength(trim))removed+=id(n)+",";
            System.out.println(result(tree,nodes,depth)+"|"+removed);
        }catch(Exception e){System.out.println("ERROR");}}
    }
}
