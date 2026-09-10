#include "PeerHarness.hpp"
#include "org/minima/system/commands/network/nodecount.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/network/p2p/params/P2PParams.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NativeThrowable.hpp"
#include "support/JavaParseDouble.hpp"
namespace minima {
class NIOManagerFixture {
public:static std::shared_ptr<NIOManager> create(){NIOManagerServices services;services.releaseLocks=[]{};return std::shared_ptr<NIOManager>(new NIOManager(services));}
};
}
static JSONValue run(const JSONObject& request,const std::string& root){
    std::filesystem::remove_all(root);std::filesystem::create_directories(root);GeneralParams::BASE_FILE_FOLDER=root;
    auto manager=NIOManagerFixture::create();NIOManager::runtimeLookup=[manager]{return manager;};
    JSONArray logs;MinimaLogger::Runtime logger;logger.println=[&](const auto& text){auto line=unicode::encodeUTF8(text);auto at=line.find("] : ");if(at!=line.npos)logs.add(line.substr(at+4));};MinimaLogger::setRuntime(logger);
    struct Reset{~Reset(){NIOManager::runtimeLookup={};MinimaLogger::setRuntime({});}}reset;JSONObject out;
    try{
        auto mode=request.getString("mode");
        if(mode=="double"){
            auto value=javaParseDouble(request.getString("text"));out.put("bits",std::to_string(std::bit_cast<uint64_t>(value)));out.put("text",javaFloating(value));
        }else{
            if(request.containsKey("directory"))std::filesystem::create_directories(std::filesystem::path(root)/request.getString("directory"));
            if(request.containsKey("initial"))MiniFile::writeDataToFile(std::filesystem::path(root)/"scan.csv",MiniData::fromHex(request.getString("initial")).bytes());
            P2PParams::DEFAULT_NODE_LIST=request.get("defaults").isNull()?nullptr:std::make_shared<JavaList<SocketAddressPtr>>(*list(request.get("defaults")),false);
            if(mode=="run")out.put("value",CommandRunner::getRunner().runMultiCommand(request.getString("caller"),request.getString("text")));
            else{auto command=std::make_shared<commands::nodecount>();for(const auto& [key,value]:object(request.get("params"))->entries())command->getParams()->put(key,value);
                if(mode=="meta"){JSONObject meta;meta.put("name",command->getName().value());meta.put("help",command->getHelp().value());meta.put("fullhelp",command->getFullHelp().value());JSONArray valid;for(const auto& v:command->getValidParams())valid.add(v);meta.put("valid",valid);meta.put("fresh",command->getFunction()->getJSONReply());out.put("value",meta);}
                else out.put("value",command->runCommand());
            }
        }
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("logs",logs);out.put("traffic",manager->getTrafficListener()->getBreakdown());out.put("read",manager->getTrafficListener()->getTotalRead());out.put("write",manager->getTrafficListener()->getTotalWrite());
    JSONObject files;JSONArray dirs;for(const auto& entry:std::filesystem::recursive_directory_iterator(root))if(entry.is_regular_file())files.put(entry.path().lexically_relative(root).string(),MiniData(MiniFile::readCompleteFile(entry.path())).to0xString());else if(entry.is_directory())dirs.add(entry.path().lexically_relative(root).string());out.put("files",files);out.put("directories",dirs);return out;
}
int main(int argc,char** argv){if(argc!=2)return 2;GeneralParams::NETWORKING_LOGS=false;std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(*object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
