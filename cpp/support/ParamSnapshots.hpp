// Fixture reflection equivalent for the actual native static fields.
static JSONObject snapshotGeneralParams() {JSONObject out;
    out.put("IS_MOBILE",GeneralParams::IS_MOBILE);
    out.put("IS_DESKTOP",GeneralParams::IS_DESKTOP);
    out.put("IS_JNLP",GeneralParams::IS_JNLP);
    out.put("IS_ACCEPTING_IN_LINKS",GeneralParams::IS_ACCEPTING_IN_LINKS);
    out.put("PRIVATE",GeneralParams::PRIVATE);
    out.put("GENESIS",GeneralParams::GENESIS);
    out.put("CLEAN",GeneralParams::CLEAN);
    out.put("ARCHIVE",GeneralParams::ARCHIVE);
    out.put("TXBLOCK_NODE",GeneralParams::TXBLOCK_NODE);
    out.put("IS_MAIN_DBPASSWORD_SET",GeneralParams::IS_MAIN_DBPASSWORD_SET);
    out.put("MAIN_DBPASSWORD",GeneralParams::MAIN_DBPASSWORD);
    out.put("DATA_FOLDER",GeneralParams::DATA_FOLDER);
    out.put("BASE_FILE_FOLDER",GeneralParams::BASE_FILE_FOLDER);
    out.put("MINIMA_HOST",GeneralParams::MINIMA_HOST);
    out.put("IS_HOST_SET",GeneralParams::IS_HOST_SET);
    out.put("MINIMA_PORT",GeneralParams::MINIMA_PORT);
    out.put("MDSFILE_PORT",GeneralParams::MDSFILE_PORT);
    out.put("MDSCOMMAND_PORT",GeneralParams::MDSCOMMAND_PORT);
    out.put("RPC_PORT",GeneralParams::RPC_PORT);
    out.put("MDS_PASSWORD",GeneralParams::MDS_PASSWORD);
    out.put("MDS_INITFOLDER",GeneralParams::MDS_INITFOLDER);
    out.put("MDS_WRITE",GeneralParams::MDS_WRITE);
    out.put("MDS_ENABLED",GeneralParams::MDS_ENABLED);
    out.put("TEST_PARAMS",GeneralParams::TEST_PARAMS);
    out.put("P2P_ENABLED",GeneralParams::P2P_ENABLED);
    out.put("P2P_ROOTNODE",GeneralParams::P2P_ROOTNODE);
    out.put("P2P_ADDNODES",GeneralParams::P2P_ADDNODES);
    out.put("P2P2_ENABLED",GeneralParams::P2P2_ENABLED);
    out.put("ALLOW_ALL_IP",GeneralParams::ALLOW_ALL_IP);
    out.put("NOCONNECT",GeneralParams::NOCONNECT);
    out.put("CONNECT_LIST",GeneralParams::CONNECT_LIST);
    out.put("SHOW_PARAMS",GeneralParams::SHOW_PARAMS);
    out.put("NO_SYNC_IBD",GeneralParams::NO_SYNC_IBD);
    out.put("MAX_RELAY_OUTPUTCOINS",GeneralParams::MAX_RELAY_OUTPUTCOINS);
    out.put("MAX_RELAY_STORESTATESIZE",GeneralParams::MAX_RELAY_STORESTATESIZE);
    out.put("MAX_SPLIT_COINS",GeneralParams::MAX_SPLIT_COINS.toString());
    out.put("NUMBER_DAYS_SQLTXPOWDB",GeneralParams::NUMBER_DAYS_SQLTXPOWDB);
    out.put("NUMBER_HOURS_RAMTXPOWDB",GeneralParams::NUMBER_HOURS_RAMTXPOWDB);
    out.put("NUMBER_DAYS_ARCHIVE",GeneralParams::NUMBER_DAYS_ARCHIVE);
    out.put("USER_PULSE_FREQ",GeneralParams::USER_PULSE_FREQ);
    out.put("DEBUGFLAG",GeneralParams::DEBUGFLAG);
    out.put("DEBUGVAR",GeneralParams::DEBUGVAR);
    out.put("SCRIPTLOGS",GeneralParams::SCRIPTLOGS.load());
    out.put("MINING_LOGS",GeneralParams::MINING_LOGS.load());
    out.put("MAXIMA_LOGS",GeneralParams::MAXIMA_LOGS.load());
    out.put("NETWORKING_LOGS",GeneralParams::NETWORKING_LOGS.load());
    out.put("IBDSYNC_LOGS",GeneralParams::IBDSYNC_LOGS.load());
    out.put("BLOCK_LOGS",GeneralParams::BLOCK_LOGS.load());
    out.put("ARCHIVESYNC_LIMIT_BANDWIDTH",GeneralParams::ARCHIVESYNC_LIMIT_BANDWIDTH);
    out.put("RPC_CRLF",GeneralParams::RPC_CRLF);
    out.put("DEFAULT_MINIDAPPS",GeneralParams::DEFAULT_MINIDAPPS);
    out.put("RPC_ENABLED",GeneralParams::RPC_ENABLED);
    out.put("RPC_AUTHENTICATE",GeneralParams::RPC_AUTHENTICATE);
    out.put("RPC_AUTHSTYLE",GeneralParams::RPC_AUTHSTYLE);
    out.put("RPC_PASSWORD",GeneralParams::RPC_PASSWORD);
    out.put("SEED_PHRASE",GeneralParams::SEED_PHRASE);
    out.put("ANYSEED_PHRASE",GeneralParams::ANYSEED_PHRASE);
    out.put("PEERSCHECKER_lOG",GeneralParams::PEERSCHECKER_lOG.load());
    out.put("IS_MEGAMMR",GeneralParams::IS_MEGAMMR);
    out.put("NOTIFY_ALL_TXPOW",GeneralParams::NOTIFY_ALL_TXPOW);
    out.put("RESCUE_MEGAMMR_NODE",GeneralParams::RESCUE_MEGAMMR_NODE);
    out.put("MDS_NOSSL",GeneralParams::MDS_NOSSL);
    out.put("MYSQL_DB_DETAILS",GeneralParams::MYSQL_DB_DETAILS);
    out.put("MYSQL_DB_DELAY",GeneralParams::MYSQL_DB_DELAY);
    out.put("MYSQL_DB_COINS",GeneralParams::MYSQL_DB_COINS);
    out.put("MYSQL_STORE_ALLTXPOW",GeneralParams::MYSQL_STORE_ALLTXPOW);
    out.put("PUBLICMDS_ENABLE",GeneralParams::PUBLICMDS_ENABLE);
    out.put("PUBLICMDS_SESSION_UID",GeneralParams::PUBLICMDS_SESSION_UID);
    out.put("MEGAMMR_MEGAPRUNE",GeneralParams::MEGAMMR_MEGAPRUNE);
    out.put("MEGAMMR_MEGAPRUNE_STATE",GeneralParams::MEGAMMR_MEGAPRUNE_STATE);
    out.put("MEGAMMR_MEGAPRUNE_TOKENS",GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS);
    out.put("SHOW_NETWORK_CALLS",GeneralParams::SHOW_NETWORK_CALLS);
    out.put("SHOW_NETWORK_POLLS",GeneralParams::SHOW_NETWORK_POLLS);
    out.put("INIT_HASHTEST_AMOUNT",GeneralParams::INIT_HASHTEST_AMOUNT);
    out.put("GENERATE_SSL",GeneralParams::GENERATE_SSL);
    out.put("LOAD_JNI",GeneralParams::LOAD_JNI);
    out.put("DB_IGNORE_LOCKS",GeneralParams::DB_IGNORE_LOCKS);
    out.put("ALLOW_GEN_MESSAGE",GeneralParams::ALLOW_GEN_MESSAGE);
    out.put("USE_SQL_COINDB",GeneralParams::USE_SQL_COINDB);
    out.put("LIMIT_IBD_30MINS",GeneralParams::LIMIT_IBD_30MINS);
    return out;
}
static void setGeneralParams(const std::string& name,const JSONValue& value) {
    if(name=="IS_MOBILE"){GeneralParams::IS_MOBILE=value.asBool();return;}
    if(name=="IS_DESKTOP"){GeneralParams::IS_DESKTOP=value.asBool();return;}
    if(name=="IS_JNLP"){GeneralParams::IS_JNLP=value.asBool();return;}
    if(name=="IS_ACCEPTING_IN_LINKS"){GeneralParams::IS_ACCEPTING_IN_LINKS=value.asBool();return;}
    if(name=="PRIVATE"){GeneralParams::PRIVATE=value.asBool();return;}
    if(name=="GENESIS"){GeneralParams::GENESIS=value.asBool();return;}
    if(name=="CLEAN"){GeneralParams::CLEAN=value.asBool();return;}
    if(name=="ARCHIVE"){GeneralParams::ARCHIVE=value.asBool();return;}
    if(name=="TXBLOCK_NODE"){GeneralParams::TXBLOCK_NODE=value.asBool();return;}
    if(name=="IS_MAIN_DBPASSWORD_SET"){GeneralParams::IS_MAIN_DBPASSWORD_SET=value.asBool();return;}
    if(name=="MAIN_DBPASSWORD"){GeneralParams::MAIN_DBPASSWORD=value.asString();return;}
    if(name=="DATA_FOLDER"){GeneralParams::DATA_FOLDER=value.asString();return;}
    if(name=="BASE_FILE_FOLDER"){GeneralParams::BASE_FILE_FOLDER=value.asString();return;}
    if(name=="MINIMA_HOST"){GeneralParams::MINIMA_HOST=value.asString();return;}
    if(name=="IS_HOST_SET"){GeneralParams::IS_HOST_SET=value.asBool();return;}
    if(name=="MINIMA_PORT"){GeneralParams::MINIMA_PORT=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MDSFILE_PORT"){GeneralParams::MDSFILE_PORT=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MDSCOMMAND_PORT"){GeneralParams::MDSCOMMAND_PORT=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="RPC_PORT"){GeneralParams::RPC_PORT=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MDS_PASSWORD"){GeneralParams::MDS_PASSWORD=value.asString();return;}
    if(name=="MDS_INITFOLDER"){GeneralParams::MDS_INITFOLDER=value.asString();return;}
    if(name=="MDS_WRITE"){GeneralParams::MDS_WRITE=value.asString();return;}
    if(name=="MDS_ENABLED"){GeneralParams::MDS_ENABLED=value.asBool();return;}
    if(name=="TEST_PARAMS"){GeneralParams::TEST_PARAMS=value.asBool();return;}
    if(name=="P2P_ENABLED"){GeneralParams::P2P_ENABLED=value.asBool();return;}
    if(name=="P2P_ROOTNODE"){GeneralParams::P2P_ROOTNODE=value.asString();return;}
    if(name=="P2P_ADDNODES"){GeneralParams::P2P_ADDNODES=value.asString();return;}
    if(name=="P2P2_ENABLED"){GeneralParams::P2P2_ENABLED=value.asBool();return;}
    if(name=="ALLOW_ALL_IP"){GeneralParams::ALLOW_ALL_IP=value.asBool();return;}
    if(name=="NOCONNECT"){GeneralParams::NOCONNECT=value.asBool();return;}
    if(name=="CONNECT_LIST"){GeneralParams::CONNECT_LIST=value.asString();return;}
    if(name=="SHOW_PARAMS"){GeneralParams::SHOW_PARAMS=value.asBool();return;}
    if(name=="NO_SYNC_IBD"){GeneralParams::NO_SYNC_IBD=value.asBool();return;}
    if(name=="MAX_RELAY_OUTPUTCOINS"){GeneralParams::MAX_RELAY_OUTPUTCOINS=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MAX_RELAY_STORESTATESIZE"){GeneralParams::MAX_RELAY_STORESTATESIZE=std::get<int64_t>(value.value);return;}
    if(name=="MAX_SPLIT_COINS"){GeneralParams::MAX_SPLIT_COINS=MiniNumber(value.asString());return;}
    if(name=="NUMBER_DAYS_SQLTXPOWDB"){GeneralParams::NUMBER_DAYS_SQLTXPOWDB=std::get<int64_t>(value.value);return;}
    if(name=="NUMBER_HOURS_RAMTXPOWDB"){GeneralParams::NUMBER_HOURS_RAMTXPOWDB=std::get<int64_t>(value.value);return;}
    if(name=="NUMBER_DAYS_ARCHIVE"){GeneralParams::NUMBER_DAYS_ARCHIVE=std::get<int64_t>(value.value);return;}
    if(name=="USER_PULSE_FREQ"){GeneralParams::USER_PULSE_FREQ=std::get<int64_t>(value.value);return;}
    if(name=="DEBUGFLAG"){GeneralParams::DEBUGFLAG=value.asBool();return;}
    if(name=="DEBUGVAR"){GeneralParams::DEBUGVAR=value.asString();return;}
    if(name=="SCRIPTLOGS"){GeneralParams::SCRIPTLOGS=value.asBool();return;}
    if(name=="MINING_LOGS"){GeneralParams::MINING_LOGS=value.asBool();return;}
    if(name=="MAXIMA_LOGS"){GeneralParams::MAXIMA_LOGS=value.asBool();return;}
    if(name=="NETWORKING_LOGS"){GeneralParams::NETWORKING_LOGS=value.asBool();return;}
    if(name=="IBDSYNC_LOGS"){GeneralParams::IBDSYNC_LOGS=value.asBool();return;}
    if(name=="BLOCK_LOGS"){GeneralParams::BLOCK_LOGS=value.asBool();return;}
    if(name=="ARCHIVESYNC_LIMIT_BANDWIDTH"){GeneralParams::ARCHIVESYNC_LIMIT_BANDWIDTH=value.asBool();return;}
    if(name=="RPC_CRLF"){GeneralParams::RPC_CRLF=value.asBool();return;}
    if(name=="DEFAULT_MINIDAPPS"){GeneralParams::DEFAULT_MINIDAPPS=value.asBool();return;}
    if(name=="RPC_ENABLED"){GeneralParams::RPC_ENABLED=value.asBool();return;}
    if(name=="RPC_AUTHENTICATE"){GeneralParams::RPC_AUTHENTICATE=value.asBool();return;}
    if(name=="RPC_AUTHSTYLE"){GeneralParams::RPC_AUTHSTYLE=value.asString();return;}
    if(name=="RPC_PASSWORD"){GeneralParams::RPC_PASSWORD=value.asString();return;}
    if(name=="SEED_PHRASE"){GeneralParams::SEED_PHRASE=value.asString();return;}
    if(name=="ANYSEED_PHRASE"){GeneralParams::ANYSEED_PHRASE=value.asBool();return;}
    if(name=="PEERSCHECKER_lOG"){GeneralParams::PEERSCHECKER_lOG=value.asBool();return;}
    if(name=="IS_MEGAMMR"){GeneralParams::IS_MEGAMMR=value.asBool();return;}
    if(name=="NOTIFY_ALL_TXPOW"){GeneralParams::NOTIFY_ALL_TXPOW=value.asBool();return;}
    if(name=="RESCUE_MEGAMMR_NODE"){GeneralParams::RESCUE_MEGAMMR_NODE=value.asString();return;}
    if(name=="MDS_NOSSL"){GeneralParams::MDS_NOSSL=value.asBool();return;}
    if(name=="MYSQL_DB_DETAILS"){GeneralParams::MYSQL_DB_DETAILS=value.asString();return;}
    if(name=="MYSQL_DB_DELAY"){GeneralParams::MYSQL_DB_DELAY=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MYSQL_DB_COINS"){GeneralParams::MYSQL_DB_COINS=value.asBool();return;}
    if(name=="MYSQL_STORE_ALLTXPOW"){GeneralParams::MYSQL_STORE_ALLTXPOW=value.asBool();return;}
    if(name=="PUBLICMDS_ENABLE"){GeneralParams::PUBLICMDS_ENABLE=value.asBool();return;}
    if(name=="PUBLICMDS_SESSION_UID"){GeneralParams::PUBLICMDS_SESSION_UID=value.asString();return;}
    if(name=="MEGAMMR_MEGAPRUNE"){GeneralParams::MEGAMMR_MEGAPRUNE=value.asBool();return;}
    if(name=="MEGAMMR_MEGAPRUNE_STATE"){GeneralParams::MEGAMMR_MEGAPRUNE_STATE=value.asBool();return;}
    if(name=="MEGAMMR_MEGAPRUNE_TOKENS"){GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS=value.asBool();return;}
    if(name=="SHOW_NETWORK_CALLS"){GeneralParams::SHOW_NETWORK_CALLS=value.asBool();return;}
    if(name=="SHOW_NETWORK_POLLS"){GeneralParams::SHOW_NETWORK_POLLS=value.asBool();return;}
    if(name=="INIT_HASHTEST_AMOUNT"){GeneralParams::INIT_HASHTEST_AMOUNT=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="GENERATE_SSL"){GeneralParams::GENERATE_SSL=value.asBool();return;}
    if(name=="LOAD_JNI"){GeneralParams::LOAD_JNI=value.asBool();return;}
    if(name=="DB_IGNORE_LOCKS"){GeneralParams::DB_IGNORE_LOCKS=value.asBool();return;}
    if(name=="ALLOW_GEN_MESSAGE"){GeneralParams::ALLOW_GEN_MESSAGE=value.asBool();return;}
    if(name=="USE_SQL_COINDB"){GeneralParams::USE_SQL_COINDB=value.asBool();return;}
    if(name=="LIMIT_IBD_30MINS"){GeneralParams::LIMIT_IBD_30MINS=value.asBool();return;}
    throw std::invalid_argument(name);
}
static JSONObject snapshotGlobalParams() {JSONObject out;
    out.put("MINIMA_BASE_VERSION",GlobalParams::MINIMA_BASE_VERSION);
    out.put("MINIMA_BUILD_NUMBER",GlobalParams::MINIMA_BUILD_NUMBER);
    out.put("MINIMA_VERSION",GlobalParams::MINIMA_VERSION);
    out.put("MINIMA_MICRO_BUILD",GlobalParams::MINIMA_MICRO_BUILD);
    out.put("MINIMA_BLOCK_SPEED",GlobalParams::MINIMA_BLOCK_SPEED.toString());
    out.put("MINIMA_BLOCKS_SPEED_CALC",GlobalParams::MINIMA_BLOCKS_SPEED_CALC.toString());
    out.put("MINIMA_CONFIRM_DEPTH",GlobalParams::MINIMA_CONFIRM_DEPTH.toString());
    out.put("MINIMA_CASCADE_FREQUENCY",GlobalParams::MINIMA_CASCADE_FREQUENCY.toString());
    out.put("MINIMA_CASCADE_START_DEPTH",GlobalParams::MINIMA_CASCADE_START_DEPTH.toString());
    out.put("MINIMA_CASCADE_LEVEL_NODES",GlobalParams::MINIMA_CASCADE_LEVEL_NODES);
    out.put("MINIMA_CASCADE_LEVELS",GlobalParams::MINIMA_CASCADE_LEVELS);
    out.put("MINIMA_MMR_PROOF_HISTORY",GlobalParams::MINIMA_MMR_PROOF_HISTORY.toString());
    out.put("MEDIAN_BLOCK_CALC",GlobalParams::MEDIAN_BLOCK_CALC);
    return out;
}
static void setGlobalParams(const std::string& name,const JSONValue& value) {
    if(name=="MINIMA_BASE_VERSION"){GlobalParams::MINIMA_BASE_VERSION=value.asString();return;}
    if(name=="MINIMA_BUILD_NUMBER"){GlobalParams::MINIMA_BUILD_NUMBER=value.asString();return;}
    if(name=="MINIMA_VERSION"){GlobalParams::MINIMA_VERSION=value.asString();return;}
    if(name=="MINIMA_MICRO_BUILD"){GlobalParams::MINIMA_MICRO_BUILD=value.asString();return;}
    if(name=="MINIMA_BLOCK_SPEED"){GlobalParams::MINIMA_BLOCK_SPEED=MiniNumber(value.asString());return;}
    if(name=="MINIMA_BLOCKS_SPEED_CALC"){GlobalParams::MINIMA_BLOCKS_SPEED_CALC=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CONFIRM_DEPTH"){GlobalParams::MINIMA_CONFIRM_DEPTH=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CASCADE_FREQUENCY"){GlobalParams::MINIMA_CASCADE_FREQUENCY=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CASCADE_START_DEPTH"){GlobalParams::MINIMA_CASCADE_START_DEPTH=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CASCADE_LEVEL_NODES"){GlobalParams::MINIMA_CASCADE_LEVEL_NODES=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MINIMA_CASCADE_LEVELS"){GlobalParams::MINIMA_CASCADE_LEVELS=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MINIMA_MMR_PROOF_HISTORY"){GlobalParams::MINIMA_MMR_PROOF_HISTORY=MiniNumber(value.asString());return;}
    if(name=="MEDIAN_BLOCK_CALC"){GlobalParams::MEDIAN_BLOCK_CALC=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    throw std::invalid_argument(name);
}
static JSONObject snapshotTestParams() {JSONObject out;
    out.put("MINIMA_VERSION",TestParams::values().MINIMA_VERSION);
    out.put("MINIMA_BLOCK_SPEED",TestParams::values().MINIMA_BLOCK_SPEED.toString());
    out.put("MINIMA_BLOCKS_SPEED_CALC",TestParams::values().MINIMA_BLOCKS_SPEED_CALC.toString());
    out.put("MINIMA_CONFIRM_DEPTH",TestParams::values().MINIMA_CONFIRM_DEPTH.toString());
    out.put("MINIMA_CASCADE_FREQUENCY",TestParams::values().MINIMA_CASCADE_FREQUENCY.toString());
    out.put("MINIMA_CASCADE_START_DEPTH",TestParams::values().MINIMA_CASCADE_START_DEPTH.toString());
    out.put("MINIMA_CASCADE_LEVEL_NODES",TestParams::values().MINIMA_CASCADE_LEVEL_NODES);
    out.put("MINIMA_CASCADE_LEVELS",TestParams::values().MINIMA_CASCADE_LEVELS);
    out.put("MINIMA_MMR_PROOF_HISTORY",TestParams::values().MINIMA_MMR_PROOF_HISTORY.toString());
    out.put("MEDIAN_BLOCK_CALC",TestParams::values().MEDIAN_BLOCK_CALC);
    return out;
}
static void setTestParams(const std::string& name,const JSONValue& value) {
    if(name=="MINIMA_VERSION"){TestParams::values().MINIMA_VERSION=value.asString();return;}
    if(name=="MINIMA_BLOCK_SPEED"){TestParams::values().MINIMA_BLOCK_SPEED=MiniNumber(value.asString());return;}
    if(name=="MINIMA_BLOCKS_SPEED_CALC"){TestParams::values().MINIMA_BLOCKS_SPEED_CALC=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CONFIRM_DEPTH"){TestParams::values().MINIMA_CONFIRM_DEPTH=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CASCADE_FREQUENCY"){TestParams::values().MINIMA_CASCADE_FREQUENCY=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CASCADE_START_DEPTH"){TestParams::values().MINIMA_CASCADE_START_DEPTH=MiniNumber(value.asString());return;}
    if(name=="MINIMA_CASCADE_LEVEL_NODES"){TestParams::values().MINIMA_CASCADE_LEVEL_NODES=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MINIMA_CASCADE_LEVELS"){TestParams::values().MINIMA_CASCADE_LEVELS=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    if(name=="MINIMA_MMR_PROOF_HISTORY"){TestParams::values().MINIMA_MMR_PROOF_HISTORY=MiniNumber(value.asString());return;}
    if(name=="MEDIAN_BLOCK_CALC"){TestParams::values().MEDIAN_BLOCK_CALC=std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));return;}
    throw std::invalid_argument(name);
}
static auto defaults(){std::vector<std::function<void()>> result;
    {auto saved=GeneralParams::IS_MOBILE;result.push_back([saved](){GeneralParams::IS_MOBILE=saved;});}
    {auto saved=GeneralParams::IS_DESKTOP;result.push_back([saved](){GeneralParams::IS_DESKTOP=saved;});}
    {auto saved=GeneralParams::IS_JNLP;result.push_back([saved](){GeneralParams::IS_JNLP=saved;});}
    {auto saved=GeneralParams::IS_ACCEPTING_IN_LINKS;result.push_back([saved](){GeneralParams::IS_ACCEPTING_IN_LINKS=saved;});}
    {auto saved=GeneralParams::PRIVATE;result.push_back([saved](){GeneralParams::PRIVATE=saved;});}
    {auto saved=GeneralParams::GENESIS;result.push_back([saved](){GeneralParams::GENESIS=saved;});}
    {auto saved=GeneralParams::CLEAN;result.push_back([saved](){GeneralParams::CLEAN=saved;});}
    {auto saved=GeneralParams::ARCHIVE;result.push_back([saved](){GeneralParams::ARCHIVE=saved;});}
    {auto saved=GeneralParams::TXBLOCK_NODE;result.push_back([saved](){GeneralParams::TXBLOCK_NODE=saved;});}
    {auto saved=GeneralParams::IS_MAIN_DBPASSWORD_SET;result.push_back([saved](){GeneralParams::IS_MAIN_DBPASSWORD_SET=saved;});}
    {auto saved=GeneralParams::MAIN_DBPASSWORD;result.push_back([saved](){GeneralParams::MAIN_DBPASSWORD=saved;});}
    {auto saved=GeneralParams::DATA_FOLDER;result.push_back([saved](){GeneralParams::DATA_FOLDER=saved;});}
    {auto saved=GeneralParams::BASE_FILE_FOLDER;result.push_back([saved](){GeneralParams::BASE_FILE_FOLDER=saved;});}
    {auto saved=GeneralParams::MINIMA_HOST;result.push_back([saved](){GeneralParams::MINIMA_HOST=saved;});}
    {auto saved=GeneralParams::IS_HOST_SET;result.push_back([saved](){GeneralParams::IS_HOST_SET=saved;});}
    {auto saved=GeneralParams::MINIMA_PORT;result.push_back([saved](){GeneralParams::MINIMA_PORT=saved;});}
    {auto saved=GeneralParams::MDSFILE_PORT;result.push_back([saved](){GeneralParams::MDSFILE_PORT=saved;});}
    {auto saved=GeneralParams::MDSCOMMAND_PORT;result.push_back([saved](){GeneralParams::MDSCOMMAND_PORT=saved;});}
    {auto saved=GeneralParams::RPC_PORT;result.push_back([saved](){GeneralParams::RPC_PORT=saved;});}
    {auto saved=GeneralParams::MDS_PASSWORD;result.push_back([saved](){GeneralParams::MDS_PASSWORD=saved;});}
    {auto saved=GeneralParams::MDS_INITFOLDER;result.push_back([saved](){GeneralParams::MDS_INITFOLDER=saved;});}
    {auto saved=GeneralParams::MDS_WRITE;result.push_back([saved](){GeneralParams::MDS_WRITE=saved;});}
    {auto saved=GeneralParams::MDS_ENABLED;result.push_back([saved](){GeneralParams::MDS_ENABLED=saved;});}
    {auto saved=GeneralParams::TEST_PARAMS;result.push_back([saved](){GeneralParams::TEST_PARAMS=saved;});}
    {auto saved=GeneralParams::P2P_ENABLED;result.push_back([saved](){GeneralParams::P2P_ENABLED=saved;});}
    {auto saved=GeneralParams::P2P_ROOTNODE;result.push_back([saved](){GeneralParams::P2P_ROOTNODE=saved;});}
    {auto saved=GeneralParams::P2P_ADDNODES;result.push_back([saved](){GeneralParams::P2P_ADDNODES=saved;});}
    {auto saved=GeneralParams::P2P2_ENABLED;result.push_back([saved](){GeneralParams::P2P2_ENABLED=saved;});}
    {auto saved=GeneralParams::ALLOW_ALL_IP;result.push_back([saved](){GeneralParams::ALLOW_ALL_IP=saved;});}
    {auto saved=GeneralParams::NOCONNECT;result.push_back([saved](){GeneralParams::NOCONNECT=saved;});}
    {auto saved=GeneralParams::CONNECT_LIST;result.push_back([saved](){GeneralParams::CONNECT_LIST=saved;});}
    {auto saved=GeneralParams::SHOW_PARAMS;result.push_back([saved](){GeneralParams::SHOW_PARAMS=saved;});}
    {auto saved=GeneralParams::NO_SYNC_IBD;result.push_back([saved](){GeneralParams::NO_SYNC_IBD=saved;});}
    {auto saved=GeneralParams::MAX_RELAY_OUTPUTCOINS;result.push_back([saved](){GeneralParams::MAX_RELAY_OUTPUTCOINS=saved;});}
    {auto saved=GeneralParams::MAX_RELAY_STORESTATESIZE;result.push_back([saved](){GeneralParams::MAX_RELAY_STORESTATESIZE=saved;});}
    {auto saved=GeneralParams::MAX_SPLIT_COINS;result.push_back([saved](){GeneralParams::MAX_SPLIT_COINS=saved;});}
    {auto saved=GeneralParams::NUMBER_DAYS_SQLTXPOWDB;result.push_back([saved](){GeneralParams::NUMBER_DAYS_SQLTXPOWDB=saved;});}
    {auto saved=GeneralParams::NUMBER_HOURS_RAMTXPOWDB;result.push_back([saved](){GeneralParams::NUMBER_HOURS_RAMTXPOWDB=saved;});}
    {auto saved=GeneralParams::NUMBER_DAYS_ARCHIVE;result.push_back([saved](){GeneralParams::NUMBER_DAYS_ARCHIVE=saved;});}
    {auto saved=GeneralParams::USER_PULSE_FREQ;result.push_back([saved](){GeneralParams::USER_PULSE_FREQ=saved;});}
    {auto saved=GeneralParams::DEBUGFLAG;result.push_back([saved](){GeneralParams::DEBUGFLAG=saved;});}
    {auto saved=GeneralParams::DEBUGVAR;result.push_back([saved](){GeneralParams::DEBUGVAR=saved;});}
    {auto saved=GeneralParams::SCRIPTLOGS.load();result.push_back([saved](){GeneralParams::SCRIPTLOGS=saved;});}
    {auto saved=GeneralParams::MINING_LOGS.load();result.push_back([saved](){GeneralParams::MINING_LOGS=saved;});}
    {auto saved=GeneralParams::MAXIMA_LOGS.load();result.push_back([saved](){GeneralParams::MAXIMA_LOGS=saved;});}
    {auto saved=GeneralParams::NETWORKING_LOGS.load();result.push_back([saved](){GeneralParams::NETWORKING_LOGS=saved;});}
    {auto saved=GeneralParams::IBDSYNC_LOGS.load();result.push_back([saved](){GeneralParams::IBDSYNC_LOGS=saved;});}
    {auto saved=GeneralParams::BLOCK_LOGS.load();result.push_back([saved](){GeneralParams::BLOCK_LOGS=saved;});}
    {auto saved=GeneralParams::ARCHIVESYNC_LIMIT_BANDWIDTH;result.push_back([saved](){GeneralParams::ARCHIVESYNC_LIMIT_BANDWIDTH=saved;});}
    {auto saved=GeneralParams::RPC_CRLF;result.push_back([saved](){GeneralParams::RPC_CRLF=saved;});}
    {auto saved=GeneralParams::DEFAULT_MINIDAPPS;result.push_back([saved](){GeneralParams::DEFAULT_MINIDAPPS=saved;});}
    {auto saved=GeneralParams::RPC_ENABLED;result.push_back([saved](){GeneralParams::RPC_ENABLED=saved;});}
    {auto saved=GeneralParams::RPC_AUTHENTICATE;result.push_back([saved](){GeneralParams::RPC_AUTHENTICATE=saved;});}
    {auto saved=GeneralParams::RPC_AUTHSTYLE;result.push_back([saved](){GeneralParams::RPC_AUTHSTYLE=saved;});}
    {auto saved=GeneralParams::RPC_PASSWORD;result.push_back([saved](){GeneralParams::RPC_PASSWORD=saved;});}
    {auto saved=GeneralParams::SEED_PHRASE;result.push_back([saved](){GeneralParams::SEED_PHRASE=saved;});}
    {auto saved=GeneralParams::ANYSEED_PHRASE;result.push_back([saved](){GeneralParams::ANYSEED_PHRASE=saved;});}
    {auto saved=GeneralParams::PEERSCHECKER_lOG.load();result.push_back([saved](){GeneralParams::PEERSCHECKER_lOG=saved;});}
    {auto saved=GeneralParams::IS_MEGAMMR;result.push_back([saved](){GeneralParams::IS_MEGAMMR=saved;});}
    {auto saved=GeneralParams::NOTIFY_ALL_TXPOW;result.push_back([saved](){GeneralParams::NOTIFY_ALL_TXPOW=saved;});}
    {auto saved=GeneralParams::RESCUE_MEGAMMR_NODE;result.push_back([saved](){GeneralParams::RESCUE_MEGAMMR_NODE=saved;});}
    {auto saved=GeneralParams::MDS_NOSSL;result.push_back([saved](){GeneralParams::MDS_NOSSL=saved;});}
    {auto saved=GeneralParams::MYSQL_DB_DETAILS;result.push_back([saved](){GeneralParams::MYSQL_DB_DETAILS=saved;});}
    {auto saved=GeneralParams::MYSQL_DB_DELAY;result.push_back([saved](){GeneralParams::MYSQL_DB_DELAY=saved;});}
    {auto saved=GeneralParams::MYSQL_DB_COINS;result.push_back([saved](){GeneralParams::MYSQL_DB_COINS=saved;});}
    {auto saved=GeneralParams::MYSQL_STORE_ALLTXPOW;result.push_back([saved](){GeneralParams::MYSQL_STORE_ALLTXPOW=saved;});}
    {auto saved=GeneralParams::PUBLICMDS_ENABLE;result.push_back([saved](){GeneralParams::PUBLICMDS_ENABLE=saved;});}
    {auto saved=GeneralParams::PUBLICMDS_SESSION_UID;result.push_back([saved](){GeneralParams::PUBLICMDS_SESSION_UID=saved;});}
    {auto saved=GeneralParams::MEGAMMR_MEGAPRUNE;result.push_back([saved](){GeneralParams::MEGAMMR_MEGAPRUNE=saved;});}
    {auto saved=GeneralParams::MEGAMMR_MEGAPRUNE_STATE;result.push_back([saved](){GeneralParams::MEGAMMR_MEGAPRUNE_STATE=saved;});}
    {auto saved=GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS;result.push_back([saved](){GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS=saved;});}
    {auto saved=GeneralParams::SHOW_NETWORK_CALLS;result.push_back([saved](){GeneralParams::SHOW_NETWORK_CALLS=saved;});}
    {auto saved=GeneralParams::SHOW_NETWORK_POLLS;result.push_back([saved](){GeneralParams::SHOW_NETWORK_POLLS=saved;});}
    {auto saved=GeneralParams::INIT_HASHTEST_AMOUNT;result.push_back([saved](){GeneralParams::INIT_HASHTEST_AMOUNT=saved;});}
    {auto saved=GeneralParams::GENERATE_SSL;result.push_back([saved](){GeneralParams::GENERATE_SSL=saved;});}
    {auto saved=GeneralParams::LOAD_JNI;result.push_back([saved](){GeneralParams::LOAD_JNI=saved;});}
    {auto saved=GeneralParams::DB_IGNORE_LOCKS;result.push_back([saved](){GeneralParams::DB_IGNORE_LOCKS=saved;});}
    {auto saved=GeneralParams::ALLOW_GEN_MESSAGE;result.push_back([saved](){GeneralParams::ALLOW_GEN_MESSAGE=saved;});}
    {auto saved=GeneralParams::USE_SQL_COINDB;result.push_back([saved](){GeneralParams::USE_SQL_COINDB=saved;});}
    {auto saved=GeneralParams::LIMIT_IBD_30MINS;result.push_back([saved](){GeneralParams::LIMIT_IBD_30MINS=saved;});}
    {auto saved=GlobalParams::MINIMA_BASE_VERSION;result.push_back([saved](){GlobalParams::MINIMA_BASE_VERSION=saved;});}
    {auto saved=GlobalParams::MINIMA_BUILD_NUMBER;result.push_back([saved](){GlobalParams::MINIMA_BUILD_NUMBER=saved;});}
    {auto saved=GlobalParams::MINIMA_VERSION;result.push_back([saved](){GlobalParams::MINIMA_VERSION=saved;});}
    {auto saved=GlobalParams::MINIMA_MICRO_BUILD;result.push_back([saved](){GlobalParams::MINIMA_MICRO_BUILD=saved;});}
    {auto saved=GlobalParams::MINIMA_BLOCK_SPEED;result.push_back([saved](){GlobalParams::MINIMA_BLOCK_SPEED=saved;});}
    {auto saved=GlobalParams::MINIMA_BLOCKS_SPEED_CALC;result.push_back([saved](){GlobalParams::MINIMA_BLOCKS_SPEED_CALC=saved;});}
    {auto saved=GlobalParams::MINIMA_CONFIRM_DEPTH;result.push_back([saved](){GlobalParams::MINIMA_CONFIRM_DEPTH=saved;});}
    {auto saved=GlobalParams::MINIMA_CASCADE_FREQUENCY;result.push_back([saved](){GlobalParams::MINIMA_CASCADE_FREQUENCY=saved;});}
    {auto saved=GlobalParams::MINIMA_CASCADE_START_DEPTH;result.push_back([saved](){GlobalParams::MINIMA_CASCADE_START_DEPTH=saved;});}
    {auto saved=GlobalParams::MINIMA_CASCADE_LEVEL_NODES;result.push_back([saved](){GlobalParams::MINIMA_CASCADE_LEVEL_NODES=saved;});}
    {auto saved=GlobalParams::MINIMA_CASCADE_LEVELS;result.push_back([saved](){GlobalParams::MINIMA_CASCADE_LEVELS=saved;});}
    {auto saved=GlobalParams::MINIMA_MMR_PROOF_HISTORY;result.push_back([saved](){GlobalParams::MINIMA_MMR_PROOF_HISTORY=saved;});}
    {auto saved=GlobalParams::MEDIAN_BLOCK_CALC;result.push_back([saved](){GlobalParams::MEDIAN_BLOCK_CALC=saved;});}
    {auto saved=TestParams::values().MINIMA_VERSION;result.push_back([saved](){TestParams::values().MINIMA_VERSION=saved;});}
    {auto saved=TestParams::values().MINIMA_BLOCK_SPEED;result.push_back([saved](){TestParams::values().MINIMA_BLOCK_SPEED=saved;});}
    {auto saved=TestParams::values().MINIMA_BLOCKS_SPEED_CALC;result.push_back([saved](){TestParams::values().MINIMA_BLOCKS_SPEED_CALC=saved;});}
    {auto saved=TestParams::values().MINIMA_CONFIRM_DEPTH;result.push_back([saved](){TestParams::values().MINIMA_CONFIRM_DEPTH=saved;});}
    {auto saved=TestParams::values().MINIMA_CASCADE_FREQUENCY;result.push_back([saved](){TestParams::values().MINIMA_CASCADE_FREQUENCY=saved;});}
    {auto saved=TestParams::values().MINIMA_CASCADE_START_DEPTH;result.push_back([saved](){TestParams::values().MINIMA_CASCADE_START_DEPTH=saved;});}
    {auto saved=TestParams::values().MINIMA_CASCADE_LEVEL_NODES;result.push_back([saved](){TestParams::values().MINIMA_CASCADE_LEVEL_NODES=saved;});}
    {auto saved=TestParams::values().MINIMA_CASCADE_LEVELS;result.push_back([saved](){TestParams::values().MINIMA_CASCADE_LEVELS=saved;});}
    {auto saved=TestParams::values().MINIMA_MMR_PROOF_HISTORY;result.push_back([saved](){TestParams::values().MINIMA_MMR_PROOF_HISTORY=saved;});}
    {auto saved=TestParams::values().MEDIAN_BLOCK_CALC;result.push_back([saved](){TestParams::values().MEDIAN_BLOCK_CALC=saved;});}
    {auto saved=p2p::P2PParams::LOG_LEVEL;result.push_back([saved](){p2p::P2PParams::LOG_LEVEL=saved;});}
    return result;
}
