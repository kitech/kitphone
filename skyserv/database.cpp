// database.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-07 21:26:24 +0800
// Version: $Id: database.cpp 171 2010-10-13 07:34:24Z drswinghead $
// 

#include <stdlib.h>
#include <assert.h>

#include <QtCore>

#include "configs.h"
#include "database.h"

// #define DB_HOST "127.0.0.1"
// #define DB_NAME "karia2_resource"
// #define DB_USER "gzleo"
// #define DB_PASSWD ""
// #define DB_PORT 5432

static void noticeReceiver(void *arg, const PGresult *res)
{
    qDebug()<<__FILE__<<__LINE__<<arg<<res;
}

Database::Database(QObject *parent)
    : QObject(parent)
{
    this->conn = NULL;
}

Database::~Database()
{
    if (this->conn != NULL) {
        PQfinish(this->conn);
        this->conn = NULL;
    }
}

bool Database::connectdb()
{
    QString db_host;
    QString db_name;
    QString db_user;
    QString db_passwd;
    QString db_port;

    if (PQisthreadsafe() != 1) {
        qDebug()<<"Warning: the libpq is not threadsafe.";
    }

    char connInfo[200] = {0};
    QHash<QString, QString> host_info;
    host_info = Configs().getDatabaseInfo();

    db_host = host_info.value("host");
    db_name = host_info.value("db_name");
    db_user = host_info.value("user");
    db_passwd = host_info.value("passwd");
    db_port = host_info.value("port");
    // qDebug()<<db_host<<db_name<<db_user<<db_passwd<<db_port;

    snprintf(connInfo, sizeof(connInfo), "hostaddr=%s dbname=%s user=%s password=%s port=%d",
             db_host.toAscii().data(), db_name.toAscii().data(), db_user.toAscii().data(),
             db_passwd.toAscii().data(), db_port.toInt());
    //             DB_HOST, DB_NAME, DB_USER, DB_PASSWD, DB_PORT);

    this->conn = PQconnectdb(connInfo);
    int st = PQstatus(this->conn);
    if (st == CONNECTION_OK) {
        qDebug()<<"Connect pg ok.";
        // PQsetNoticeReceiver(this->conn, noticeReceiver, 0);
        return true;
    } else {
        switch(st) {
        case CONNECTION_STARTED:
            break;
        case CONNECTION_MADE:
            break;
        default:
            qDebug()<<"Unknown db connect status:"<<st;
            break;
        }
        PQfinish(this->conn);
        this->conn = NULL;
    }
    return false;
}

bool Database::isConnected()
{
    return (this->conn != NULL && PQstatus(this->conn) == CONNECTION_OK);
    return false;
}

bool Database::disconnectdb()
{
    if (this->conn != NULL) {
        PQfinish(this->conn);
        this->conn = NULL;
    }

    return false;
}
bool Database::reconnectdb()
{
    if (this->conn != NULL) {
        PQreset(this->conn);
        return true;
    }
    
    return false;
}


