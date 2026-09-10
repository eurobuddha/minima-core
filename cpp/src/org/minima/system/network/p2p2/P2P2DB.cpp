#include "P2P2DB.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::p2p2 {
void P2P2DB::loadDB(const std::filesystem::path& file){
    MiniFile::loadObjectSlow(file,*this);
    mAllPeers->clear();
    auto array=getJSONArray("all_known_peers");
    // Cast and append in order: a later non-string leaves earlier peers loaded.
    for(const auto& value:array->elements())mAllPeers->push_back(value.isNull()?std::nullopt:std::optional(value.asString()));
}
void P2P2DB::saveDB(const std::filesystem::path& file){
    setJSONArray("all_known_peers",MiniUtil::convertArrayList(*mAllPeers));
    MiniFile::saveObjectDirect(file,*this);
}
bool P2P2DB::addPeerToAllKnown(const std::optional<std::string>& peer){
    for(const auto& existing:*mAllPeers)if((!peer&&!existing)||(peer&&existing&&unicode::decodeUTF8(*peer)==unicode::decodeUTF8(*existing))){std::clog<<"Peer already added : "<<peer.value_or("null")<<'\n';return false;}
    mAllPeers->push_back(peer);std::clog<<"Peer added : "<<peer.value_or("null")<<'\n';return true;
}
}
