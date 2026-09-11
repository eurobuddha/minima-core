#pragma once
#include "MySQLConnect.hpp"
#include "org/minima/system/network/rpc/HTTPServer.hpp"
#include "support/ConsoleInput.hpp"
namespace minima {
class ArchiveServer:public HTTPServer {
public:
    using Text=std::optional<std::string>;
    using Args=std::vector<std::optional<std::u16string>>;
    using Blocks=std::vector<std::shared_ptr<TxBlock>>;
    struct Runtime {
        MySQLConnectServices mysql=MySQLConnect::runtimeServices;
        std::function<void(int64_t)> sleep;
        std::function<void()> loadDriver;
        std::function<Text(const std::string&)> environment;
        std::function<std::shared_ptr<ConsoleInput>()> openInput;
        std::function<void(const std::string&)> println,printStackTrace;
    };
    std::shared_ptr<MySQLConnect> mMySQL;
    std::shared_ptr<Cascade> mCascade;
    class ArchiveHandler {
        std::shared_ptr<ArchiveServer> mServer;
        std::shared_ptr<PosixNIOSocket> mSocket;
        std::recursive_mutex mMonitor;
    public:
        ArchiveHandler(std::shared_ptr<ArchiveServer> server,std::shared_ptr<PosixNIOSocket> socket):mServer(std::move(server)),mSocket(std::move(socket)){}
        virtual ~ArchiveHandler()=default;
        virtual std::shared_ptr<Blocks> reconnectLoadTxBlocks(const MiniNumber* firstBlock);
        virtual void run();
    };
    static void SystemClean();
    static std::shared_ptr<ArchiveServer> create(int32_t port,Text host,Text database,Text user,Text password,Runtime runtime);
    static std::shared_ptr<ArchiveServer> create(int32_t port,Text host,Text database,Text user,Text password){return create(port,std::move(host),std::move(database),std::move(user),std::move(password),Runtime{});}
    std::function<void()> getSocketHandler(const std::shared_ptr<PosixNIOSocket>& socket) override;
    static int main(const Args* args,Runtime runtime);
    static int main(const Args& args,Runtime runtime){return main(&args,std::move(runtime));}
    static int main(const Args& args){return main(&args,Runtime{});}
protected:
    explicit ArchiveServer(int32_t port,Runtime runtime);
private:
    Runtime mRuntime;
    inline static std::recursive_mutex mCleanMonitor;
    inline static int64_t mLastClean=0;
    static Runtime configured(Runtime runtime);
    friend class ArchiveServerFixture;
};
}
