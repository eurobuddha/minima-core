#include "CMDHandler.hpp"
#include "Authorizer.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/network/p2p/P2PFunctions.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include "support/SocketCharacterReader.hpp"
#include "support/JavaURLDecoder.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaUpperCase.hpp"
#include "support/JavaDateString.hpp"
#include <cstdlib>
namespace minima {
namespace {
std::string trim(const std::string& text){size_t start=0,end=text.size();while(start<end&&uint8_t(text[start])<=32)++start;while(end>start&&uint8_t(text[end-1])<=32)--end;return text.substr(start,end-start);}
class NoSuchElement:public std::runtime_error {public:NoSuchElement():std::runtime_error("java.util.NoSuchElementException") {}};
void print(const std::shared_ptr<PosixNIOSocket>& socket,const std::string& text){
    // PrintWriter records output IO errors without throwing them to CMDHandler.
    try{nioWriteAll(socket,{reinterpret_cast<const uint8_t*>(text.data()),text.size()});}catch(const wire::IOException&){}
}
}
void CMDHandler::run(){
    std::string firstline="no first line..";bool quit=false;
    try{
        if(!mSocket)throw std::invalid_argument("Null RPC socket");SocketCharacterReader reader(mSocket);
        auto input=reader.readLine();if(!input)input="";firstline=*input;size_t position=0;
        auto token=[&](){position=input->find_first_not_of(" \t\n\r\f",position);if(position==std::string::npos)throw NoSuchElement();auto end=input->find_first_of(" \t\n\r\f",position);auto result=input->substr(position,end==std::string::npos?end:end-position);position=end;return result;};
        auto method=javaUpperCase(token());auto requested=token();
        if(requested.starts_with('/'))requested.erase(0,1);if(requested.ends_with('/'))requested.pop_back();requested=trim(javaURLDecode(requested));
        json::JSONObject user;user.put("valid",true);user.put("mode","write");
        bool authenticate=GeneralParams::RPC_AUTHENTICATE;
        if(!authenticate){if(!Authorizer::userDB)throw std::logic_error("CMDHandler requires MinimaDB binding");auto db=Authorizer::userDB();if(!db)throw std::invalid_argument("Null UserDB");auto users=db->getRPCUsers();if(!users)throw std::invalid_argument("Null RPC users");authenticate=users->size()>0;}
        if(authenticate)user.put("valid",false);
        int32_t contentlength=0;
        while(input&&!trim(*input).empty()){
            if(input->find("Authorization:")!=std::string::npos)user=Authorizer::checkAuchCredentials(input);
            if(input->find("Content-Length:")!=std::string::npos)contentlength=javaParseInt(trim(input->substr(input->find(':')+1)));
            input=reader.readLine();
        }
        if(!user.get("valid").asBool()){
            print(mSocket,"HTTP/1.1 401 Unauthorized\nServer: HTTP RPC Server from Minima 1.3\n\n{\"error\":\"Authentication failure..\"}\n");
            throw std::invalid_argument("Invalid Authentication at RPC");
        }
        if(method=="POST"){
            if(contentlength<0)throw JavaNegativeArraySizeException(std::to_string(contentlength));
            auto [characters,total]=reader.readCharacters(size_t(contentlength));
            if(total!=size_t(contentlength))p2p::P2PFunctions::log("CMDHANDLER : Read wrong amount -1/"+std::to_string(contentlength));
            requested=unicode::encodeUTF8(characters);
        }
        json::JSONObject failed;failed.put("status",false);std::string result=failed.toJSONString();
        try{
            if(requested=="quit")quit=true;
            auto commands=CommandRunner::getRunner().runMultiCommand(requested);
            result=commands.size()==1?commands.get(0).toJSONString():commands.toJSONString();
        }catch(const std::exception& error){p2p::P2PFunctions::log("ERROR CMDHANDLER : "+requested+" "+javaExceptionText(error));}
        std::string eol=GeneralParams::RPC_CRLF?"\r\n":"\n";
        print(mSocket,"HTTP/1.1 200 OK"+eol+"Server: HTTP RPC Server from Minima 1.3"+eol+
            "Date: "+javaDateString(TimerMessage::currentTimeMillis())+eol+"Content-type: text/plain"+eol+
            "Content-length: "+std::to_string(result.size())+eol+"Access-Control-Allow-Origin: *"+eol+eol+result+"\n");
    }catch(const NoSuchElement& error){p2p::P2PFunctions::log("CMDHANDLER : "+std::string(error.what())+" "+firstline);}
    catch(const std::exception& error){p2p::P2PFunctions::log("CMDHANDLER : "+javaExceptionText(error)+" "+firstline);}
    try{if(mSocket)mSocket->close();}catch(const std::exception& error){p2p::P2PFunctions::log(javaExceptionText(error));}
    if(quit)std::_Exit(0);
}
}
