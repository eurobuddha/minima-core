#include "MinimaRPCClient.hpp"
#include "RPCClient.hpp"
#include "MiniFormat.hpp"
#include "support/NativeThrowable.hpp"
#include "support/SocketCharacterReader.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>

namespace minima {
namespace {
using String=MinimaRPCClient::String;
const std::u16string& required(const String& text) {
    if(!text)throw JavaNullPointerException("null RPC command-line string");
    return *text;
}
RPCClient::String utf8(const String& text) {return text?RPCClient::String(unicode::encodeUTF8(*text)):std::nullopt;}
std::u16string trim(const std::u16string& text) {
    size_t start=0,end=text.size();
    while(start<end&&text[start]<=32)++start;
    while(end>start&&text[end-1]<=32)--end;
    return text.substr(start,end-start);
}
// java.net.URLEncoder UTF-8 form encoding, using the port's existing charset
// replacement behavior. This is distinct from encoding an entire URL.
std::string encodeCommand(const std::u16string& text) {
    constexpr char hex[]="0123456789ABCDEF";
    std::string result;
    for(unsigned char byte:unicode::encodeUTF8(text)) {
        if((byte>='a'&&byte<='z')||(byte>='A'&&byte<='Z')||(byte>='0'&&byte<='9')||byte=='.'||byte=='-'||byte=='*'||byte=='_')result+=char(byte);
        else if(byte==' ')result+='+';
        else {result+='%';result+=hex[byte>>4];result+=hex[byte&15];}
    }
    return result;
}
std::string pretty(const std::string& result) {
    auto parsed=json::parser::JSONParser().parse(result);
    if(parsed.isNull())throw JavaNullPointerException("null JSONObject");
    auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&parsed.value);
    if(!object)throw JavaClassCastException("JSON value is not a JSONObject");
    return MiniFormat::JSONPretty(**object);
}

}
int MinimaRPCClient::main(const std::vector<String>* args,Runtime runtime) {
    if(!runtime.println)runtime.println=[](const std::string& text){std::cout<<text<<std::endl;};
    if(!runtime.printStackTrace)runtime.printStackTrace=[](const std::exception& error){NativeThrowable(error).printStackTrace(std::cerr);};
    if(!runtime.openInput)runtime.openInput=[](){return openStandardConsoleInput();};
    String host=u"http://127.0.0.1:9005",username=u"minima",password=u"password",sslpubkey=u"",command=u"";
    bool bpass=false;
    if(!args)throw JavaNullPointerException("null argument array");
    size_t counter=0;
    auto next=[&]()->String {
        const auto index=counter++;
        if(index>=args->size())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(index)+" out of bounds for length "+std::to_string(args->size()));
        return (*args)[index];
    };
    while(counter<args->size()) {
        const auto arg=next();const auto& flag=required(arg);
        if(flag==u"-host")host=next();
        else if(flag==u"-username")username=next();
        else if(flag==u"-password"){password=next();bpass=true;}
        else if(flag==u"-sslpubkey")sslpubkey=next();
        else if(flag==u"-command")command=next();
        else if(flag==u"-help") {
            for(const char* line:{"MinimaRPCClient Help"," -host       : Specify the host IP:PORT"," -password   : Specify the RPC Basic AUTH password (use with SSL)"," -username   : Specify the RPC Basic AUTH Username (defaults to minima)"," -command    : Specify a single command to run"," -sslpubkey  : The SSL public key from Minima rpc command ( if using SSL )"," -help       : Print this help"})runtime.println(line);
            return 1;
        }else {runtime.println("Unknown parameter : "+unicode::encodeUTF8(flag));return 1;}
    }
    // The upstream SSL-context setup is commented out. Its flag stays false.
    if(!required(host).ends_with(u"/"))*host+=u"/";
    if(required(command).empty()) {
        for(const char* line:{"**********************************************","*  __  __  ____  _  _  ____  __  __    __    *","* (  \\/  )(_  _)( \\( )(_  _)(  \\/  )  /__\\   *","*  )    (  _)(_  )  (  _)(_  )    (  /(__)\\  *","* (_/\\/\\_)(____)(_)\\_)(____)(_/\\/\\_)(__)(__) *","*                                            *","**********************************************","Welcome to the Minima RPCClient - for assistance type help. Then press enter."})MinimaLogger::log(line);
        MinimaLogger::log(String(u"host        :"+*host));
        MinimaLogger::log("ssl         :false");
        MinimaLogger::log(bpass?"usepassword :true":"usepassword :false");
        MinimaLogger::log(String(u"sslpubkey   :"+sslpubkey.value_or(u"null")));
        MinimaLogger::log("To exit this app use 'exit'. 'quit' will shutdown Minima");
    }
    auto request=[&](const std::string& encoded){return RPCClient::sendGETBasicAuth(unicode::encodeUTF8(*host)+encoded,utf8(username),utf8(password));};
    if(!command->empty()) {
        auto result=request(encodeCommand(*command));
        try {runtime.println(pretty(result));}
        catch(const json::parser::ParseException& error){runtime.printStackTrace(error);}
    }else {
        auto input=runtime.openInput();
        if(!input)throw JavaNullPointerException("null input reader");
        for(;;) {
            try {
                auto line=input->readLine();
                if(line&&!line->empty()) {
                    auto text=trim(*line);
                    if(text==u"exit")break;
                    auto encoded=encodeCommand(text);
                    runtime.println(pretty(request(encoded)));
                    if(encoded=="quit")break;
                }
            }catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
        }
        try {input->closeBufferedReader();input->closeInputStreamReader();}
        catch(const wire::IOException& error){MinimaLogger::log(javaIOExceptionText(error));}
    }
    return 0;
}
}
