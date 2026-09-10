import java.io.*;
import java.math.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.*;
import org.minima.system.genesis.*;
import org.minima.objects.keys.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.utils.*;
import org.minima.utils.json.*;
public class ObjectOracle {
    static String str(String hex) {return new String(BaseConverter.decode16(hex),StandardCharsets.UTF_8);}
    static String wire(Streamable obj) throws Exception {var bytes=new ByteArrayOutputStream();obj.writeDataStream(new DataOutputStream(bytes));return BaseConverter.encode16(bytes.toByteArray());}
    static String result(Streamable obj) throws Exception {
        if(obj instanceof Greeting g) return wire(g)+"|"+g.getRootBlock()+"|"+g.getExtraData();
        if(obj instanceof Pulse p) {var ids=new JSONArray();for(var id:p.getBlockList()) ids.add(id.to0xString());return wire(p)+"|"+ids;}
        if(obj instanceof TxBlock b) {
            var inputs=new JSONArray();for(var c:b.getInputCoinProofs()) inputs.add(c.toJSON());
            var outputs=new JSONArray();for(var c:b.getOutputCoins()) outputs.add(c.toJSON());
            var removed=new JSONArray();for(var c:b.getOutputCoins()) {var state=b.removedState(c.getCoinID().to0xString());removed.add(state==null?null:Coin.convertStateListToJSON(state));}
            return wire(b)+"|"+inputs+"|"+outputs+"|"+removed;
        }
        if(obj instanceof MegaMMR m) {var bytes=wire(m);var coins=new JSONArray();for(var c:Collections.list(m.getAllCoins().elements())) coins.add(c.toJSON());return bytes+"|"+m.getMMR().toJSON()+"|"+coins+"|"+m.isEmpty();}
        if(obj instanceof TxPoW t) return wire(t)+"|"+(t.hasBody()?t.toJSON().toString():"HEADER_ONLY")+"|"+t.getTxPoWID()+"|"+t.isBlock()+"|"+t.isTransaction()+"|"+t.getSuperLevel()+"|"+t.getSizeinBytes()+"|"+t.getWeight().unscaledValue()+":"+t.getWeight().scale();
        if(obj instanceof MMREntryNumber n) return wire(n)+"|"+n.toString();
        JSONObject json;
        if(obj instanceof Address a) json=a.toJSON();
        else if(obj instanceof StateVariable s) json=s.toJSON();
        else if(obj instanceof Token t) json=t.toJSON();
        else if(obj instanceof MMRData m) json=m.toJSON();
        else if(obj instanceof Magic m) json=m.toJSON();
        else if(obj instanceof TxBody b) json=b.toJSON();
        else if(obj instanceof TxHeader h) json=h.toJSON();
        else if(obj instanceof MMR m) json=m.toJSON();
        else if(obj instanceof Witness w) json=w.toJSON();
        else if(obj instanceof Signature s) json=s.toJSON();
        else if(obj instanceof SignatureProof s) json=s.toJSON();
        else if(obj instanceof Transaction t) json=t.toJSON();
        else if(obj instanceof MMREntry e) json=e.toJSON();
        else if(obj instanceof Coin c) json=c.toJSON();
        else if(obj instanceof CoinProof c) json=c.toJSON();
        else if(obj instanceof ScriptProof p) json=p.toJSON();
        else json=((MMRProof)obj).toJSON();
        return wire(obj)+"|"+json.toString();
    }
    static Coin coin(int n) {
        var coin=new Coin(new MiniData("0x"+Integer.toHexString(n+1)),new MiniData("0x001122"),new MiniNumber("0.125"),new MiniData("0x00"),n%2==0);
        coin.setMMREntryNumber(new MMREntryNumber(BigInteger.ONE.shiftLeft(n)));
        coin.setSpent(n%3==0);coin.setBlockCreated(new MiniNumber(n*123));
        var states=new ArrayList<StateVariable>();
        for(int i=0;i<n%5;i++) states.add(new StateVariable(i%3,i%2==0?"[state 🐼]":"1.00"));
        coin.setState(states);
        if(n%2==1) {var token=new Token(new MiniData("0xFF"),new MiniNumber(3),new MiniNumber("0.001"),new MiniString("Test"),new MiniString("RETURN TRUE"));coin.setToken(token);coin.resetTokenID(token.getTokenID());}
        return coin;
    }
    static TxPoW fixedPow(int nonce) {
        var t=new TxPoW();t.setTimeMilli(new MiniNumber(1788883200000L));t.setNonce(new MiniNumber(nonce));t.getTxBody().mPRNG=new MiniData("0x000102");return t;
    }
    static TxBlock blockFixture(int size,int flags,int time) {
        var parent=new MMR();parent.setBlockTime(new MiniNumber(time));var coins=new ArrayList<Coin>();
        for(int i=0;i<size;i++) {var c=coin(i);c.setSpent(false);c.setMMREntryNumber(parent.getEntryNumber());parent.addEntry(MMRData.CreateMMRDataLeafNode(c,c.getAmount()));coins.add(c);}
        var txns=new ArrayList<TxPoW>();
        for(int i=0;i<3;i++) {
            var t=fixedPow(i);var txn=t.getTransaction();var witness=t.getWitness();
            if((flags&4)!=0&&i==1) {txn=t.getBurnTransaction();witness=t.getBurnWitness();}
            var input=coins.get(i%size);txn.addInput(input);
            if((flags&8)==0) witness.addCoinProof(new CoinProof(input,parent.getProof(input.getMMREntryNumber())));
            for(int j=0;j<2;j++) {var c=new Coin(new MiniData("0x1122"),new MiniNumber("0.0125"),Token.TOKENID_MINIMA,((flags&1)==0)||j==0);if((flags&2)!=0&&j==1) {c.resetTokenID(Token.TOKENID_CREATE);c.setToken(new Token(new MiniData("0x01"),new MiniNumber(3),c.getAmount(),new MiniString("Created"),new MiniString("RETURN TRUE")));}txn.addOutput(c);}
            txn.addStateVariable(new StateVariable(0,"[transaction "+i+"]"));t.setHeaderBodyHash();t.calculateTXPOWID();txns.add(t);
        }
        var main=txns.get(0);main.setBlockNumber(new MiniNumber(time+1));main.addBlockTxPOW(txns.get(2).getTxPoWIDData());main.addBlockTxPOW(txns.get(1).getTxPoWIDData());main.setHeaderBodyHash();main.calculateTXPOWID();
        if((flags&16)!=0) txns.remove(1);
        return new TxBlock(parent,main,txns);
    }
    static MegaMMR megaFixture(int size,int flags,int rounds) {
        var mega=new MegaMMR();
        for(int r=0;r<rounds;r++) {
            var pow=fixedPow(r);pow.setBlockNumber(new MiniNumber(r+1));pow.setHeaderBodyHash();pow.calculateTXPOWID();var block=new TxBlock(pow);block.getPreviousPeaks().addAll(mega.getMMR().getPeaks());
            if(r>0) for(var c:Collections.list(mega.getAllCoins().elements())) if(c.getMMREntryNumber().modulo(new MMREntryNumber(3)).isEqual(MMREntryNumber.ZERO)) block.getInputCoinProofs().add(new CoinProof(c,mega.getMMR().getProof(c.getMMREntryNumber())));
            for(int i=0;i<size;i++) {var c=coin(i%32);c.resetCoinID(new MiniData("0x"+Integer.toHexString(r*1000+i+1)));if(i%3==0) c=new Coin(c.getCoinID(),new MiniData(new byte[32]),c.getAmount(),c.getTokenID());block.getOutputCoins().add(c);}
            mega.addBlock(block);
            if((flags&8)!=0) mega.getMMR().pruneTree();
        }
        return mega;
    }
    // Mutations must reach every reference; wire/deep copies must stay detached.
    static String coinAliases(int n,int growth,int mode) throws Exception {
        var c=coin(n);var alias=c;var detached=c.deepCopy();
        var renamed=c.getSameCoinWithCoinID(new MiniData("0xCAFE"));
        var txn=new Transaction();txn.addInput(c);txn.addInput(c);txn.addOutput(c);
        var cp=new CoinProof(c,new MMRProof(new MiniNumber(42)));
        var witness=new Witness();witness.addCoinProof(cp);
        var held=txn.getAllInputs().get(0);
        for(int i=0;i<growth;i++) {txn.addInput(coin(i%32));witness.addCoinProof(new CoinProof(coin(i%32),new MMRProof()));}
        var removed=txn.getAllInputs().remove(0);
        var decoded=CoinProof.ReadFromStream(new DataInputStream(new ByteArrayInputStream(BaseConverter.decode16(wire(cp)))));
        Coin target=switch(mode) {
            case 0 -> c;case 1 -> alias;case 2 -> held;case 3 -> removed;
            case 4 -> txn.getAllInputs().get(0);case 5 -> txn.getAllOutputs().get(0);
            case 6 -> cp.getCoin();case 7 -> witness.getAllCoinProofs().get(0).getCoin();
            case 8 -> detached;case 9 -> renamed;case 10 -> decoded.getCoin();
            default -> throw new IllegalArgumentException();
        };
        target.resetCoinID(new MiniData("0xAABB"));target.resetTokenID(new MiniData("0xCC"));
        target.setSpent(!target.getSpent());target.setStoreState(!target.storeState());
        target.setBlockCreated(new MiniNumber("12345678901234567890"));
        target.setMMREntryNumber(new MMREntryNumber(BigInteger.ONE.shiftLeft(100)));
        target.setToken(null);target.getState().add(new StateVariable(255,"[shared mutation]"));
        String first=result(c)+"|"+result(alias)+"|"+result(held)+"|"+result(removed)
            +"|"+result(txn.getAllInputs().get(0))+"|"+result(txn.getAllOutputs().get(0))
            +"|"+result(cp)+"|"+result(witness.getAllCoinProofs().get(0))+"|"+result(cp.getMMRData())
            +"|"+result(detached)+"|"+result(renamed)+"|"+result(decoded);
        // Clear every container, retain a coin reference, replace its state and token.
        txn.getAllInputs().clear();txn.getAllOutputs().clear();witness.clearCoinProofs();
        held.setState(new ArrayList<>());held.setToken(coin(1).getToken());
        alias=coin((n+1)%32); // Rebinding a reference must not replace the old object.
        return first+"|"+result(c)+"|"+result(cp)+"|"+result(removed)+"|"+result(alias)
            +"|"+result(detached)+"|"+result(renamed)+"|"+result(decoded);
    }
    public static void main(String[] args) throws Exception {
        var input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null) {
            try {
                var a=line.split(" ");String out;
                switch(a[0]) {
                case "genesis": {
                    var stdout=System.out;try {System.setOut(new PrintStream(OutputStream.nullOutputStream()));var genesis=new GenesisTxPoW(a[1]);genesis.setTimeMilli(new MiniNumber(1788883200000L));genesis.getTxBody().mPRNG=new MiniData("0x000102");genesis.setHeaderBodyHash();genesis.calculateTXPOWID();out=result(genesis)+"|"+result(new GenesisCoin())+"|"+result(new GenesisMMR());}finally {System.setOut(stdout);}break;
                }
                case "greeting": {var g=new Greeting();g.setTopBlock(new MiniNumber(a[1]));g.getExtraData().put("welcome",str(a[3]));for(int i=0;i<Integer.parseInt(a[2]);i++) g.getChain().add(new MiniData("0x"+Integer.toHexString(i+1)));var stdout=System.out;try {System.setOut(new PrintStream(OutputStream.nullOutputStream()));out=result(g);}finally {System.setOut(stdout);}break;}
                case "pulse": {var p=new Pulse();for(int i=0;i<Integer.parseInt(a[1]);i++) p.getBlockList().add(new MiniData("0x"+Integer.toHexString(i+1)));out=result(p);break;}
                case "txblock":out=result(blockFixture(Integer.parseInt(a[1]),Integer.parseInt(a[2]),Integer.parseInt(a[3])));break;
                case "mega": {
                    int flags=Integer.parseInt(a[2]);org.minima.system.params.GeneralParams.MEGAMMR_MEGAPRUNE=(flags&1)!=0;org.minima.system.params.GeneralParams.MEGAMMR_MEGAPRUNE_STATE=(flags&2)!=0;org.minima.system.params.GeneralParams.MEGAMMR_MEGAPRUNE_TOKENS=(flags&4)!=0;
                    var stdout=System.out;try {System.setOut(new PrintStream(OutputStream.nullOutputStream()));out=result(megaFixture(Integer.parseInt(a[1]),flags,Integer.parseInt(a[3])));}finally {System.setOut(stdout);}
                    break;
                }
                case "pow": {
                    var t=new TxPoW();t.setTimeMilli(new MiniNumber(1788883200000L));t.setNonce(new MiniNumber(a[1]));t.setBlockDifficulty(new MiniData(a[2]));t.getTxBody().mPRNG=new MiniData("0x000102");
                    int mode=Integer.parseInt(a[3]);if(mode==1) t.getTransaction().addInput(coin(0));else if(mode==2) t.getBurnTransaction().addInput(coin(0));
                    t.addBlockTxPOW(new MiniData("0x1234"));t.setHeaderBodyHash();if(mode==3) t.clearBody();t.calculateTXPOWID();out=result(t);break;
                }
                case "magic": {var magic=new Magic();magic.setDesiredMaxTxPoWSize(new MiniNumber(a[1]));magic.setDesiredKISSVM(new MiniNumber(a[2]));magic.setDesiredMaxTxns(new MiniNumber(a[3]));magic.mDesiredMinTxPoWWork=new MiniData(a[4]);out=result(magic)+"|"+result(magic.calculateNewCurrent());break;}
                case "body": {var body=new TxBody();body.mPRNG=new MiniData(a[1]);int count=Integer.parseInt(a[2]);for(int i=0;i<count;i++) {body.mTxPowIDList.add(new MiniData("0x"+Integer.toHexString(i+1)));body.mTransaction.addInput(coin(i));}out=result(body);break;}
                case "header": {var h=new TxHeader();h.mTimeMilli=new MiniNumber(a[1]);int run=Integer.parseInt(a[2]);for(int i=0;i<h.mSuperParents.length;i++) if(run>0) h.mSuperParents[i]=new MiniData("0x"+Integer.toHexString(i/run));h.mNonce=new MiniNumber(900000);h.mBlockNumber=new MiniNumber(123450);h.mTxBodyHash=Crypto.getInstance().hashObject(new MiniData("0x1234"));out=result(h);break;}
                case "treekey": {
                    var key=new TreeKey(new MiniData(a[1]),Integer.parseInt(a[2]),Integer.parseInt(a[3]));key.setUses(Integer.parseInt(a[4]));var data=new MiniData(a[6]);out=key.getPublicKey()+"|"+key.getMaxUses();
                    var stdout=System.out;
                    try {System.setOut(new PrintStream(OutputStream.nullOutputStream()));for(int i=0;i<Integer.parseInt(a[5]);i++) {var sig=key.sign(data);out+="|"+key.getUses()+"|"+wire(sig)+"|"+key.verify(data,sig);}}finally {System.setOut(stdout);}
                    break;
                }
                case "treeverify": {
                    var key=new TreeKey();key.setPublicKey(new MiniData(a[1]));var sig=Signature.ReadFromStream(new DataInputStream(new ByteArrayInputStream(BaseConverter.decode16(a[3]))));out=""+key.verify(new MiniData(a[2]),sig);break;
                }
                case "mmr": {
                    int n=Integer.parseInt(a[1]);String mode=a[2];var tree=new MMR();
                    for(int i=0;i<n;i++) {var leaf=MMRData.CreateMMRDataLeafNode(new MiniString("leaf"+i),new MiniNumber(i%3==0?0:i+1));leaf.setUnspendable(i%2==0);tree.addEntry(leaf);}
                    String checks="";
                    if(mode.equals("prune")) tree.pruneTree();
                    else if(mode.equals("scan")) tree.scanUnspendableTree();
                    else if(mode.equals("child")) {
                        var parent=tree;tree=new MMR(parent);
                        tree.addEntry(MMRData.CreateMMRDataLeafNode(new MiniString("new"),new MiniNumber(7)));
                        if(n>0) {var number=new MMREntryNumber(n/2);var leaf=parent.getEntry(0,number).getMMRData();var proof=parent.getProof(number);checks="|"+tree.checkProofTimeValid(number,leaf,proof);tree.updateEntry(number,proof,new MMRData(new MiniData("0x00"),MiniNumber.ZERO));checks+="|"+tree.checkProofTimeValid(number,leaf,proof);}
                    }
                    var proofs=new JSONArray();
                    if(mode.equals("normal")) for(int i=0;i<n;i++) {var number=new MMREntryNumber(i);var proof=tree.getProof(number);var row=new JSONObject();row.put("proof",proof.toJSON());row.put("valid",tree.checkProof(tree.getEntry(0,number).getMMRData(),proof));proofs.add(row);}
                    out=result(tree)+"|"+proofs+checks;break;
                }
                case "witness": {
                    var witness=new Witness();
                    for(int i=0;i<Integer.parseInt(a[1]);i++) {
                        var sig=new Signature();sig.addSignatureProof(new SignatureProof(new MiniData("0x"+Integer.toHexString(i+1)),new MiniData("0x1234"),new MMRProof()));
                        witness.addSignature(sig);witness.addSignature(sig);
                        witness.addCoinProof(new CoinProof(coin(i),new MMRProof(new MiniNumber(i))));
                        witness.addScript(new ScriptProof("RETURN TRUE"));witness.addScript(new ScriptProof("RETURN TRUE"));
                    }
                    out=result(witness);break;
                }
                case "wots": {var key=new Winternitz(new MiniData(a[1]));var message=new MiniData(a[2]);var sig=key.sign(message);out=key.getPublicKey()+"|"+sig+"|"+Winternitz.verify(key.getPublicKey(),message,sig);break;}
                case "verify":out=""+Winternitz.verify(new MiniData(a[1]),new MiniData(a[2]),new MiniData(a[3]));break;
                case "sigproof":out=result(new SignatureProof(new MiniData(a[1]),new MiniData(a[2]),new MMRProof(new MiniNumber(17))));break;
                case "signature": {var sig=new Signature();for(int i=0;i<Integer.parseInt(a[1]);i++) sig.addSignatureProof(new SignatureProof(new MiniData("0x"+Integer.toHexString(i+1)),new MiniData("0x123456"),new MMRProof(new MiniNumber(i))));out=result(sig);break;}
                case "txn": {
                    var txn=new Transaction();
                    if(!a[3].equals("-")) {var inputCoin=new Coin(new MiniData("0x01"),new MiniData("0x02"),new MiniNumber(a[1]),new MiniData(a[3]));txn.addInput(inputCoin);if(a[5].equals("1")) txn.addInput(inputCoin);}
                    if(!a[4].equals("-")) txn.addOutput(new Coin(new MiniData("0x02"),new MiniNumber(a[2]),new MiniData(a[4])));
                    txn.addStateVariable(new StateVariable(255,"TRUE"));txn.addStateVariable(new StateVariable(0,"[first]"));txn.addStateVariable(new StateVariable(128,"1.00"));txn.addStateVariable(new StateVariable(0,"[replacement]"));
                    var stdout=System.out;boolean valid;
                    try {System.setOut(new PrintStream(OutputStream.nullOutputStream()));valid=txn.checkValid();}finally {System.setOut(stdout);}
                    out=result(txn)+"|"+valid+"|"+txn.sumInputs()+"|"+txn.sumOutputs()+"|"+txn.getBurn()+"|"+txn.calculateStateSize()+"|"+txn.calculateCoinID(new MiniData("0xFF"),7);break;
                }
                case "entry": {
                    var n=new MMREntryNumber(new BigDecimal(new BigInteger(a[1]),Integer.parseInt(a[2])));
                    out=result(n)+"|"+result(n.floor())+"|"+result(n.increment())+"|"+result(n.decrement())+"|"+result(n.div2())+"|"+result(n.mult2())+"|"+result(n.modulo(MMREntryNumber.TWO));break;
                }
                case "mmrentry": {
                    var n=new MMREntryNumber(new BigInteger(a[2]));var entry=new MMREntry(Integer.parseInt(a[1]),n,new MMRData(new MiniData("0x01"),new MiniNumber("7.25")));
                    out=result(entry)+"|"+entry.getSibling()+"|"+entry.getParentEntry()+"|"+entry.getLeftChildEntry()+"|"+entry.getRightChildEntry()+"|"+entry.isLeft();break;
                }
                case "coinalias":out=coinAliases(Integer.parseInt(a[1]),Integer.parseInt(a[2]),Integer.parseInt(a[3]));break;
                case "coin": {var c=coin(Integer.parseInt(a[1]));out=result(c)+"|"+c.toJSON(true)+"|"+result(c.deepCopy());break;}
                case "coinproof": {var c=new CoinProof(coin(Integer.parseInt(a[1])),new MMRProof(new MiniNumber(42)));out=result(c)+"|"+result(c.getMMRData());break;}
                case "scriptproof":out=result(new ScriptProof(str(a[1])));break;
                case "address":out=result(new Address(str(a[1])));break;
                case "mx":out=Address.makeMinimaAddress(new MiniData(a[1]));break;
                case "unmx":out=Address.convertMinimaAddress(a[1]).to0xString();break;
                case "b32":out=BaseConverter.encode32(BaseConverter.decode16(a[1]));break;
                case "unb32":out=BaseConverter.encode16(BaseConverter.decode32(a[1]));break;
                case "state":out=result(new StateVariable(Integer.parseInt(a[1]),str(a[2])));break;
                case "token":out=result(new Token(new MiniData(a[1]),new MiniNumber(a[2]),new MiniNumber(a[3]),new MiniString(str(a[4])),new MiniString(str(a[5])),new MiniNumber(a[6])));break;
                case "leaf":out=result(MMRData.CreateMMRDataLeafNode(new MiniString(str(a[1])),new MiniNumber(a[2])));break;
                case "proof": {
                    var proof=new MMRProof(new MiniNumber(a[1]));var leaf=MMRData.CreateMMRDataLeafNode(new MiniString(str(a[2])),new MiniNumber(a[3]));
                    for(int i=4;i<a.length;i+=3) proof.addProofChunk(a[i].equals("1"),MMRData.CreateMMRDataLeafNode(new MiniString(str(a[i+1])),new MiniNumber(a[i+2])));
                    out=result(proof)+"|"+result(proof.calculateProof(leaf));break;
                }
                case "read": {
                    var readStdout=System.out;try {System.setOut(new PrintStream(OutputStream.nullOutputStream()));
                    var data=new DataInputStream(new ByteArrayInputStream(BaseConverter.decode16(a[2])));Streamable obj;
                    switch(a[1]) {
                    case "greeting":obj=Greeting.ReadFromStream(data);break;
                    case "pulse":obj=Pulse.ReadFromStream(data);break;
                    case "txblock":obj=TxBlock.ReadFromStream(data);break;
                    case "mega":var mega=new MegaMMR();mega.readDataStream(data);obj=mega;break;
                    case "pow":obj=TxPoW.ReadFromStream(data);break;
                    case "magic":obj=Magic.ReadFromStream(data);break;
                    case "header":obj=TxHeader.ReadFromStream(data);break;
                    case "body":var b=new TxBody();b.readDataStream(data);obj=b;break;
                    case "mmr":obj=MMR.ReadFromStream(data);break;
                    case "witness":var w=new Witness();w.readDataStream(data);obj=w;break;
                    case "sigproof":obj=SignatureProof.ReadFromStream(data);break;
                    case "signature":obj=Signature.ReadFromStream(data);break;
                    case "txn":var txn=new Transaction();txn.readDataStream(data);obj=txn;break;
                    case "entry":obj=MMREntryNumber.ReadFromStream(data);break;
                    case "mmrentry":obj=MMREntry.ReadFromStream(data);break;
                    case "coin":obj=Coin.ReadFromStream(data);break;
                    case "coinproof":obj=CoinProof.ReadFromStream(data);break;
                    case "scriptproof":obj=ScriptProof.ReadFromStream(data);break;
                    case "address":obj=Address.ReadFromStream(data);break;
                    case "state":obj=StateVariable.ReadFromStream(data);break;
                    case "token":obj=Token.ReadFromStream(data);break;
                    case "leaf":obj=MMRData.ReadFromStream(data);break;
                    default:obj=MMRProof.ReadFromStream(data);break;
                    }
                    out=result(obj)+"|"+data.available();}finally {System.setOut(readStdout);}break;
                }
                case "json": {
                    var obj=new JSONObject();obj.put("text",str(a[1]));obj.put("flag",true);obj.put("null",null);var arr=new JSONArray();arr.add(17L);arr.add(false);arr.add(str(a[2]));obj.put("array",arr);obj.put("text",str(a[1]));out=obj.toString();break;
                }
                default:throw new IllegalArgumentException();
                }
                System.out.println(out);
            } catch(Exception error) {System.out.println("ERROR");}
        }
    }
}
