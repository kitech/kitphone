// intermessage.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-09-05 21:20:58 +0800
// Version: $Id: intermessage.cpp 996 2011-09-16 15:04:24Z drswinghead $
// 

#ifdef _WIN32
#include <time.h>
#endif

#include "simplelog.h"
#include "intermessage.h"

int InterMessage::mseq;

// void test_im_pack ()
// {
//     // 
//     CmdMakeCall cmd1;

//     cmd1.callee_phone = "dsifdsfdsf";
//     cmd1.caller_name = "nnnnnnn中文如何name";
//     cmd1.caller_acc_id = 56789;

//     std::string cmd1str = InterMessage().pack_message(cmd1);
//     qLogx()<<"aaaa:"<<  cmd1str.c_str();
 
//     CmdMakeCall cmd2;
//     InterMessage().unpack_message(cmd2, cmd1str);

//     qLogx()<<"bbb:"<<cmd2.caller_acc_id<<cmd2.caller_name.c_str()<<cmd2.callee_phone.c_str();

//     exit(0);
// }

void test_im_jpack()
{
    CmdMakeCall cmd1;

    cmd1.callee_phone = "uuuuuuuuuuuudsifdsfdsf";
    cmd1.caller_name = "nnnnnnn中文如何nameaaaaaaaaannnnnnnnn";
    cmd1.acc_id = 56789789;

    qLogx()<<"bbb:"<<cmd1.acc_id<<cmd1.caller_name.c_str()<<cmd1.callee_phone.c_str();

    std::string jstr1 = InterMessage().jpack_message(cmd1);
    qLogx()<<jstr1.c_str();

    CmdMakeCall cmd2;
    InterMessage().junpack_message(cmd2, jstr1);

    qLogx()<<"bbb:"<<cmd2.acc_id<<cmd2.caller_name.c_str()<<cmd2.callee_phone.c_str();

    exit(0);
}

// call obj
std::string CmdMakeCall::to_json()
{
    std::string jstr;

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::FastWriter jwriter;

    root["cmd_id"] = cmd_id;
    root["cmd_seq"] = cmd_seq;
    root["callee_phone"] = callee_phone;
    root["caller_name"] = caller_name;
    root["sip_server"] = sip_server;
    root["caller_ipaddr"] = caller_ipaddr;
    root["acc_id"] = acc_id;
    root["call_id"] = call_id;
    root["switcher"] = switcher;

    /////// options
    root["tp_type"] = tp_type;
    root["use_tls"] = use_tls;
    root["use_stun"] = use_stun;
    root["use_turn"] = use_turn;
    root["use_ice"] = use_ice;
    root["stun_server"] = stun_server;
    root["use_codec"] = use_codec;

    jstr = jwriter.write(root);

    // 中文会出问题啊。

    return jstr;
}

// static 
CmdMakeCall &CmdMakeCall::from_json(CmdMakeCall &cmd, std::string jstr)
{
    Json::Features::all();
    Json::Value root;
    Json::Reader jreader;

    if (jreader.parse(jstr, root)) {
        cmd.cmd_seq = root["cmd_seq"].asInt();
        cmd.callee_phone = root["callee_phone"].asString();
        cmd.caller_name = root["caller_name"].asString();
        cmd.sip_server = root["sip_server"].asString();
        cmd.caller_ipaddr = root["caller_ipaddr"].asString();
        cmd.acc_id = root["acc_id"].asInt();
        cmd.call_id = root["call_id"].asInt();
        cmd.switcher = root["switcher"].asString();

        /////// options
        cmd.tp_type = root["tp_type"].asInt();
        cmd.use_tls = root["use_tls"].asBool();
        cmd.use_stun = root["use_stun"].asBool();
        cmd.use_turn = root["use_turn"].asBool();
        cmd.use_ice = root["use_ice"].asInt();
        cmd.stun_server = root["stun_server"].asString();
        cmd.use_codec = root["use_codec"].asString();
    }

    return cmd;
}

