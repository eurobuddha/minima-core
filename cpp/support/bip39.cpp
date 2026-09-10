#include "org/minima/utils/BIP39.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaUpperCase.hpp"
#include <iostream>
using namespace minima;
static std::optional<std::string> text(const json::JSONValue& value){return value.isNull()?std::nullopt:std::optional(value.asString());}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){auto saved=BIP39::WORD_LIST;try{auto data=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);auto op=data->get("op").asString(),language=data->get("locale").asString();if(data->containsKey("replace")){auto change=std::get<std::shared_ptr<json::JSONArray>>(data->get("replace").value);BIP39::WORD_LIST.at(size_t(std::get<int64_t>(change->get(0).value)))=text(change->get(1));}std::string result;
    if(op=="clean")result=BIP39::cleanSeedPhrase(text(data->get("text")),language);
    else if(op=="seed")result=BIP39::convertStringToSeed(text(data->get("text"))).to0xString();
    else if(op=="list"||op=="listseed"){auto list=std::get<std::shared_ptr<json::JSONArray>>(data->get("words").value);if(!list)throw std::runtime_error("Null list");BIP39::Words words;for(size_t i=0;i<list->size();++i)words.push_back(text(list->get(i)));result=op=="list"?BIP39::convertWordListToString(words):BIP39::convertWordListToSeed(words).to0xString();}
    else if(op=="words"){json::JSONArray words;for(const auto& word:BIP39::WORD_LIST)words.add(word?json::JSONValue(*word):json::JSONValue());std::cout<<words.toJSONString()<<'\n';BIP39::WORD_LIST=std::move(saved);continue;}
    else {auto words=BIP39::getNewWordList(language);if(words.size()!=24)throw std::runtime_error("Wrong length");for(const auto& word:words){bool found=false;for(const auto& candidate:BIP39::WORD_LIST)if(javaUpperCase(candidate.value(),language)==word){found=true;break;}if(!found)throw std::runtime_error("Wrong word");}result="VALID";}
    std::cout<<json::JSONValue(result).toJSONString()<<'\n';
}catch(const std::invalid_argument& e){std::cout<<json::JSONValue(std::string("ERROR:")+e.what()).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"\"ERROR\"\n";}BIP39::WORD_LIST=std::move(saved);}}
