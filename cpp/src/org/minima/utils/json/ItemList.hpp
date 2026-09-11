#pragma once
#include "support/JavaList.hpp"
#include "org/minima/utils/messages/Message.hpp"
#include <variant>
namespace minima::json {
class ItemList {
public:
    using String=std::optional<std::u16string>;
    // Strings retain UTF-16; the existing shared object adapter handles raw List values.
    using Item=std::variant<std::u16string,MessageObject>;
    using List=JavaList<Item>;
private:
    String sp=u",";
    std::shared_ptr<List> items=std::make_shared<List>();
public:
    ItemList()=default;
    explicit ItemList(const String& s);
    ItemList(const String& s,const String& separator);
    ItemList(const String& s,const String& separator,bool multiToken);
    virtual ~ItemList()=default;
    std::shared_ptr<List> getItems() const{return items;}
    std::vector<String> getArray() const;
    void split(const String& s,const String& separator,const std::shared_ptr<List>& append,bool multiToken);
    void split(const String& s,const String& separator,const std::shared_ptr<List>& append);
    void setSP(String separator){sp=std::move(separator);}
    void add(int32_t index,const String& item);
    void add(const String& item);
    void addAll(const ItemList* list);
    void addAll(const String& s){split(s,sp,items);}
    void addAll(const String& s,const String& separator){split(s,separator,items);}
    void addAll(const String& s,const String& separator,bool multiToken){split(s,separator,items,multiToken);}
    String get(int32_t index) const;
    int32_t size() const{return int32_t(items->size());}
    std::u16string toString() const{return toString(sp);}
    std::u16string toString(const String& separator) const;
    void clear(){items->clear();}
    void reset(){sp=u",";items->clear();}
};
}