///////// hangup obj
std::string CmdHangupCall::to_json()
{
    std::string jstr;

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::FastWriter jwriter;

    root["cmd_id"] = cmd_id;
    root["cmd_seq"] = cmd_seq;
    root["call_id"] = call_id;
    root["reason_no"] = reason_no;
    root["reason_text"] = reason_text;

    jstr = jwriter.write(root);

    // 中文会出问题啊。

    return jstr;

}

// static 
CmdHangupCall &CmdHangupCall::from_json(CmdHangupCall &cmd, std::string jstr)
{
    Json::Features::all();
    Json::Value root;
    Json::Reader jreader;

    if (jreader.parse(jstr, root)) {
        cmd.cmd_seq = root["cmd_seq"].asInt();
        cmd.call_id = root["call_id"].asInt();
        cmd.reason_no = root["reason_no"].asInt();
        cmd.reason_text = root["reason_text"].asString();
    }

    return cmd;

}


// register obj
std::string CmdRegister::to_json()
{
    std::string jstr;

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::FastWriter fjwriter;

    root["cmd_id"] = cmd_id;
    root["cmd_seq"] = cmd_seq;
    root["display_name"] = display_name;
    root["user_name"] = user_name;
    root["password"] = password;
    root["sip_server"] = sip_server;
    root["acc_id"] = acc_id;
    root["unregister"] = unregister;

    jstr = fjwriter.write(root);

    return jstr;
}

// static 
CmdRegister &CmdRegister::from_json(CmdRegister &cmd, std::string jstr)
{

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::Reader jreader;

    if (jreader.parse(jstr, root)) {
        cmd.cmd_seq = root["cmd_seq"].asInt();
        cmd.display_name = root["display_name"].asString();
        cmd.user_name = root["user_name"].asString();
        cmd.password = root["password"].asString();
        cmd.sip_server = root["sip_server"].asString();
        cmd.acc_id = root["acc_id"].asInt();
        cmd.unregister = root["unregister"].asBool();
    }

    return cmd;
}

std::string CmdRegState::to_json()
{
    std::string jstr;

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::FastWriter jwriter;

    root["cmd_id"] = cmd_id;
    root["cmd_seq"] = cmd_seq;
    root["status"] = status;
    root["status_text"] = status_text;
    root["acc_id"] = acc_id;
    root["acc_uri"] = acc_uri;
    root["is_default"] = is_default;

    jstr = jwriter.write(root);

    return jstr;
}

// static 
CmdRegState &CmdRegState::from_json(CmdRegState &cmd, std::string jstr)
{
    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::Reader jreader;

    if (jreader.parse(jstr, root)) {
        cmd.cmd_seq = root["cmd_seq"].asInt();
        cmd.status = root["status"].asInt();
        cmd.status_text = root["status_text"].asString();
        cmd.acc_id = root["acc_id"].asInt();
        cmd.acc_uri = root["acc_uri"].asString();
        cmd.is_default = root["is_default"].asBool();
    }

    return cmd;

}

// from call state
std::string CmdCallState::to_json()
{
    std::string jstr;

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::FastWriter jwriter;

    root["cmd_id"] = cmd_id;
    root["cmd_seq"] = cmd_seq;
    root["call_id"] = call_id;
    root["acc_id"] = acc_id;
    root["state"] = state;
    root["state_text"] = state_text;
    root["last_status"] = last_status;
    root["last_status_text"] = last_status_text;
    root["media_status"] = media_status;
    root["media_dir"] = media_dir;

    jstr = jwriter.write(root);

    return jstr;
}

// static 
CmdCallState &CmdCallState::from_json(CmdCallState &cmd, std::string jstr)
{
    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::Reader jreader;

    if (jreader.parse(jstr, root)) {
        cmd.cmd_seq = root["cmd_seq"].asInt();
        cmd.call_id = root["call_id"].asInt();
        cmd.acc_id = root["acc_id"].asInt();
        cmd.state = root["state"].asInt();
        cmd.state_text = root["state_text"].asString();
        cmd.last_status = root["last_status"].asInt();
        cmd.last_status_text = root["last_status_text"].asString();
        cmd.media_status = root["media_status"].asInt();
        cmd.media_dir = root["media_dir"].asInt();
    }

    return cmd;

}