int Database::acquireGateway(QString caller_name, QString callee_phone, QString &gateway,
                             unsigned short &port, QString &ipaddr)
{
    gateway = QString::null;
    QString esc_caller_name, esc_callee_phone;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, caller_name.toAscii().data(), caller_name.length(), &errval);
    if (errval == 0) {
        esc_caller_name = QString(str);
    } else {
        qDebug()<<"Warning: escape caller_name error:"<<caller_name;
    }

    errval = -1;
    memset(str, 0, sizeof(str));
    slen = PQescapeStringConn(this->conn, str, callee_phone.toAscii().data(), callee_phone.length(), &errval);
    if (errval == 0) {
        esc_callee_phone = QString(str);
    } else {
        qDebug()<<"Warning: escape callee_phone error:"<<callee_phone<<errval;
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, caller_name.toAscii().data(), caller_name.length());
    // if (str != NULL) {
    //     esc_caller_name = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape caller_name error:"<<caller_name;
    // }
    // str = PQescapeLiteral(this->conn, callee_phone.toAscii().data(), callee_phone.length());
    // if (str != NULL) {
    //     esc_callee_phone = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape callee_phone error:"<<callee_phone;
    // }

    int rand_key = time(NULL) + qrand();
    int upcnt = 0;
    QString same_net_clause = " 1=1 ";

    if (!ipaddr.isEmpty()) {
        same_net_clause = QString(" ws_ipaddr LIKE '%1%'")
            .arg(ipaddr.left(ipaddr.lastIndexOf(".") + 1));
        Q_ASSERT(ipaddr.lastIndexOf(".") != -1);
        ipaddr = QString(); // 不再需要，防止再返回这个值。
    }

    QString sql = QString("UPDATE skype_gateways SET in_use = 1, lock_time=NOW(),caller_name='%1',callee_phone='%2', rand_key=%3 WHERE in_use = 0 AND skype_id = (SELECT skype_id FROM skype_gateways WHERE in_use=0 AND %4 ORDER BY RANDOM() LIMIT 1 FOR UPDATE)")
        .arg(esc_caller_name).arg(esc_callee_phone).arg(rand_key).arg(same_net_clause);
    QString sql2 = QString("SELECT skype_id,ws_port,ws_ipaddr FROM skype_gateways WHERE in_use=1 AND caller_name='%1' AND rand_key=%2").arg(esc_caller_name).arg(rand_key);

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK) {
        upcnt = atoi(PQcmdTuples(pres));
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    PQclear(pres);
    if (upcnt == 1) {
        pres = PQexec(this->conn, sql2.toAscii().data());
        if (PQresultStatus(pres) != PGRES_TUPLES_OK) {
            assert(PQresultStatus(pres) == PGRES_TUPLES_OK);
        } else {
            gateway = QString(PQgetvalue(pres, 0, 0));
            port = QString(PQgetvalue(pres, 0, 1)).toUShort();
            ipaddr = QString(PQgetvalue(pres, 0, 2));
        }
        PQclear(pres);
    }
    if (gateway == QString::null || gateway.length() == 0) {
        return 404;
    }
    return 200;
}

int Database::releaseGateway(QString caller_name, QString gateway)
{
    // 200 ok, 404 not found, 500 internal error
    QString esc_caller_name, esc_gateway;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }
    
    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, gateway.toAscii().data(), gateway.length(), &errval);
    if (errval == 0) {
        esc_gateway = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<gateway<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }

    int upcnt = 0;
    QString sql = QString("UPDATE skype_gateways SET in_use = 0, lock_time=NOW() WHERE skype_id='%1' AND in_use=1").arg(esc_gateway);
    qDebug()<<sql;

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK) {
        upcnt = atoi(PQcmdTuples(pres));
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    PQclear(pres);
    
    if (upcnt == 0) {
        return 404;
    } else if (upcnt == 1) {
        return 200;
    } else {
        return 500;
    }
    return 500;
}

QString Database::getForwardPhone(QString caller_name, QString gateway)
{
    QString callee_phone = QString::null;
    QString update_time;
    QString esc_caller_name, esc_gateway;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, gateway.toAscii().data(), gateway.length(), &errval);
    if (errval == 0) {
        esc_gateway = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<gateway<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }

    int upcnt = 0;
    QString sql = QString("SELECT callee_phone, lock_time FROM skype_gateways WHERE skype_id='%1' AND in_use=1").arg(esc_gateway);

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres);
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<sql;
    if (upcnt == 1) {
        callee_phone = PQgetvalue(pres, 0, 0);
        update_time = QString(PQgetvalue(pres, 0, 1));
        // PQclear(pres);
        qDebug()<<sql<<callee_phone<<update_time;
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<gateway;
    }
    PQclear(pres);
    
    return callee_phone;
}

