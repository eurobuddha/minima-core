#pragma once
#include "org/minima/database/userprefs/txndb/TxnRow.hpp"
namespace minima::commands::automatic_transaction {
// Shared source algorithm: txnutils adds exclusions/link and optional signing;
// txnauto validates early, has no exclusions/link and always signs.
std::shared_ptr<TxnRow> create(const std::vector<std::string>& excluded,
    const MiniData* link,const MiniNumber* amount,bool sign,bool validateFirst);
}
