// intermessage.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-05 21:20:11 +0800
// Version: $Id: intermessage.h 999 2011-09-17 14:25:17Z drswinghead $
// 

#ifndef _INTERMESSAGE_H_
#define _INTERMESSAGE_H_

// 这种方式无法兼容web方式发来的数据，还是不太行。得找从web上容易序列化的方法
// 还是得考虑一下json方法。
// 实现使用JSON的数据打包方法。

#include <string.h>
#include <fstream>
#include <sstream>

// include headers that implement a archive in simple text format
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>

#include "json/json.h"

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

enum {
    IPC_NO_MIN = 0,
    IPC_NO_MAKE_CALL = 10,
    IPC_NO_HANGUP_CALL = 12,
    IPC_NO_REGISTER = 14,
    IPC_NO_REG_STATE = 15,
    IPC_NO_CALL_STATE = 17,
    IPC_NO_CALL_MEDIA_STATE = 19,
    IPC_NO_CODEC_LIST = 21,
    IPC_NO_RESOLV_NAME = 23,
    IPC_NO_MAX
};

class CmdBase
{
public:
    CmdBase () {
        cmd_seq = -1;
        
        eno = -1;
    }

    int cmd_seq;

    // return value;
    int eno;
    std::string emsg;

    // utils, session
    std::string session_id;
};

class CmdMakeCall 
{
public:
    CmdMakeCall() {
        cmd_seq = -1;
        acc_id = -1;
        call_id = -1;

        /// options
        tp_type = -1;
        use_tls = false;
        use_stun = false;
        use_turn = false;
        use_ice = false;

        eno = -1;
    }

    static const int cmd_id = IPC_NO_MAKE_CALL;
    int cmd_seq;
    std::string callee_phone;
    std::string caller_name;
    std::string sip_server;
    std::string caller_ipaddr;
    int acc_id;
    int call_id;
    std::string switcher;

    // options
    int tp_type;
    std::vector<std::string> codec_priorities;
    bool use_tls;
    bool use_stun;
    bool use_turn;
    bool use_ice;
    std::string stun_server;
    std::string use_codec;

    // return value
    int eno;
    std::string emsg;

    // template<class Archive>
    // void serialize(Archive & ar, const unsigned int version) {
    //     ar & cmd_id;
    //     ar & callee_phone;
    //     ar & caller_name;
    //     ar & caller_acc_id;
    // }

    std::string to_json();
    static CmdMakeCall &from_json(CmdMakeCall &cmd, std::string jstr);
};

class CmdHangupCall
{
public:
    CmdHangupCall() {
        cmd_seq = -1;
        reason_no = -1;

        call_id = -1;

        eno = -1;
    }

    const static int cmd_id = IPC_NO_HANGUP_CALL;
    int cmd_seq;
    int reason_no;
    std::string reason_text;
    int call_id;

    // return value
    int eno;
    std::string emsg;

    std::string to_json();
    static CmdHangupCall &from_json(CmdHangupCall &cmd, std::string jstr);
    
};

class CmdRegister
{
public:
    CmdRegister() {
        cmd_seq = -1;

        acc_id = -1;
        unregister = false;

        eno = -1;
    }

    static const int cmd_id = IPC_NO_REGISTER;
    int cmd_seq;
    std::string display_name;
    std::string user_name;
    std::string password;
    std::string sip_server;
    int acc_id;
    bool unregister;
    
    // return value
    int eno;
    std::string emsg;

    std::string to_json();
    static CmdRegister &from_json(CmdRegister &cmd, std::string jstr);

    // template<class Archive>
    // void serialize(Archive & ar, const unsigned int version) {
    //     ar & cmd_id;
    // }
    
};

class CmdRegState
{
public:
    CmdRegState() {
        cmd_seq = -1;

        acc_id = -1;
        status = -1;
        is_default = false;
        eno = -1;
    }

    static const int cmd_id = IPC_NO_REG_STATE;
    int cmd_seq;
    int acc_id;
    int status;
    std::string status_text;
    std::string acc_uri;
    bool is_default;

    // return value
    int eno;
    std::string emsg;

    std::string to_json();
    static CmdRegState &from_json(CmdRegState &cmd, std::string jstr);

    // template<class Archive>
    // void serialize(Archive & ar, const unsigned int version) {
    //     ar & cmd_id;
    // }

};

class CmdCallState
{
public:
    CmdCallState() {
        cmd_seq = -1;
        acc_id = -1;
        call_id = -1;

        state = -1;
        last_status = -1;
        media_status = -1;
        media_dir = -1;

        eno = -1;
    }

    static const int cmd_id = IPC_NO_CALL_STATE;
    int cmd_seq;
    int acc_id;
    int call_id;

    std::string local_info;
    std::string local_contact;
    std::string remote_info;
    std::string remote_contact;

    int state;
    std::string state_text;
    int last_status;
    std::string last_status_text;
    int media_status;
    int media_dir;

    // return value
    int eno;
    std::string emsg;

    std::string to_json();
    static CmdCallState &from_json(CmdCallState &cmd, std::string jstr);
};

class CmdCallMediaState : public CmdCallState
{
public:
    CmdCallMediaState() : CmdCallState() {
    }
    static const int cmd_id = IPC_NO_CALL_MEDIA_STATE;
};

