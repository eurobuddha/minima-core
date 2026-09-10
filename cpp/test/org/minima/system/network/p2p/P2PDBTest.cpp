// Native counterpart of all four pinned P2PDBTest.java test methods.
#include "org/minima/system/network/p2p/P2PDB.hpp"
using namespace minima;using namespace minima::p2p;
static void require(bool value){if(!value)throw std::runtime_error("P2PDBTest assertion");}
int main() {
    auto peers=std::make_shared<std::vector<SocketAddressPtr>>();
    for(int subnet=1;subnet<5;++subnet)for(int host=0;host<250;++host)peers->push_back(std::make_shared<InetSocketAddress>("192.168."+std::to_string(subnet)+"."+std::to_string(host),9001));
    P2PDB empty;require(empty.getPeersList()->empty());
    P2PDB filled;filled.setPeersList(peers);require(!filled.getPeersList()->empty());
    // Unique fixture directory: never overwrite a user's tmp.db.
    auto directory=std::filesystem::temp_directory_path()/("minima-peer-db-test-"+MiniData::getRandomData(12).toHexString(false));
    require(std::filesystem::create_directory(directory));auto file=directory/"tmp.db";
    try {
        P2PDB db;db.setPeersList(peers);db.saveDB(file);db.loadDB(file);require(std::filesystem::remove(file));
        auto loaded=db.getPeersList();require(!loaded->empty());require(loaded->size()==1000);
        P2PDB version;version.setVersion();version.saveDB(file);version.loadDB(file);require(std::filesystem::remove(file));require(version.getVersion()==P2PParams::VERSION);
    }catch(...){std::filesystem::remove(file);std::filesystem::remove(directory);throw;}
    require(std::filesystem::remove(directory));
}
