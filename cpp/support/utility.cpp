#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "org/minima/utils/encrypt/SignVerify.hpp"
#include "org/minima/utils/encrypt/CryptoPackage.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "support/Gzip.hpp"
#include <sstream>
using namespace minima;
static auto bytes(const std::string& s){return BaseConverter::decode16(s);}
static auto str(const std::string& s){auto b=bytes(s);return MiniString(std::span<const uint8_t>(b)).toString();}
static auto hx(std::span<const uint8_t> b){return BaseConverter::encode16(b);}
static auto hx(const std::string& s){return hx(std::span(reinterpret_cast<const uint8_t*>(s.data()),s.size()));}
int main(int argc,char** argv){std::filesystem::path root=argc>1?argv[1]:".";std::string line;while(std::getline(std::cin,line)){try{std::istringstream in(line);std::vector<std::string> p;std::string part;while(in>>part)p.push_back(part);auto op=p.at(0);std::string out;
    if(op=="gen"){auto key=GenerateKey::generateKeyPair();out=hx(key.getPublic().getEncoded())+"|"+hx(key.getPrivate().getEncoded());}
    else if(op=="key")out=hx((p[1]=="public"?GenerateKey::convertBytesToPublic(bytes(p[2])):GenerateKey::convertBytesToPrivate(bytes(p[2]))).getEncoded());
    else if(op=="rsacipher"){auto cipher=GenerateKey::getAsymetricCipher();cipher.init(std::stoi(p[2]),p[1]=="public"?GenerateKey::convertBytesToPublic(bytes(p[3])):GenerateKey::convertBytesToPrivate(bytes(p[3])));out=hx(cipher.doFinal(bytes(p[4])));}
    else if(op=="derive")out=hx(GenerateKey::secretKey(str(p[1]),bytes(p[2])).getEncoded());
    else if(op=="aesenc")out=hx(EncryptDecrypt::encryptSYM(bytes(p[1]),bytes(p[2]),bytes(p[3])));
    else if(op=="aesdec")out=hx(EncryptDecrypt::decryptSYM(bytes(p[1]),bytes(p[2]),bytes(p[3])));
    else if(op=="rsaenc")out=hx(EncryptDecrypt::encryptASM(bytes(p[1]),bytes(p[2])));
    else if(op=="rsadec")out=hx(EncryptDecrypt::decryptASM(bytes(p[1]),bytes(p[2])));
    else if(op=="sign")out=hx(SignVerify::sign(bytes(p[1]),bytes(p[2])));
    else if(op=="verify")out=SignVerify::verify(bytes(p[1]),bytes(p[2]),bytes(p[3]))?"true":"false";
    else if(op=="pwenc")out=PasswordCrypto::encryptPassword(str(p[1]),MiniData(bytes(p[2]))).to0xString();
    else if(op=="pwdec")out=PasswordCrypto::decryptPassword(str(p[1]),MiniData(bytes(p[2]))).to0xString();
    else if(op=="packenc"){CryptoPackage pack;pack.encrypt(bytes(p[2]),bytes(p[1]));out=pack.getCompleteEncryptedData().to0xString();}
    else if(op=="packdec"){CryptoPackage pack;pack.ConvertMiniDataVersion(MiniData(bytes(p[2])));out=hx(pack.decrypt(bytes(p[1])));}
    else if(op=="gzip")out=hx(compressGzip(bytes(p[1])));
    else if(op=="ungzip")out=hx(decompressGzip(bytes(p[1])));
    else if(op=="size")out=hx(MiniFormat::formatSize(std::stoll(p[1])));
    else if(op=="time")out=hx(MiniFormat::ConvertMilliToTime(std::stoll(p[1])));
    else if(op=="pad")out=hx(MiniFormat::zeroPad(std::stoi(p[1]),MiniNumber(p[2])));
    else if(op=="type")out=hx(MiniFile::getContentType(str(p[1])));
    else if(op=="child")out=MiniFile::isChild(str(p[1]),str(p[2]))?"true":"false";
    else if(op=="pretty"){auto value=json::JSONValue::parseWithException(str(p[1]));if(auto array=std::get_if<std::shared_ptr<json::JSONArray>>(&value.value))out=hx(MiniFormat::JSONPretty(**array));else out=hx(MiniFormat::JSONPretty(*std::get<std::shared_ptr<json::JSONObject>>(value.value)));}
    else if(op=="files"){
        auto folder=root/"fixture";MiniFile::deleteFileOrFolder(std::filesystem::absolute(root).string(),folder);std::filesystem::create_directories(folder);
        auto source=folder/"sub/input.dat",copy=folder/"copy.dat",zipped=folder/"file.gz",plain=folder/"plain.dat";auto payload=bytes(p[1]);MiniFile::writeDataToFile(source,payload);MiniFile::writeDataToFile(source,std::vector<uint8_t>{1,2,3},true);MiniFile::copyFile(source,copy);MiniFile::copyFileOrFolder(folder/"sub",folder/"subcopy");MiniFile::compressGzipFile(copy,zipped);MiniFile::decompressGzipFile(zipped,plain);
        out=hx(MiniFile::readCompleteFile(source))+"|"+hx(MiniFile::readCompleteFile(copy))+"|"+hx(MiniFile::readCompleteFile(zipped))+"|"+hx(MiniFile::readCompleteFile(plain))+"|"+std::to_string(MiniFile::getTotalFileSize(folder));json::JSONObject sizes;out+="|"+std::to_string(MiniFile::getTotalFileSizeWithNames(folder,sizes,2,0))+"|"+hx(sizes.toString());
        auto object=folder/"object.dat";MiniData original(payload),loaded=MiniData::fromHex("0xFE");MiniFile::saveObject(object,original);MiniFile::loadObject(object,loaded);out+="|"+loaded.to0xString();MiniFile::saveObjectDirect(object,original);MiniFile::loadObjectSlow(object,loaded);out+="|"+loaded.to0xString();MiniFile::saveObjectEncrypted("password",object,original);loaded=MiniData::fromHex("0xFE");MiniFile::loadObjectEncrypted("password",object,loaded);out+="|"+loaded.to0xString();MiniFile::deleteFileOrFolder(std::filesystem::absolute(root).string(),folder/"subcopy");out+=std::string("|")+(std::filesystem::exists(folder/"subcopy")?"true":"false");
    }else throw std::invalid_argument("Unknown utility operation");std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}}
