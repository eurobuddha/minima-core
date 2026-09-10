// Active consumer bodies translated from pinned ParamConfigurer.java.
        case 0: { // data
            auto path=javaUnixChildFile(*javaUnixFile(std::filesystem::path(utf8(value))),GlobalParams::MINIMA_BASE_VERSION);makeDirectories(path);GeneralParams::DATA_FOLDER=javaUnixAbsoluteFile(path);
            break;
        }
        case 1: { // dbpassword
            GeneralParams::IS_MAIN_DBPASSWORD_SET = true;
            GeneralParams::MAIN_DBPASSWORD 		 = utf8(value);
            break;
        }
        case 2: { // basefolder
            auto path=*javaUnixFile(std::filesystem::path(utf8(value)));makeDirectories(path);GeneralParams::BASE_FILE_FOLDER=javaUnixAbsoluteFile(path);
            break;
        }
        case 3: { // host
            GeneralParams::MINIMA_HOST = utf8(value);
            GeneralParams::IS_HOST_SET = true;
            break;
        }
        case 4: { // port
            GeneralParams::MINIMA_PORT = javaParseInt(utf8(value));
            break;
        }
        case 5: { // rpc
            MinimaLogger::log("-rpc is no longer in use. Your RPC port is: "+std::to_string(GeneralParams::RPC_PORT));
            break;
        }
        case 6: { // rpcenable
            if (value==u"true") {
                GeneralParams::RPC_ENABLED = true;
            }else {
                GeneralParams::RPC_ENABLED = false;
            }
            break;
        }
        case 7: { // rpcpassword
            GeneralParams::RPC_PASSWORD 		= utf8(value);
            GeneralParams::RPC_AUTHENTICATE 	= true;
            break;
        }
        case 8: { // rpccrlf
            if (value==u"true") {
                GeneralParams::RPC_CRLF = true;
            }else {
                GeneralParams::RPC_CRLF = false;
            }
            break;
        }
        case 9: { // shownetcalls
            if (value==u"true") {
                GeneralParams::SHOW_NETWORK_CALLS = true;
                GeneralParams::SHOW_NETWORK_POLLS = true;
            }else {
                GeneralParams::SHOW_NETWORK_CALLS = false;
            }
            break;
        }
        case 10: { // shownetcallsnopoll
            if (value==u"true") {
                GeneralParams::SHOW_NETWORK_CALLS = true;
                GeneralParams::SHOW_NETWORK_POLLS = false;
            }else {
                GeneralParams::SHOW_NETWORK_CALLS = false;
            }
            break;
        }
        case 11: { // allowallip
            if (value==u"true") {
                GeneralParams::ALLOW_ALL_IP = true;
            }else {
                GeneralParams::ALLOW_ALL_IP = false;
            }
            break;
        }
        case 12: { // archive
            if (value==u"true") {
                GeneralParams::ARCHIVE = true;
            }else {
                GeneralParams::ARCHIVE = false;
            }
            break;
        }
        case 13: { // mdsenable
            if (value==u"true") {
                GeneralParams::MDS_ENABLED = true;
            }else {
                GeneralParams::MDS_ENABLED = false;
            }
            break;
        }
        case 14: { // mdspassword
            GeneralParams::MDS_PASSWORD = utf8(trim(value));
            break;
        }
        case 15: { // mdsinit
            auto path=*javaUnixFile(std::filesystem::path(utf8(value)));makeDirectories(path);GeneralParams::MDS_INITFOLDER=javaUnixAbsoluteFile(path);
            break;
        }
        case 16: { // mdswrite
            GeneralParams::MDS_WRITE= utf8(value);
            break;
        }
        case 17: { // conf
            break;
        }
        case 18: { // daemon
            if (value==u"true") {
                mDaemon = true;
            }
            break;
        }
        case 19: { // isclient
            if (value==u"true") {
                GeneralParams::IS_ACCEPTING_IN_LINKS = false;
            }
            break;
        }
        case 20: { // desktop
            if (value==u"true") {
                GeneralParams::IS_DESKTOP 			= true;
                GeneralParams::IS_ACCEPTING_IN_LINKS = false;
            }
            break;
        }
        case 21: { // server
            if (value==u"true") {
                GeneralParams::IS_ACCEPTING_IN_LINKS = true;
            }
            break;
        }
        case 22: { // mobile
            if (value==u"true") {
                GeneralParams::IS_ACCEPTING_IN_LINKS = false;
                GeneralParams::IS_MOBILE 			= true;
            }
            break;
        }
        case 23: { // jnlp
            if (value==u"true") {
                GeneralParams::IS_JNLP = true;
            }
            break;
        }
        case 24: { // showparams
            if (value==u"true") {
                GeneralParams::SHOW_PARAMS = true;
            }
            break;
        }
        case 25: { // nop2p
            if (value==u"true") {
                GeneralParams::P2P_ENABLED = false;
            }
            break;
        }
        case 26: { // noshutdownhook
            mShutdownhook = false;
            break;
        }
        case 27: { // noconnect
            if (value==u"true") {
                GeneralParams::NOCONNECT = true;
            }
            break;
        }
        case 28: { // p2prootnode
            GeneralParams::P2P_ROOTNODE = utf8(value);
            break;
        }
        case 29: { // p2pnodes
            if(value.starts_with(u"http")) {
                try {GeneralParams::P2P_ADDNODES=RPCClient::sendGET(utf8(value));}
                catch(const wire::IOException& error){MinimaLogger::log("Error trying -p2pnodes URL:"+utf8(value)+" "+javaIOExceptionText(error));}
            }else GeneralParams::P2P_ADDNODES=utf8(value);
            break;
        }
        case 30: { // p2ploglevelinfo
            p2p::P2PParams::LOG_LEVEL = p2p::P2PLogLevel::INFO;
            break;
        }
        case 31: { // p2plogleveldebug
            p2p::P2PParams::LOG_LEVEL = p2p::P2PLogLevel::DEBUG;
            break;
        }
        case 32: { // p2p2
            if (value==u"true") {
                GeneralParams::P2P2_ENABLED = true;
                GeneralParams::P2P_ENABLED  = false;
            }
            break;
        }
        case 33: { // allowgenmessage
            GeneralParams::ALLOW_GEN_MESSAGE = true;
            break;
        }
        case 34: { // connect
            GeneralParams::P2P_ENABLED = false;
            GeneralParams::CONNECT_LIST = utf8(value);
            break;
        }
        case 35: { // clean
            if (value==u"true") {
                GeneralParams::CLEAN = true;
            }
            break;
        }
        case 36: { // nodefaultminidapps
            if (value==u"true") {
                GeneralParams::DEFAULT_MINIDAPPS = false;
            }
            break;
        }
        case 37: { // nosyncibd
            if (value==u"true") {
                GeneralParams::NO_SYNC_IBD = true;
            }
            break;
        }
        case 38: { // syncibdlogs
            if (value==u"true") {
                GeneralParams::IBDSYNC_LOGS = true;
            }
            break;
        }
        case 39: { // megammr
            if (value==u"true") {
                GeneralParams::IS_MEGAMMR = true;
            }
            break;
        }
        case 40: { // notifyalltxpow
            if (value==u"true") {
                GeneralParams::NOTIFY_ALL_TXPOW = true;
            }
            break;
        }
        case 41: { // slavenode
            GeneralParams::CONNECT_LIST 			= utf8(value);
            GeneralParams::P2P_ENABLED 			= false;
            GeneralParams::TXBLOCK_NODE 			= true;
            GeneralParams::NO_SYNC_IBD 			= true;
            GeneralParams::IS_ACCEPTING_IN_LINKS = false;
            break;
        }
        case 42: { // limitbandwidth
            if (value==u"true") {
                GeneralParams::ARCHIVESYNC_LIMIT_BANDWIDTH = true;
            }
            break;
        }
        case 43: { // sqlcoindb
            if (value==u"true") {
                GeneralParams::USE_SQL_COINDB = true;
            }
            break;
        }
        case 44: { // genesis
            if (value==u"true") {
                GeneralParams::CLEAN = true;
                GeneralParams::GENESIS = true;
            }
            break;
        }
        case 45: { // test
            if (value==u"true") {
                GeneralParams::TEST_PARAMS 		= true;
                TestParams::setTestParams();
            }
            break;
        }
        case 46: { // solo
            if (value==u"true") {
                GeneralParams::PRIVATE 		= true;
                GeneralParams::TEST_PARAMS 	= true;
                TestParams::setTestParams();
                GeneralParams::P2P_ENABLED 	= false;
            }
            break;
        }
        case 47: { // testchainlength
            TestParams::values().MINIMA_CASCADE_START_DEPTH 	= MiniNumber(utf8(trim(value)));
            if(GeneralParams::TEST_PARAMS) {
                TestParams::setTestParams();
            }
            break;
        }
        case 48: { // nosslmds
            if (value==u"true") {
                GeneralParams::MDS_NOSSL = true;
            }
            break;
        }
        case 49: { // mysqldb
            GeneralParams::MYSQL_DB_DETAILS = utf8(value);
            break;
        }
        case 50: { // mysqldbcoins
            if (value==u"true") {
                GeneralParams::MYSQL_DB_COINS = true;
            }
            break;
        }
        case 51: { // mysqldbdelay
            GeneralParams::MYSQL_DB_DELAY = javaParseInt(utf8(value));
            break;
        }
        case 52: { // mysqlalltxpow
            if (value==u"true") {
                GeneralParams::MYSQL_STORE_ALLTXPOW = true;
            }
            break;
        }
        case 53: { // txpowdbstore
            GeneralParams::NUMBER_DAYS_SQLTXPOWDB = javaParseLong(utf8(trim(value)));
            if(GeneralParams::NUMBER_DAYS_SQLTXPOWDB<3) {
                GeneralParams::NUMBER_DAYS_SQLTXPOWDB=3;
                MinimaLogger::log("Invalid txpowdbstore.. MUST be >= 3.. setting to 3");
            }
            break;
        }
        case 54: { // publicmds
            GeneralParams::PUBLICMDS_ENABLE = true;
            break;
        }
        case 55: { // publicmdsuid
            GeneralParams::PUBLICMDS_ENABLE 		= true;
            GeneralParams::PUBLICMDS_SESSION_UID = utf8(trim(value));
            break;
        }
        case 56: { // rescuenode
            GeneralParams::RESCUE_MEGAMMR_NODE 	= utf8(trim(value));
            break;
        }
        case 57: { // help
            mRuntime.println("Minima Help");
            for(const auto& key:keys()) {
                auto flag="-"+key.key;auto help=key.help;
                mRuntime.println(flag+std::string(flag.size()<20?20-flag.size():0,' ')+help+std::string(help.size()<15?15-help.size():0,' '));
            }
            mRuntime.exit(1);
            break;
        }
        case 58: { // hashtest
            GeneralParams::INIT_HASHTEST_AMOUNT 	= javaParseInt(utf8(trim(value)));
            break;
        }
        case 59: { // enablejni
            GeneralParams::LOAD_JNI = true;
            break;
        }
        case 60: { // nossl
            GeneralParams::GENERATE_SSL 	= false;
            break;
        }
        case 61: { // seed
            GeneralParams::SEED_PHRASE 	= utf8(value);
            break;
        }
        case 62: { // anyseed
            GeneralParams::ANYSEED_PHRASE = true;
            break;
        }
        case 63: { // dbignorelocks
            GeneralParams::DB_IGNORE_LOCKS  = true;
            break;
        }
        case 64: { // megaprune
            if (value==u"true") {
                GeneralParams::MEGAMMR_MEGAPRUNE = true;
            }
            break;
        }
        case 65: { // megaprunestate
            if (value==u"true") {
                GeneralParams::MEGAMMR_MEGAPRUNE 		= true;
                GeneralParams::MEGAMMR_MEGAPRUNE_STATE 	= true;
            }
            break;
        }
        case 66: { // megaprunetokens
            if (value==u"true") {
                GeneralParams::MEGAMMR_MEGAPRUNE 		= true;
                GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS  = true;
            }
            break;
        }
