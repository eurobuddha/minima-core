#include "decryptbackup.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaExceptions.hpp"
#include "support/BufferedFileOutput.hpp"
#include "support/CipherInput.hpp"
#include "support/GzipInput.hpp"
namespace minima::commands {
decryptbackup::decryptbackup():Command("decryptbackup","[file:] (password:) (output:)- Decrypt an encrypted backup."){}
std::optional<std::string> decryptbackup::getFullHelp() const{return std::string(
"\ndecryptbackup \n"
"\n"
"Decrypt an encrypted backup.\n"
"\n"
"file:\n"
"    Specify the filename or local path of the backup to restore\n"
"\n"
"password: (optional)\n"
"    Enter the password of the backup \n"
"\n"
"output: (optional)\n"
"    Specify the output file \n"
"\n"
"Examples:\n"
"\n"
"decryptbackup file:my-full-backup-01-Jan-22 password:Longsecurepassword456\n");}
json::JSONValue decryptbackup::runCommand(){
    auto result=getJSONReply();auto file=getParam("file"),password=getParam("password","minima");
    if(password.empty())throw CommandException("Cannot have a blank password");
    auto input=MiniFile::createBaseFile(file);std::error_code error;
    if(input.string().find('\0')!=std::string::npos||!std::filesystem::exists(input,error))throw JavaCheckedException("Restore file doesn't exist : "+javaUnixAbsoluteFile(input));
    auto name=getParam("output","");if(name.empty())name="decrypted-"+input.filename().string();
    auto output=MiniFile::createBaseFile(name);
    if(output.string().find('\0')==std::string::npos&&std::filesystem::exists(output,error))std::filesystem::remove(output,error);
    BufferedFileOutput destination(output);
    auto bytes=MiniFile::readCompleteFile(input);size_t offset=0;
    auto salt=MiniData::deserialise(bytes,offset),iv=MiniData::deserialise(bytes,offset);
    auto key=GenerateKey::secretKey(password,salt.bytes());
    CipherInput cipher(GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,iv.bytes(),key.bytes),std::span(bytes).subspan(offset));
    std::unique_ptr<GzipInput> gzip;
    try{gzip=std::make_unique<GzipInput>([&](auto data){return cipher.read(data);},[&]{cipher.close();});}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){throw CommandException("Incorrect Password!");}
    try{
        std::array<uint8_t,16384> buffer;
        while(auto count=gzip->read(buffer))destination.write(std::span(buffer).first(count));
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){/* Source intentionally preserves partial output. */}
    destination.flush();destination.close();cipher.close();gzip->close();
    json::JSONObject response;response.put("input",javaUnixAbsoluteFile(input));response.put("inputsize",MiniFormat::formatSize(javaUnixFileLength(input)));
    response.put("output",javaUnixAbsoluteFile(output));response.put("outputsize",MiniFormat::formatSize(javaUnixFileLength(output)));
    response.put("message","You can now open the output file in a HEX editor to get the seed if your backup was not locked");
    result.put("response",response);return result;
}
int64_t decryptbackup::readNextBackup(const std::filesystem::path& output,std::span<const uint8_t> data,size_t& offset){auto bytes=MiniData::deserialise(data,offset);MiniFile::writeDataToFile(output,bytes.bytes());return javaUnixFileLength(output);}
std::shared_ptr<TxPoWList> decryptbackup::readNextTxPoWList(std::span<const uint8_t> data,size_t& offset){return TxPoWList::convertMiniDataVersion(MiniData::deserialise(data,offset));}
}