///// codec list
class CmdCodecList {
public:
    CmdCodecList () {
        cmd_seq =0;
        
        eno = 0;
    }
    static const int cmd_id = IPC_NO_CODEC_LIST;
    int cmd_seq;
    std::vector<std::string> codecs;
    
    // return value;
    int eno;
    std::string emsg;

    std::string to_json();
    static CmdCodecList &from_json(CmdCodecList &cmd, std::string jstr);
};


//// obj
class CmdResolvName {
public:
    CmdResolvName () {
        cmd_seq = 0;
        eno = 0;
    }

    static const int cmd_id = IPC_NO_RESOLV_NAME;
    int cmd_seq;
    std::string name;

    // return value;
    int eno;
    std::string emsg;

    std::string to_json();
    static CmdResolvName &from_json(CmdResolvName &cmd, std::string jstr);    
};

//////////////////////
class InterMessage
{
public:
    static int mseq;
    // void pack_message();
    // void unpack_message();
    /////////
    template<class Command>
    std::string jpack_message(Command &cmd)
    {
        std::string jmsg;
        std::string rmsg;
        int cseq = ++ InterMessage::mseq;
        int msg_len = 0;

        cmd.cmd_seq = cseq;
        jmsg = cmd.to_json();
        char proto_head[32] = {0};
        snprintf(proto_head, sizeof(proto_head)-1, "%.8d%.8ld", cmd.cmd_id, jmsg.length());

        rmsg += std::string(proto_head) + jmsg;

        msg_len = rmsg.length();
        msg_len = msg_len;

        return rmsg;
    }


    template<class Command>
    bool junpack_message(Command &cmd, std::string str)
    {
        bool bret = false;

        int cmd_id;
        int cmd_len;
        char proto_head[32] = {0};
        
        strncpy(proto_head, str.c_str(), 8);
        cmd_id = strtol(proto_head, NULL, 10);
        strncpy(proto_head+8, str.substr(8, 8).c_str(), 8);
        cmd_len = strtol(proto_head, NULL, 10);

        std::string cmd_raw = str.substr(16);

        Command::from_json(cmd, cmd_raw);

        assert(cmd.cmd_id == cmd_id);
        cmd_len = str.length();
        cmd_len = cmd_len;

        return bret;
    }

    int jpack_cmdid(const std::string &str) {
        int cmd_id = 0;
        char proto_head[32] = {0};

        strncpy(proto_head, str.c_str(), 8);
        cmd_id = strtol(proto_head, NULL, 10);

        return cmd_id;
    }

    //////////////
    // template<class Command>
    // std::string pack_message(Command &cmd) {
    //     std::string rmsg;
    //     int cseq = ++ InterMessage::mseq;
    //     int msg_len = 0;
    //     std::ostringstream oss;
    //     std::ostringstream ross;
    //     // std::ofstream ofs("cmd_serial.txt");
    //     boost::archive::text_oarchive oa(ross);

    //     cmd.cmd_seq = cseq;
    //     oa << cmd;
    //     rmsg = ross.str();

    //     char proto_head[10] = {0};
    //     snprintf(proto_head, sizeof(proto_head)-1, "%.4d%.4d", cmd.cmd_id, rmsg.length());
        
    //     oss << proto_head << rmsg;
    //     rmsg = oss.str();

    //     // return std::string("aaaaaaaaaaaa");
    //     return rmsg;
    // }

    // template<class Command>
    // bool unpack_message(Command &cmd, std::string str) {
    //     int cmd_id;
    //     int cmd_len;
    //     char proto_head[10] = {0};
        
    //     strncpy(proto_head, str.c_str(), 4);
    //     cmd_id = strtol(proto_head, NULL, 10);
    //     strncpy(proto_head+4, str.substr(4, 4).c_str(), 4);
    //     cmd_len = strtol(proto_head, NULL, 10);

    //     std::string cmd_raw = str.substr(8);
    //     std::istringstream iss(cmd_raw);
    //     boost::archive::text_iarchive ia(iss);

    //     ia >> cmd;

    //     assert(cmd.cmd_id == cmd_id);
        
    //     return true;
    // }
};

class PackageSplitter
{
public:
    explicit PackageSplitter(const std::string &pstr);
    virtual ~PackageSplitter();

    bool hasNextFragment();
    std::string nextFragment();

protected:
    static int pkg_seq;
    static const int frag_len = 80;
    int tlen;
    int cseq; // current seq;

    std::string pstr;
};

class PackageCombiner
{
public:

    bool save_fragment(const std::string &frag);
    bool is_package_finish(const std::string &frag);
    std::string get_full_package(const std::string &any_frag);

protected:
    class PackageState {
    public:
        PackageState() {
            pkg_seq = -1;
            tlen = glen = 0;
            ctime = mtime = 0;
        }
        int pkg_seq;
        int tlen;
        int glen;
        std::map<int,int> arrmap; // arrive map;
        std::map<int,std::string> arrfrag; // arrive fragment
        int ctime;
        int mtime;
    };
    static const int frag_len = 80;
    std::map<int,PackageState> fragstats;
};

#endif /* _INTERMESSAGE_H_ */