int Database::setCallPair(QString caller_name, QString callee_phone, QString remote_ipaddr, QStringList &routers)
{
    QString esc_caller_name, esc_callee_phone;
    size_t slen = 0;
    int errval = -1;
    char str[512] = {0};
    QString sql;
    int upcnt = 0;

    this->reconnectdb();

    if (callee_phone.left(1) == "*") {
        callee_phone = QString("99008668056") + callee_phone.right(callee_phone.length()-1);
    } else if (callee_phone.startsWith("9900866")) {
    } else {
        // wrong phone_number;
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, caller_name.toAscii().data(), caller_name.length(), &errval);
    if (errval == 0) {
        esc_caller_name = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<caller_name<<errval; 
    }

    slen = PQescapeStringConn(this->conn, str, callee_phone.toAscii().data(), callee_phone.length(), &errval);
    if (errval == 0) {
        esc_callee_phone = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<callee_phone<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }
    
    sql = QString("SELECT merge_replace_skype_call_pairs('%1', '%2', '%3')")
      .arg(esc_caller_name).arg(esc_callee_phone).arg(remote_ipaddr);
    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres);
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
        PQclear(pres);
        return 503;
    }
    PQclear(pres);

    sql = QString("SELECT skype_id,in_service FROM skype_routers WHERE in_service=1 ORDER BY random() LIMIT 3");
    pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres);
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<sql;
    if (upcnt >= 1) {
        QString router;
        for (int i = 0 ; i < upcnt; i++) {
            router = QString(PQgetvalue(pres, i, 0));
            routers << router;
            qDebug()<<sql<<i<<router;
        }
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<caller_name;
        PQclear(pres);
        return 504;
    }
    PQclear(pres);
    
    return 200;
}

QString Database::getCallPeer(QString caller_name, QString &caller_ipaddr)
{
    QString callee_phone = QString::null;
    QString update_time;
    QString esc_caller_name, esc_gateway;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, caller_name.toAscii().data(), caller_name.length(), &errval);
    if (errval == 0) {
        esc_caller_name = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<caller_name<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }

    int upcnt = 0;
    QString sql = QString("SELECT callee_phone, caller_ipaddr, lock_time FROM skype_callpairs WHERE skype_id='%1' AND EXTRACT(EPOCH from NOW()-lock_time) < 60.0 ORDER BY lock_time DESC LIMIT 1").arg(esc_caller_name);

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres);
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<sql;
    if (upcnt == 1) {
        callee_phone = PQgetvalue(pres, 0, 0);
        caller_ipaddr = PQgetvalue(pres, 0, 1);
        update_time = QString(PQgetvalue(pres, 0, 2));
        // PQclear(pres);
        qDebug()<<sql<<callee_phone<<update_time;
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<caller_name;
    }
    PQclear(pres);
    
    return callee_phone;
}

/////////////
bool Database::removeCallPair(QString caller_name)
{
    this->reconnectdb();

    QString callee_phone = QString::null;
    QString update_time;
    QString esc_caller_name, esc_gateway;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;
    bool real_remove = false; // delete or update flag

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, caller_name.toAscii().data(), caller_name.length(), &errval);
    if (errval == 0) {
        esc_caller_name = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<caller_name<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }
    
    // here, the delta time should be large, because user answered time is included
    int upcnt = 0;
    QString sql;
    if (real_remove == true) {
        sql = QString("DELETE FROM skype_callpairs WHERE skype_id='%1' AND EXTRACT(EPOCH from NOW()-lock_time) < 86400.0").arg(esc_caller_name);
    } else {
        sql = QString("UPDATE skype_callpairs SET delete_flag=1 WHERE skype_id='%1' AND EXTRACT(EPOCH from NOW()-lock_time) < 86400.0").arg(esc_caller_name);
        // ORDER BY lock_time DESC LIMIT 1
    }

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres);
        upcnt = atoi(PQcmdTuples(pres));
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<upcnt<<sql;
    if (upcnt == 1) {
        PQclear(pres);
        return true;
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<caller_name;
    }
    PQclear(pres);
    
    return false;
}

