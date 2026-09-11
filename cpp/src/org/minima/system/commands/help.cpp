#include "help.hpp"
#include "CommandRunner.hpp"
#include "whitepaper.hpp"
#include "base/status.hpp"
#include "base/block.hpp"
#include "base/scanchain.hpp"
#include "base/printtree.hpp"
#include "base/burn.hpp"
#include "base/trace.hpp"
#include "base/logs.hpp"
#include "base/hashtest.hpp"
#include "base/timemilli.hpp"
#include "base/checkaddress.hpp"
#include "search/history.hpp"
#include "search/txpow.hpp"
#include "search/coins.hpp"
#include "search/tokens.hpp"
#include "search/keys.hpp"
#include "base/getaddress.hpp"
#include "base/newaddress.hpp"
#include "send/send.hpp"
#include "send/sendpoll.hpp"
#include "send/sendnosign.hpp"
#include "send/sendview.hpp"
#include "send/sendsign.hpp"
#include "send/sendpost.hpp"
#include "send/multisig.hpp"
#include "base/sphincs.hpp"
#include "send/wallet/sendfrom.hpp"
#include "send/wallet/rawtxnfrom.hpp"
#include "send/wallet/createfrom.hpp"
#include "send/wallet/signfrom.hpp"
#include "send/wallet/postfrom.hpp"
#include "send/wallet/createtokenfrom.hpp"
#include "base/balance.hpp"
#include "base/tokencreate.hpp"
#include "base/tokenvalidate.hpp"
#include "base/consolidate.hpp"
#include "base/hash.hpp"
#include "base/random.hpp"
#include "base/convert.hpp"
#include "base/maths.hpp"
#include "scripts/scripts.hpp"
#include "scripts/newscript.hpp"
#include "scripts/runscript.hpp"
#include "scripts/removescript.hpp"
#include "tutorial.hpp"
#include "base/mmrcreate.hpp"
#include "base/mmrproof.hpp"
#include "base/coincheck.hpp"
#include "base/coinimport.hpp"
#include "base/coinexport.hpp"
#include "base/cointrack.hpp"
#include "base/coinnotify.hpp"
#include "signatures/sign.hpp"
#include "signatures/verify.hpp"
#include "txn/txnlist.hpp"
#include "txn/txncreate.hpp"
#include "txn/txnauto.hpp"
#include "txn/txnaddamount.hpp"
#include "txn/txnbasics.hpp"
#include "txn/txndelete.hpp"
#include "txn/txncheck.hpp"
#include "txn/txninput.hpp"
#include "txn/txnoutput.hpp"
#include "txn/txnstate.hpp"
#include "txn/txnscript.hpp"
#include "txn/txnmmr.hpp"
#include "txn/txnsign.hpp"
#include "txn/txnclear.hpp"
#include "txn/txnpost.hpp"
#include "txn/txnimport.hpp"
#include "txn/txnexport.hpp"
#include "txn/txnview.hpp"
#include "txn/txnmine.hpp"
#include "txn/txnminepost.hpp"
#include "network/network.hpp"
#include "network/message.hpp"
#include "network/connect.hpp"
#include "network/disconnect.hpp"
#include "network/rpc.hpp"
#include "network/webhooks.hpp"
#include "backup/backup.hpp"
#include "backup/restore.hpp"
#include "backup/restoresync.hpp"
#include "backup/decryptbackup.hpp"
#include "backup/reset.hpp"
#include "backup/archive.hpp"
#include "backup/vault.hpp"
#include "backup/mmrsync/megammr.hpp"
#include "backup/mmrsync/megammrsync.hpp"
#include "backup/rescue.hpp"
#include "backup/mysql.hpp"
#include "backup/mysqlcoins.hpp"
#include "base/quit.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::commands {
help::help():Command("help","Show Help. [] are required. () are optional. Use 'help command:' for full help. Chain multiple commands with ;"){}
json::JSONValue help::runCommand(){
    auto result=getJSONReply();
    json::JSONObject details;
    auto name=getParam("command","");
    if(!name.empty()){
        auto command=CommandRunner::getRunner().getCommandOnly(name);
        if(!command)throw CommandException("Command not found : "+name);
        details.put("command",name);
        details.put("help",command->getHelp()?json::JSONValue(*command->getHelp()):json::JSONValue());
        auto full=command->getFullHelp();details.put("fullhelp",full?json::JSONValue(*full):json::JSONValue());
    }else{
        addCommand(details,help());
        addCommand(details,whitepaper());
        addCommand(details,status());
        addCommand(details,block());
        addCommand(details,scanchain());
        addCommand(details,printtree());
        addCommand(details,burn());
        addCommand(details,trace());
        addCommand(details,logs());
        addCommand(details,hashtest());
        addCommand(details,timemilli());
        addCommand(details,checkaddress());
        addCommand(details,history());
        addCommand(details,txpow());
        addCommand(details,coins());
        addCommand(details,tokens());
        addCommand(details,keys());
        addCommand(details,getaddress());
        addCommand(details,newaddress());
        addCommand(details,send());
        addCommand(details,sendpoll());
        addCommand(details,sendnosign());
        addCommand(details,sendview());
        addCommand(details,sendsign());
        addCommand(details,sendpost());
        addCommand(details,multisig());
        addCommand(details,sphincs());
        addCommand(details,sendfrom());
        addCommand(details,rawtxnfrom());
        addCommand(details,createfrom());
        addCommand(details,signfrom());
        addCommand(details,postfrom());
        addCommand(details,createtokenfrom());
        addCommand(details,balance());
        addCommand(details,tokencreate());
        addCommand(details,tokenvalidate());
        addCommand(details,consolidate());
        addCommand(details,hash());
        addCommand(details,random());
        addCommand(details,convert());
        addCommand(details,maths());
        addCommand(details,scripts());
        addCommand(details,newscript());
        addCommand(details,runscript());
        addCommand(details,removescript());
        addCommand(details,tutorial());
        addCommand(details,mmrcreate());
        addCommand(details,mmrproof());
        addCommand(details,coincheck());
        addCommand(details,coinimport());
        addCommand(details,coinexport());
        addCommand(details,cointrack());
        addCommand(details,coinnotify());
        addCommand(details,sign());
        addCommand(details,verify());
        addCommand(details,txnlist());
        addCommand(details,txncreate());
        addCommand(details,txnauto());
        addCommand(details,txnaddamount());
        addCommand(details,txnbasics());
        addCommand(details,txndelete());
        addCommand(details,txncheck());
        addCommand(details,txninput());
        addCommand(details,txnoutput());
        addCommand(details,txnstate());
        addCommand(details,txnscript());
        addCommand(details,txnmmr());
        addCommand(details,txnsign());
        addCommand(details,txnclear());
        addCommand(details,txnpost());
        addCommand(details,txnimport());
        addCommand(details,txnexport());
        addCommand(details,txnview());
        addCommand(details,txnmine());
        addCommand(details,txnminepost());
        addCommand(details,network());
        addCommand(details,message());
        addCommand(details,connect());
        addCommand(details,disconnect());
        addCommand(details,rpc());
        addCommand(details,webhooks());
        addCommand(details,backup());
        addCommand(details,restore());
        addCommand(details,restoresync());
        addCommand(details,decryptbackup());
        addCommand(details,reset());
        addCommand(details,archive());
        addCommand(details,vault());
        addCommand(details,megammr());
        addCommand(details,megammrsync());
        addCommand(details,rescue());
        addCommand(details,mysql());
        addCommand(details,mysqlcoins());
        addCommand(details,quit());
    }
    result.put("response",details);
    return result;
}
void help::addCommand(json::JSONObject& details,const Command& command){
    auto name=command.getName();
    details.put(getStrOfLength(15,name?std::optional(unicode::decodeUTF8(*name)):std::nullopt),
                command.getHelp()?json::JSONValue(*command.getHelp()):json::JSONValue());
}
std::u16string help::getStrOfLength(int32_t desired,const std::optional<std::u16string>& text) const{
    if(!text)throw JavaNullPointerException("null String copy");
    auto result=*text;
    if(desired<0)throw JavaTypedException("java.lang.StringIndexOutOfBoundsException",
        "Range [0, "+std::to_string(desired)+") out of bounds for length "+std::to_string(result.size()));
    if(result.size()>=static_cast<size_t>(desired))return result.substr(0,desired);
    result.append(static_cast<size_t>(desired)-result.size(),u' ');
    return result;
}
}
