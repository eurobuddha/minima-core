import org.minima.utils.encrypt.*;
public class CryptoMainsOracle {
    public static void main(String[] args) throws Exception {
        String[] input=args[1].equals("null")?null:args[1].equals("args")?new String[]{"unused","value"}:new String[0];
        switch(args[0]) {
            case "EncryptDecrypt":EncryptDecrypt.main(input);break;
            case "PasswordCrypto":PasswordCrypto.main(input);break;
            case "CryptoPackage":CryptoPackage.main(input);break;
            case "SignVerify":SignVerify.main(input);break;
            default:throw new IllegalArgumentException(args[0]);
        }
    }
}