bool Database::setForwardPort(QString gateway, unsigned short port, QString ipaddr)
{
    this->reconnectdb();

    QString esc_gateway;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, gateway.toAscii().data(), gateway.length(), &errval);
    if (errval == 0) {
        esc_gateway = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<gateway<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }
    
    // here, the delta time should be large, because user answered time is included
    int upcnt = 0;
    QString sql;
    sql = QString("UPDATE skype_gateways SET ws_port=%1, ws_ipaddr='%2', lock_time=NOW() WHERE skype_id='%3'")
        .arg(port).arg(ipaddr).arg(esc_gateway);
        // ORDER BY lock_time DESC LIMIT 1

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres); 
        upcnt = atoi(PQcmdTuples(pres));
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<upcnt<<sql;
    if (upcnt == 1) {
        PQclear(pres);
        return true;
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<gateway;
    }
    PQclear(pres);
    
    return false;
}

unsigned short Database::getForwardPort(QString caller_name, QString &ipaddr)
{
    
    return 0;
}

bool Database::setLineState(QString gateway, int state)
{
    
    this->reconnectdb();

    QString esc_gateway;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, gateway.toAscii().data(), gateway.length(), &errval);
    if (errval == 0) {
        esc_gateway = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<gateway<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }
    
    // here, the delta time should be large, because user answered time is included
    int upcnt = 0;
    QString sql;
    sql = QString("UPDATE skype_gateways SET in_use=%1, lock_time=NOW() WHERE skype_id='%2'")
        .arg(state).arg(esc_gateway);
        // ORDER BY lock_time DESC LIMIT 1

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres); 
        upcnt = atoi(PQcmdTuples(pres));
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<upcnt<<sql;
    if (upcnt == 1) {
        PQclear(pres);
        return true;
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<gateway;
    }
    PQclear(pres);

    return true;
}

bool Database::setBatchLineState(QStringList & gateways, int state)
{
    
    this->reconnectdb();

    QString esc_gateway;
    QString gw_names;
    char str[512] = {0};
    size_t slen = 0;
    int errval = -1;

    if (PQstatus(this->conn) != CONNECTION_OK) {
        // connection lost, reset connection
        PQreset(this->conn);
        Q_ASSERT(PQstatus(this->conn) == CONNECTION_OK);
    }

    gw_names = QString("'") + gateways.join("','") + QString("'");

    // for postgresql 8.x
    slen = PQescapeStringConn(this->conn, str, gw_names.toAscii().data(), gw_names.length(), &errval);
    if (errval == 0) {
        esc_gateway = QString(str);
    } else {
        qDebug()<<"Warning: escape gateway error:"<<gateways<<errval; 
    }

    // for postgresql 9.x
    // str = PQescapeLiteral(this->conn, gateway.toAscii().data(), gateway.length());
    // if (str != NULL) {
    //     esc_gateway = QString(str);
    //     PQfreemem(str); str = NULL;
    // } else {
    //     qDebug()<<"Warning: escape gateway error:"<<gateway; 
    // }
    
    // here, the delta time should be large, because user answered time is included
    int upcnt = 0;
    QString sql;
    sql = QString("UPDATE skype_gateways SET in_use=%1, lock_time=NOW() WHERE skype_id in (%2)")
        .arg(state).arg(gw_names);
        // ORDER BY lock_time DESC LIMIT 1

    PGresult *pres = PQexec(this->conn, sql.toAscii().data());
    if (PQresultStatus(pres) == PGRES_COMMAND_OK
        || PQresultStatus(pres) == PGRES_TUPLES_OK) {
        upcnt = PQntuples(pres); 
        upcnt = atoi(PQcmdTuples(pres));
    } else {
        qDebug()<<"PQexec error: "<<PQresultStatus(pres)<<QString(PQerrorMessage(this->conn));
    }
    qDebug()<<upcnt<<sql;
    if (upcnt == 1) {
        PQclear(pres);
        return true;
    } else {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<"not found:"<<gateways;
    }
    PQclear(pres);

    return true;
}