/////// codec list
std::string CmdCodecList::to_json()
{
    std::string jstr;

    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::FastWriter jwriter;

    root["cmd_id"] = cmd_id;
    root["cmd_seq"] = cmd_seq;

    for (int i = 0; i < codecs.size(); ++i) {
        root["codecs"][i] = codecs.at(i);
    }

    jstr = jwriter.write(root);

    return jstr;
}

CmdCodecList &CmdCodecList::from_json(CmdCodecList &cmd, std::string jstr)
{
    Json::Features::all();
    Json::Value root;
    // Json::StyledWriter jwriter;
    Json::Reader jreader;

    Json::Value cvs;

    if (jreader.parse(jstr, root)) {
        cmd.cmd_seq = root["cmd_seq"].asInt();

        cvs = root["codecs"];
        for (int i = 0; i < cvs.size(); ++i) {
            cmd.codecs.push_back(cvs[i].asString());
        }
    }

    return cmd;
}

///////////////////
int PackageSplitter::pkg_seq = 12345678;
PackageSplitter::PackageSplitter(const std::string &pstr)
{
    this->pstr = pstr;
    this->tlen = pstr.length();
    this->cseq = -1;
    PackageSplitter::pkg_seq ++;
}

PackageSplitter::~PackageSplitter()
{
}

bool PackageSplitter::hasNextFragment()
{
    this->cseq ++;
    
    if ((this->cseq) * frag_len <= this->tlen) {
        return true;
    } 
    return false;
}

std::string PackageSplitter::nextFragment()
{
    std::string frag;
    char tbuf[22] = {0};

    frag = this->pstr.substr(this->cseq*frag_len, this->frag_len);
    snprintf(tbuf, sizeof(tbuf)-1, "%.8d%.4d%.4d%.4d",
             this->pkg_seq, this->tlen, this->cseq, (int)frag.length());
    
    frag = std::string(tbuf) + frag;

    return frag;
}


bool PackageCombiner::save_fragment(const std::string &frag)
{
    int msg_id;
    int tlen;
    int frag_id;
    int frag_len;
    std::string frag_str;
    
    // 00000000 0000 0000 0000 xxxxxxxxxxxxxxxx
    msg_id = strtol(frag.substr(0, 8).c_str(), NULL, 10);
    tlen = strtol(frag.substr(8, 4).c_str(), NULL, 10);
    frag_id = strtol(frag.substr(12, 4).c_str(), NULL, 10);
    frag_len = strtol(frag.substr(16, 4).c_str(), NULL, 10);

    frag_str = frag.substr(20);

    assert(frag_str.length() == frag_len);
    
    if (this->fragstats.find(msg_id) == this->fragstats.end()) {
        this->fragstats[msg_id] = PackageState();

        this->fragstats[msg_id].pkg_seq = msg_id;
        this->fragstats[msg_id].tlen = tlen;
        this->fragstats[msg_id].ctime = time(NULL);
    }

    this->fragstats[msg_id].glen += frag_len;
    this->fragstats[msg_id].arrfrag[frag_id] = frag_str;
    this->fragstats[msg_id].mtime = time(NULL);

    return true;
}

bool PackageCombiner::is_package_finish(const std::string &frag)
{
    int msg_id;

    msg_id = strtol(frag.substr(0, 8).c_str(), NULL, 10);

    if (this->fragstats[msg_id].tlen == this->fragstats[msg_id].glen) {
        return true;
    }

    return false;
}


std::string PackageCombiner::get_full_package(const std::string &any_frag)
{
    std::string mstr;

    int msg_id = -1;

    msg_id = strtol(any_frag.substr(0, 8).c_str(), NULL, 10);

    assert(this->fragstats[msg_id].tlen == this->fragstats[msg_id].glen);

    if (this->fragstats.find(msg_id) == this->fragstats.end()) {
        assert(1==2);
    }

    for (int i = 0; i < (this->fragstats[msg_id].tlen/frag_len)+1; ++i) {
        mstr += this->fragstats[msg_id].arrfrag[i];
    }

    int iret = this->fragstats.erase(msg_id);
    if (iret != 1) {
        assert(1==2 && iret == 1);
    }

    return mstr;
}
