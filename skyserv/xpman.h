// krtman.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-05 22:44:57 +0800
// Version: $Id: krtman.h 672 2011-01-07 01:20:27Z drswinghead $
// 

#ifndef _KRTMAN_H_
#define _KRTMAN_H_

#include <QtCore>
#include <QThread>
#include <QString>
#include <QHash>
#include <QProcess>

#include "../libng/qbihash.h"

#include "boost/signals2.hpp"
#include "boost/bind.hpp"
#include "boost/asio.hpp"

#include "nbtimer.h"

class Database;

class KitRuntimeManager : public QThread
{
    Q_OBJECT;
public:
    KitRuntimeManager(int count, QObject *parent = 0);
    ~KitRuntimeManager();

    bool parserArguments(int argc, char **argv);
    void handleArguments(int argc, char **argv, int client_id);

    // void run_old();
    void run();

public slots:
    void onOtherInstanceMessageRecived(const QString &msg);
    void onOtherInstanceMessageRecived(const QString &msg, int client_id);

    void onAboutToQuit();

    // 处理unix信号
public:
    void init_unix_signal_handlers();

    // http://doc.qt.nokia.com/4.7-snapshot/unix-signals.html
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
    static void intSignalHandler(int unused);
    static void quitSignalHandler(int unused);

    static int sighupFd[2];
    static int sigtermFd[2];
    static int sigintFd[2];
    static int sigquitFd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;
    QSocketNotifier *snInt;
    QSocketNotifier *snQuit;

public slots:
    // app
    void handleSigHup();
    void handleSigTerm();
    void handleSigInt();
    void handleSigQuit();

private:
    bool set_all_line_exit();

    int manual_start_xvfb(QProcess *proc, int seq);
    int manual_start_vnc(QProcess *proc, int seq);
    int manual_start_skype(QProcess *proc, int seq);
    int manual_start_skyserv(QProcess *proc, int seq);
    int manual_start_obox(QProcess *proc, int seq);
    int manual_start_awesome(QProcess *proc, int seq);
    int manual_start_fvwm(QProcess *proc, int seq);
    int manual_start_xterm(QProcess *proc, int seq);

    int manual_start(QProcess *proc, int seq);

    struct AccountState {
        QString name;
        QString passwd;
        bool is_free;
        int line_no;
        QDateTime offtime;
    };

    bool find_candidate_account(AccountState **account, int serv_mode);
    bool find_account_by_seq(AccountState **account, int seq);
    bool dump_accounts_info();

private slots:
    void on_error(QProcess::ProcessError error);
    void on_finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void on_readyReadStandardError ();
    void on_readyReadStandardOutput ();
    void on_started ();
    void on_stateChanged ( QProcess::ProcessState newState );

    void connect_process_slot(QProcess *proc);
    void disconnect_process_slot(QProcess *proc);

    void on_check_process_alive();
    void on_check_process_alive_b();
    void on_check_process_alive_c(const boost::system::error_code& e);
    void catch_crash_backtrace(QProcess *proc, int seq);

    void on_catch_crash_proc_finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void on_catch_crash_proc_readyReadStandardOutput();

private:
    // 运行于forward模式，管理forward服务器，
    // router模式，管理router服务器
    // both模式，两类服务器都需要管理。
    enum ServerMode {
        SM_MIN = 0,
        SM_FORWARDER = 1,
        SM_ROUTER,
        SM_BOTH,
        SM_MAX
    };
    int server_mode; 

    
    QString xvfb_exec_path;
    QString vnc_exec_path;
    QString skype_router_exec_path;
    QString skype_switcher_exec_path;
    QString skyserv_exec_path;
    QString obox_exec_path;
    QString awesome_exec_path;
    QString fvwm_exec_path;
    QString xterm_exec_path;

    KBiHash<QProcess*, int> xvfb_procs;
    KBiHash<QProcess*, int> vnc_procs;
    KBiHash<QProcess*, int> skype_procs;
    KBiHash<QProcess*, int> skyserv_procs;
    KBiHash<QProcess*, int> obox_procs;
    KBiHash<QProcess*, int> awesome_procs;
    KBiHash<QProcess*, int> fvwm_procs;
    KBiHash<QProcess*, int> xterm_procs;

    QString cfg_dir;
    QString pid_dir;
    QString log_dir;

    QVector<QPair<QString, QString> > routers;
    QVector<QPair<QString, QString> > switchers;
    QVector<QPair<QString, QString> > old_switchers;
    QVector<QPair<QString, QString> > new_switchers;

    QVector<AccountState> router_pool;
    QVector<AccountState> switcher_pool;

    class LineAttribute {
    public:
        LineAttribute() {
            xvfb_log_fp = NULL;
            vnc_log_fp = NULL;
            skype_log_fp = NULL;
            skyserv_log_fp = NULL;
            skype_last_crash_time = 0;
            skyserv_last_alive_time = 0;
            skyserv_last_crash_time = 0;
            set_stop = false;
        }
        
        int seq;
        int screen_no;
        unsigned short vnc_port;
        QString xvfb_log_file;
        QFile *xvfb_log_fp;
        QString vnc_log_file;
        QFile *vnc_log_fp;
        QString skype_log_file;
        QFile *skype_log_fp;
        QString skyserv_log_file;
        QFile *skyserv_log_fp;
        time_t skype_last_crash_time;
        time_t skyserv_last_alive_time;
        time_t skyserv_last_crash_time;
        bool  set_stop;
    };
    QHash<int, LineAttribute *> iattrs;
    // QTimer *check_alive_timer;

    QString main_log_file;
    QFile *main_log_fp;
    QString log_msg;

    int last_ins_count;
    int curr_ins_count;  // 当前已经有多少个实例
    int init_ins_count;  // 启动初始化时的实例个数
    QString rt_path;
    QString key_path_base;
    QString sk_dbpath_base;
    int rt_count;
    unsigned short sk_port_base;
    QHash<QProcess*, int> procs; // proc, idx

    Database *m_db;

    // boost::signals2::signal<void (int, char*)> check_alive_signal;
    // boost::asio::deadline_timer *check_alive_timer_b;
    // boost::asio::io_service bio_srv;
    MyTimer *check_alive_timer_b;
};

#endif /* _KRTMAN_H_ */

