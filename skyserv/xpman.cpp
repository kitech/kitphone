// krtman.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-01-05 22:45:16 +0800
// Version: $Id: krtman.cpp 676 2011-01-08 16:05:54Z drswinghead $
// 

#include <cassert>
#include <QtCore>

#include "../libng/getopt_pp_standalone.h"

#include "configs.h"
#include "xpapp.h"
#include "xpman.h"

#include "database.h"

#include <algorithm>

//#define VNC_PORT_BASE 6000
//#define SCN_SEQ_BASE 1000

#define VNC_PORT_BASE 5900 // 
#define SCN_SEQ_BASE 999 // 


int run_as_server = 1;

/////////////signal
static int setup_unix_signal_handlers()
{
    struct sigaction hup, term, sint, quit;

    // hup.sa_handler = MyDaemon::hupSignalHandler;
    hup.sa_handler = KitRuntimeManager::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0) > 0)
        return 1;

    // term.sa_handler = MyDaemon::termSignalHandler;
    term.sa_handler = KitRuntimeManager::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
        return 2;

    sint.sa_handler = KitRuntimeManager::intSignalHandler;
    sigemptyset(&sint.sa_mask);
    sint.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &sint, 0) > 0) {
        return 3;
    }

    quit.sa_handler = KitRuntimeManager::quitSignalHandler;
    sigemptyset(&quit.sa_mask);
    quit.sa_flags |= SA_RESTART;

    if (sigaction(SIGQUIT, &quit, 0) > 0) {
        return 4;
    }

    return 0;
}

QString packArguments(QCoreApplication *app, int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    int cargc = 0;
    QString cmdLine;
    QStringList args;

    args = app->arguments();
#ifdef Q_OS_WIN
    // the string given by arguments().at(0) might not be the program name on Windows, 
    // depending on how the application was started.
    // args.prepend(app->applicationFilePath());
    if (args.count() == 0) {
        args.append(app->applicationFilePath());
    } else if (!args.at(0).endsWith("xpman.exe", Qt::CaseInsensitive)) {
        args.prepend(app->applicationFilePath());
    } else {
        // ok, get argument's mechinism the same as *nix, go on 
    }
#endif

    cargc = args.count();
    
    cmdLine = args.join(" ");
    
    qDebug()<<__FUNCTION__<<cmdLine;
    return cmdLine;
}
// unpack is not need, because getopt4 support QStringList format cmdLine.
// int unpackArguments(QString cmdLine, char **argv);

int unpackArguments(QString cmdline, int &argc, char ***argv)
{
    QString uri, uri2;
    QString refer, refer2;
    QString shortArgName;
    QString longArgName;
    QString defaultArgValue = QString::null;
    QStringList args;
    char **v;

    args = cmdline.split(" ");
    // args.takeFirst(); // it is app name

    // int argc = args.count();
    // char *argv[100] = {0};
    argc = args.count();
    v = *argv = (char**)calloc(100, sizeof(char*));
    
    for (int i = 0; i < argc; i ++) {
        v[i] = strdup(args.at(i).toAscii().data());
    }

    return argc;

}

int releaseArguments(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
    // 
    return argc;
}

static void print_help()
{
    printf("xpman Usage: (xpman 1.0 beta1)\n"
           "\t -h --help\n"
           "\t -v --version\n"
           "\t -E --exit\n"
           "\t    --status\n"
           "\t -N --parral numberparral default 20.\n"
           "\t -c --config configfile\n"
           "\t -l --reload\n"
           "\t -S --startall\n"
           "\t -s --start idx\n"
           "\t -R --restartall\n"
           "\t -r --restart idx\n"
           "\t -Q --quitall\n"
           "\t -q --quit idx\n"
           //           "\t -F --fixall\n"   // 现在这个功能是自动的，不需要外界干预也能完成修复任务
           //           "\t -f --fix idx\n"
           "\t    --quitskyserv idx\n"
           "\t    --quitskyservall\n"
           "\t -i --incr numberincr\n"
           "\t -d --decr numberdecr\n"
           
           "\t -m --mode <forward|router|both>  defulat 1\n"    // 不能以客户方式指定该参数
           "\t    --restartrouter\n" // for router server mode
           "\t    --quitrouter\n"
           "\n"
           "written by liuguangzhao@tomonline-inc.com\n"
           "\n"
           );
}

XpApp *eapp = NULL;
int main(int argc, char **argv)
{
    // QCoreApplication a(argc, argv);
    // QtSingleCoreApplication app(argc, argv);

    bool q_help = false;
    GetOpt::GetOpt_pp args(argc, argv);
    args >> GetOpt::OptionPresent('h', "help", q_help);
    if (q_help) {
        print_help();
        return 0;
    }

    if (setup_unix_signal_handlers() != 0) {
        fprintf(stderr, "install unix handler faild.\n");
        exit(3);
    }

    XpApp app(argc, argv);
    ::eapp = &app;
    if (app.isRunning()) {
        run_as_server = 0;
        qDebug()<<"Another instance of xpman is running.";
        QObject::connect(&app, SIGNAL(messageReceived(const QString&)), 
                         &app, SLOT(handleMessage(const QString &)));

        bool sendok;
        QString msg("sayhello: hi master, are you ok?");
        // bool sendok = app.sendMessage(msg);
        msg = "cmdline:" + packArguments(&app, argc, argv);
        sendok = app.sendMessage(msg, 0);

        if (sendok) {
        } else {
        }
        qDebug()<<"waiting server process's response...";
        // wait for server process's response
        return app.exec();
        return 0;
    }

    // enter server mode
    int parral_number = 0;
    args >> GetOpt::Option('N', "parral", parral_number);
    if (parral_number < 0 || parral_number > 1000) {
        printf("parral number error: should 0 < -N < 100 \n");
        return -1;
    }
    if (parral_number == 0) {
        printf("parral number should not be 0, set default 3 now.\n");
        parral_number = 3;
    }

    std::string config_file;
    args >> GetOpt::Option('c', "config", config_file);
    if (config_file.length() > 0) {
        
    }

    // KitRuntimeManager manager(3);
    KitRuntimeManager manager(parral_number);
    if (!manager.parserArguments(argc, argv)) {
        return -1;
    }

    // QObject::connect(&app, SIGNAL(aboutToQuit()), &manager, SLOT(onAboutToQuit()));
    QObject::connect(&app, SIGNAL(messageReceived(const QString&, int)),
                     &manager, SLOT(onOtherInstanceMessageRecived(const QString&, int)));
    // QObject::connect(&app, SIGNAL(messageReceived(const QString&, int)), 
    //                     &app, SLOT(handleMessage(const QString &, int)));

    manager.run();
    int iret = app.exec();

    // no change here if Ctrl + C 
    qDebug()<<"exit now, last clean up chance";

    return iret;
    return 0;
}

struct test_functor {
    void operator()() {
        std::cout<<"vvvvvvvvvv"<<std::endl;
    }
};

void tprint(const boost::system::error_code& /*e*/)
{
    std::cout << "Hello, world!\n";
}

/////////////
////////////
// TODO 退出时数据库中状态重置问题。
// TODO 给数据库一个心跳包状态及时间？？？
KitRuntimeManager::KitRuntimeManager(int count, QObject *parent)
    : QThread(parent)
{
    boost::signals2::signal<void ()> sig;
    sig.connect(test_functor());

    sig();

    /////////////////////
    this->init_unix_signal_handlers();

    this->m_db = new Database();
    if (!this->m_db->connectdb()) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"connect database error";
        exit(-1);
    }

    this->server_mode = SM_FORWARDER;
    this->sk_port_base = 8950 - 1;
    this->curr_ins_count = this->init_ins_count = this->rt_count = count;
    this->rt_path = "";

    // detect skrt dir
    QString home_dir = QString(getenv("HOME"));
    this->rt_path = home_dir + "/" + Configs().get_sk_path();
    this->rt_path += "/" + Configs().get_rt_name();
    // the path should get from .ini file
    // this->rt_path = home_dir + QString("/sxxxxxkxxxit/dis-kkkkkkkkk-path/bin/linux-x86");
    // this->rt_path += "/linux-x86-skypekit-voicepcm-novideo";

    // ~/skype_cluster/ipc_keys/ipc_key_0/
    this->key_path_base = home_dir + "/" + Configs().get_storage_path() + "/ipc_keys/ipc_key"; 
    // qDebug()<<"File path:"<<this->rt_path<<this->key_path_base;

    this->sk_dbpath_base = home_dir + "/" + Configs().get_storage_path() + "/gateways";

    this->xvfb_exec_path = "/usr/bin/Xvfb";
    this->vnc_exec_path = "/usr/bin/x11vnc";
    this->skype_router_exec_path = home_dir + "/" + Configs().getMultiCallSkypePath();
    this->skype_switcher_exec_path = home_dir + "/" + Configs().getSingleCallSkypePath();
    this->skyserv_exec_path = Configs().get_self_app_path() + "/skyserv";
    this->obox_exec_path = "/usr/bin/openbox";
    this->awesome_exec_path = "/usr/bin/awesome";
    this->fvwm_exec_path = "/usr/bin/fvwm";
    this->xterm_exec_path = "/usr/bin/xterm";

    if (!QFile(this->skype_switcher_exec_path).exists()) {
        assert(1==2);
        // 找不到执行程序可不行。
        // this->skype_switcher_exec_path = "/usr/bin/skype";
    }

    Q_ASSERT(QFile(this->skype_router_exec_path).exists());
    Q_ASSERT(QFile(this->skype_switcher_exec_path).exists());

    this->cfg_dir = sk_dbpath_base;
    this->log_dir = home_dir + "/" + Configs().get_storage_path() + "/logs";
    this->pid_dir = home_dir + "/" + Configs().get_storage_path() + "/pids";

    this->routers = Configs().getSkypeRouters();
    qDebug()<<this->routers;
    this->switchers = Configs().getSwitchers();
    qDebug()<<this->switchers;

    AccountState acc;
    std::for_each(this->routers.begin(), this->routers.end(),
                  [&] (QPair<QString,QString> &elm) {
                      acc = {elm.first, elm.second, true, -1, QDateTime::currentDateTime()};
                      this->router_pool.append(acc);
                      
                  });

    std::for_each(this->switchers.begin(), this->switchers.end(),
                  [&] (QPair<QString,QString> &elm) {
                      acc = {elm.first, elm.second, true, -1, QDateTime::currentDateTime()};
                      this->switcher_pool.append(acc);
                  });

    this->main_log_file = this->log_dir + "/xpman.log";
    this->main_log_fp = new QFile(this->main_log_file);
    this->main_log_fp->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered);

    // this->check_alive_timer = new QTimer();
    // this->check_alive_timer->setInterval(1000 * 20);
    // QObject::connect(this->check_alive_timer, SIGNAL(timeout()), 
    //                  this, SLOT(on_check_process_alive()));

    //// new c++0x
    // this->check_alive_timer_b = new boost::asio::deadline_timer(this->bio_srv, boost::posix_time::seconds(20));
    // // this->check_alive_timer_b->async_wait(tprint);
    // this->check_alive_timer_b->async_wait(boost::bind(&KitRuntimeManager::on_check_process_alive_c, this, _1));
    // this->bio_srv.run();
    
    // this->check_alive_signal.connect(boost::bind(&KitRuntimeManager::on_check_process_alive_b, this, _1, _2));
    this->check_alive_timer_b = new MyTimer();
    this->check_alive_timer_b->setInterval(1000 * 20);
    // this->check_alive_timer_b->connect(boost::bind(&KitRuntimeManager::on_check_process_alive_b, this));
    // this->check_alive_timer_b->connect(boost::bind(&KitRuntimeManager::on_check_process_alive, this));
    this->check_alive_timer_b->connect(boost::bind(&KitRuntimeManager::on_check_process_alive, this));
    // this->check_alive_timer_b->start();
}

void KitRuntimeManager::on_check_process_alive_c(const boost::system::error_code& e)
{
    //
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<e;
}

KitRuntimeManager::~KitRuntimeManager()
{
    // this->check_alive_timer->stop();
    // delete this->check_alive_timer;

    this->check_alive_timer_b->stop();
    delete this->check_alive_timer_b;

    this->main_log_fp->close();
    delete this->main_log_fp;
}



// static 
int KitRuntimeManager::sighupFd[2] = {0};
int KitRuntimeManager::sigtermFd[2] = {0};
int KitRuntimeManager::sigintFd[2] = {0};
int KitRuntimeManager::sigquitFd[2] = {0};

// static 
void KitRuntimeManager::init_unix_signal_handlers()
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, KitRuntimeManager::sighupFd)) {
        qFatal("Couldn't create HUP socketpair");
    }
    
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, KitRuntimeManager::sigtermFd)) {
        qFatal("Couldn't create TERM socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, KitRuntimeManager::sigintFd)) {
        qFatal("Couldn't create INT socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, KitRuntimeManager::sigquitFd)) {
        qFatal("Couldn't create QUIT socketpair");
    }

    this->snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

    this->snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));

    this->snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

    this->snQuit = new QSocketNotifier(sigquitFd[1], QSocketNotifier::Read, this);
    QObject::connect(this->snQuit, SIGNAL(activated(int)), this, SLOT(handleSigQuit()));

}

// static 
void KitRuntimeManager::hupSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(KitRuntimeManager::sighupFd[0], &a, sizeof(a));
}

// static 
void KitRuntimeManager::termSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(KitRuntimeManager::sigtermFd[0], &a, sizeof(a));
}

// static 
void KitRuntimeManager::intSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(KitRuntimeManager::sigintFd[0], &a, sizeof(a));
}

// static 
void KitRuntimeManager::quitSignalHandler(int unused)
{
    Q_UNUSED(unused);
    fprintf(stdout, "%s %d %s, \n", __FILE__, __LINE__, __FUNCTION__);
    char a = 1;
    ::write(KitRuntimeManager::sigquitFd[0], &a, sizeof(a));
}

// 重加载配置文件!!!
void KitRuntimeManager::handleSigHup()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snHup->setEnabled(false);
     char tmp;
     ::read(sighupFd[1], &tmp, sizeof(tmp));

     // do Qt stuff

    this->snHup->setEnabled(true);
}
void KitRuntimeManager::handleSigTerm()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snTerm->setEnabled(false);
     char tmp;
     ::read(sigtermFd[1], &tmp, sizeof(tmp));

     // do Qt stuff

    this->snTerm->setEnabled(true);
}

// 妥善退出
void KitRuntimeManager::handleSigInt()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snInt->setEnabled(false);
     char tmp;
     ::read(sigintFd[1], &tmp, sizeof(tmp));

     // do Qt stuff
     // this->on_app_want_quit();

    this->onAboutToQuit();

    this->snInt->setEnabled(true);
}
// 妥善退出
void KitRuntimeManager::handleSigQuit()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"";
    this->snQuit->setEnabled(false);
     char tmp;
     ::read(sigquitFd[1], &tmp, sizeof(tmp));

     // do Qt stuff
     // this->on_app_want_quit();


    this->snQuit->setEnabled(true);

    //    this->onAboutToQuit();
}

void KitRuntimeManager::onAboutToQuit()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QProcess *proc = nullptr;
    int seq = 0;

    auto it = this->skyserv_procs.leftBegin();
    for (; it != this->skyserv_procs.leftEnd(); it++) {
        proc = it.key();
        this->disconnect_process_slot(proc);
        proc->terminate();
        proc->waitForFinished();
    }

    it = this->xvfb_procs.leftBegin();
    for (; it != this->xvfb_procs.leftEnd(); it++) {
        proc = it.key();
        this->disconnect_process_slot(proc);
        proc->terminate();
        proc->waitForFinished();
    }

    this->set_all_line_exit();

    qApp->quit();
}


// void KitRuntimeManager::run_old()
// {
//     QProcess *proc = NULL;
//     for (int i = 0 ; i < this->rt_count ; i ++) {
//         proc = new QProcess(this);
//         QObject::connect(proc, SIGNAL(error(QProcess::ProcessError)),
//                          this, SLOT(on_error(QProcess::ProcessError)));
//         QObject::connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
//                          this, SLOT(on_finished(int, QProcess::ExitStatus)));
//         QObject::connect(proc, SIGNAL(readyReadStandardError()),
//                          this, SLOT(on_readyReadStandardError()));
//         QObject::connect(proc, SIGNAL(readyReadStandardOutput()),
//                          this, SLOT(on_readyReadStandardOutput()));
//         QObject::connect(proc, SIGNAL(started()),
//                          this, SLOT(on_started()));
//         QObject::connect(proc, SIGNAL(stateChanged(QProcess::ProcessState)),
//                          this, SLOT(on_stateChanged(QProcess::ProcessState)));

//         this->procs.insert(proc, i);

//         this->manual_start(proc, i);
//     }
// }


int KitRuntimeManager::manual_start_xvfb(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;
    int screen_no = seq + SCN_SEQ_BASE;

    str2 = this->sk_dbpath_base + QString("/sd_%1").arg(seq);
    if (!QDir().exists(str2)) {
        QDir().mkpath(str2);
    }

    args.clear();
    //     $XVFB :$SCRN -pixdepths 32 -screen :$SCRN 800x600x8 &
    args << QString(":%1").arg(screen_no);
    args << "-pixdepths" << QString::number(32);
    args << "-screen" << QString(":%1").arg(screen_no);
    args << "800x600x8";
    args << "-nolisten" << "TCP";

    proc->start(this->xvfb_exec_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);
    // proc->start(this->rt_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);

    // qDebug()<<args;

    return 0;
}

int KitRuntimeManager::manual_start_obox(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;
    int screen_no = seq + SCN_SEQ_BASE;

    args.clear();
    args << "--sm-disable";
    // args << "--restart";
    // args << "--config-file" << "/dev/null";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DISPLAY", QString(":%1").arg(screen_no));
    proc->setProcessEnvironment(env);

    proc->start(this->obox_exec_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);

    // qDebug()<<args;

    return 0;
}

int KitRuntimeManager::manual_start_xterm(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;
    int screen_no = seq + SCN_SEQ_BASE;

    args.clear();
    // args << "--sm-disable";
    // args << "--restart";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DISPLAY", QString(":%1").arg(screen_no));
    proc->setProcessEnvironment(env);

    proc->start(this->xterm_exec_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);

    // qDebug()<<args;

    return 0;
}

int KitRuntimeManager::manual_start_vnc(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;
    int screen_no = seq + SCN_SEQ_BASE;
    unsigned short vnc_port = seq + VNC_PORT_BASE;

    str = this->iattrs.value(seq)->vnc_log_file;
    str2 = "0.0.0.0";
    str3 = QString("skyvnc.%1").arg(screen_no);

    args.clear();
    // args << "--sm-disable";
    // args << "--restart";
    
    // $XVNC -forever -listen $LSN_IP -autoport $VNCPORT &
    args << "-forever";
    args << "-listen" << str2;
    args << "-autoport" << QString::number(vnc_port);
    args << "-o" << str;
    args << "-ncache" << QString::number(10);
    args << "-passwd" << str3;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DISPLAY", QString(":%1").arg(screen_no));
    proc->setProcessEnvironment(env);

    proc->start(this->vnc_exec_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);

    qDebug()<<args;

    return 0;
}

int KitRuntimeManager::manual_start_skype(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;
    int screen_no = seq + SCN_SEQ_BASE;
    QString dbpath = QString("%1/sd%2").arg(this->cfg_dir).arg(screen_no);
    AccountState *acc = nullptr;

    args.clear();
    // args << "--sm-disable";
    // args << "--restart";
    //    echo ${AUDIO_SWITCHER_USERS[$idx]} ${AUDIO_SWITCHER_PASSWORDS[$idx]} | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} >$LOGDIR/skype_${SCRN}.log 2>&1 &
    args << "--pipelogin";
    args << QString("--dbpath=%1").arg(dbpath);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DISPLAY", QString(":%1").arg(screen_no));
    proc->setProcessEnvironment(env);

    if (!QDir(dbpath).exists()) {
        QDir().mkpath(dbpath);
    }

    str = dbpath + "/shared.xml"; // new file name
    str2 = Configs().get_self_app_path() + "/shared.xml";
    if (!QFile::exists(str)) {
        QFile::copy(str2, str);
    }

    // QPair<QString, QString> switcher;
    // Q_ASSERT(seq < this->switchers.count());
    // switcher = this->switchers.at(seq);
    if (this->server_mode == SM_FORWARDER
        || (this->server_mode == SM_BOTH && seq != 0)) {
        if (this->find_candidate_account(&acc, SM_FORWARDER)) {
            acc->line_no = seq;
            qDebug()<<"Using free switch account."<<acc->name;
        } else {
            qDebug()<<"Can not find a free switcher account.";
        }
    } else if (this->server_mode == SM_ROUTER
               || (this->server_mode == SM_BOTH && seq == 0)) {
        if (this->find_candidate_account(&acc, SM_ROUTER)) {
            acc->line_no = seq;
            qDebug()<<"Using free router account."<<acc->name;
        } else {
            qDebug()<<"Can not find a free router account.";
        }
    } else {
        assert(1 == 2);
    }
    assert(acc->line_no == seq);

    // str = dbpath + QString("/%1/").arg(switcher.first);
    str = dbpath + QString("/%1/").arg(acc->name);
    if (!QDir(str).exists()) {
        QDir().mkpath(str);
    }

    // str = dbpath + QString("/%1/config.xml").arg(switcher.first);
    str = dbpath + QString("/%1/config.xml").arg(acc->name);
    str2 = Configs().get_self_app_path() + "/config.xml";
    if (!QFile::exists(str)) {
        QFile::copy(str2, str);
    }
   
    acc = nullptr;
    if (this->server_mode == SM_FORWARDER
        || (this->server_mode == SM_BOTH && seq != 0)) {
        if (this->find_account_by_seq(&acc, seq)) {
            qDebug()<<"starting switcher mode skype:"<<this->skype_switcher_exec_path;
            proc->start(this->skype_switcher_exec_path, args, QIODevice::ReadWrite | QIODevice::Unbuffered);
        } else {
            qDebug()<<"can not find preprocessed switcher skype:"<<this->skype_switcher_exec_path;
        }
    } else if (this->server_mode == SM_ROUTER
               || (this->server_mode == SM_BOTH && seq == 0)) {
        if (this->find_account_by_seq(&acc, seq)) {
            qDebug()<<"starting router mode skype:"<<this->skype_router_exec_path;
            proc->start(this->skype_router_exec_path, args, QIODevice::ReadWrite | QIODevice::Unbuffered);
        } else {
            qDebug()<<"can not find preprocessed router skype:"<<this->skype_router_exec_path;
        }
    } else {
        assert(1 == 2);
    }

    // qDebug()<<args;

    return 0;
}

int KitRuntimeManager::manual_start_skyserv(QProcess *proc, int seq)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<seq;
    QString str, str2, str3;
    QStringList args;
    int screen_no = seq + SCN_SEQ_BASE;

    args.clear();
    // args << "--sm-disable";
    // args << "--restart";
    //    echo ${AUDIO_SWITCHER_USERS[$idx]} ${AUDIO_SWITCHER_PASSWORDS[$idx]} | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} >$LOGDIR/skype_${SCRN}.log 2>&1 &
    // args << "--pipelogin";
    // args << QString("--dbpath=%1/sd%2").arg(this->cfg_dir).arg(screen_no);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("DISPLAY", QString(":%1").arg(screen_no));
    proc->setProcessEnvironment(env);

    proc->start(this->skyserv_exec_path, args, QIODevice::ReadWrite | QIODevice::Unbuffered);

    // qDebug()<<args;

    return 0;
}


void KitRuntimeManager::run()
{

    QProcess *proc = NULL;
    LineAttribute *attr;
    bool bret;

    this->set_all_line_exit();

    // 
    for (int i = 0 ; i <= this->rt_count ; i ++) {

        attr = new LineAttribute();
        attr->seq = i;
        attr->screen_no = i + SCN_SEQ_BASE;
        attr->vnc_port = i + VNC_PORT_BASE;

        attr->vnc_log_file = QString("%1/vnc_%2.log").arg(this->log_dir).arg(attr->screen_no);

        attr->skype_log_file = QString("%1/skype_%2.log").arg(this->log_dir).arg(attr->screen_no);
        qDebug()<<"Ready open skype log file:"<<attr->skype_log_file;
        attr->skype_log_fp = new QFile(attr->skype_log_file);
        bret = attr->skype_log_fp->open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Unbuffered);
        Q_ASSERT(bret);

        attr->skyserv_log_file = QString("%1/skyserv_%2.log").arg(this->log_dir).arg(attr->screen_no);
        qDebug()<<"Ready open skyserv log file:"<<attr->skyserv_log_file;
        attr->skyserv_log_fp = new QFile(attr->skyserv_log_file);
        bret = attr->skyserv_log_fp->open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Unbuffered);
        Q_ASSERT(bret);
        
        this->iattrs.insert(i, attr);

        ///// 只在需要的时候启动Xvfb
        if ((this->server_mode == SM_ROUTER && i == 0)
            || (this->server_mode == SM_FORWARDER && i != 0)
            || (this->server_mode == SM_BOTH)) {
            proc = new QProcess(this);
            this->connect_process_slot(proc);

            this->xvfb_procs.insert(proc, i);
            this->manual_start_xvfb(proc, i);
        }
    }
}

void KitRuntimeManager::connect_process_slot(QProcess *proc)
{
    QObject::connect(proc, SIGNAL(error(QProcess::ProcessError)),
                     this, SLOT(on_error(QProcess::ProcessError)));
    QObject::connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(on_finished(int, QProcess::ExitStatus)));
    QObject::connect(proc, SIGNAL(readyReadStandardError()),
                     this, SLOT(on_readyReadStandardError()));
    QObject::connect(proc, SIGNAL(readyReadStandardOutput()),
                     this, SLOT(on_readyReadStandardOutput()));
    QObject::connect(proc, SIGNAL(started()),
                     this, SLOT(on_started()));
    QObject::connect(proc, SIGNAL(stateChanged(QProcess::ProcessState)),
                     this, SLOT(on_stateChanged(QProcess::ProcessState)));

}

void KitRuntimeManager::disconnect_process_slot(QProcess *proc)
{
    QObject::disconnect(proc, SIGNAL(error(QProcess::ProcessError)),
                     this, SLOT(on_error(QProcess::ProcessError)));
    QObject::disconnect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(on_finished(int, QProcess::ExitStatus)));
    QObject::disconnect(proc, SIGNAL(readyReadStandardError()),
                     this, SLOT(on_readyReadStandardError()));
    QObject::disconnect(proc, SIGNAL(readyReadStandardOutput()),
                     this, SLOT(on_readyReadStandardOutput()));
    QObject::disconnect(proc, SIGNAL(started()),
                     this, SLOT(on_started()));
    QObject::disconnect(proc, SIGNAL(stateChanged(QProcess::ProcessState)),
                     this, SLOT(on_stateChanged(QProcess::ProcessState)));
}

bool KitRuntimeManager::set_all_line_exit()
{
    bool bret;

    // 统一设置更新网关状态，当配置的网关数大于当前指定的启动个数时
    // 这个更新设置是必须的，否则会出现线路假不能用的误会。
    QStringList names;
    if (this->server_mode == SM_FORWARDER || this->server_mode == SM_BOTH) {
        std::for_each(this->switcher_pool.begin(), this->switcher_pool.end(),
                      [&names] (AccountState &elm) {
                          names << elm.name;
                      });
        Q_ASSERT(names.count() > 0);
        bret = this->m_db->setBatchLineState(names, 2); // 设置为正常退出状态。
    }

    return true;
}

void KitRuntimeManager::on_check_process_alive()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;

    int seq;
    LineAttribute *attr = NULL;
    QHash<int, LineAttribute*>::iterator it;
    QDateTime last_modify_time;
    QProcess *proc = NULL;

    int alive_max_silent_time = 30; // 超过这个时间，监控就要认为这个进行已经死锁了，需要重启动。

    for (it = this->iattrs.begin(); it != this->iattrs.end(); ++it) {
        seq = it.key();
        attr = it.value();

        // check skyserv
        last_modify_time = QFileInfo(attr->skyserv_log_file).lastModified();
        if (attr->skyserv_last_alive_time > 0
            && last_modify_time.toTime_t() - alive_max_silent_time > attr->skyserv_last_alive_time ) {
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"skyserv deadlock detected:"<<seq;
            
            Q_ASSERT(this->skyserv_procs.rightContains(seq));
            proc = this->skyserv_procs.findRight(seq).value();

            if (proc->state() == QProcess::Running) {
                proc->terminate();
            }
        }

        // check what???
    }

    // 
    
}

void KitRuntimeManager::on_check_process_alive_b()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    
}

void KitRuntimeManager::catch_crash_backtrace(QProcess *proc, int seq)
{
    // gdb --batch -ex "bt" ./skyserv core.12345

    QString str;
    QStringList args;

    str = QString("core.%1").arg(proc->pid());
    args << "--batch" << "--ex" << "bt";
    args << "./skyserv" << str;

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"catching crash backtrace:"<<str;

    if (QFile::exists(str)) {
        QProcess *cproc = new QProcess(this);
        QObject::connect(cproc,  SIGNAL(finished(int, QProcess::ExitStatus)),
                         this, SLOT(on_catch_crash_proc_finished(int, QProcess::ExitStatus)));
        QObject::connect(cproc, SIGNAL(readyReadStandardOutput()),
                         this, SLOT(on_catch_crash_proc_readyReadStandardOutput()));
    } else {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"can not find core file:"<<str;
    }
}

void KitRuntimeManager::on_catch_crash_proc_finished ( int exitCode, QProcess::ExitStatus exitStatus )
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;

    proc->deleteLater();
}

void KitRuntimeManager::on_catch_crash_proc_readyReadStandardOutput()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;

    QByteArray ba = proc->readAllStandardOutput();
    this->main_log_fp->write(ba);
}

// depcreated
int KitRuntimeManager::manual_start(QProcess *proc, int seq)
{
    QString str, str2, str3;
    QStringList args;

    str = this->key_path_base + QString("_%1/").arg(seq);
    if (!QDir().exists(str)) {
        QDir().mkpath(str);
    }

    str2 = this->sk_dbpath_base + QString("/sd_%1").arg(seq);
    if (!QDir().exists(str2)) {
        QDir().mkpath(str2);
    }

    str3 = this->key_path_base + QString("_un_name_%1.sock").arg(seq);

    args.clear();
    args<<"-m";
    // args<<"-p" << QString::number(this->sk_port_base + seq);
    args<<"-l" << str3;
    args<<"-s" << str;
    args<<"-f" << str2;
    args<<"-d" << str2;

    proc->start(this->rt_path, args, QIODevice::ReadOnly | QIODevice::Unbuffered);

    // qDebug()<<args;

    return 0;
}

// LRU算法
bool KitRuntimeManager::find_candidate_account(AccountState **account, int serv_mode)
{
    Q_ASSERT(serv_mode > SM_MIN && serv_mode < SM_MAX && serv_mode != SM_BOTH);
    bool found = false;
    int max_dist_time = -1;
    int dist_time = -1;
    int last_candidate_pos = -1;
    int i = 0;

    QDateTime now_time = QDateTime::currentDateTime();

    if (serv_mode == SM_ROUTER) {
        std::for_each(this->router_pool.begin(), this->router_pool.end(),
                      [&] (AccountState &acc) -> AccountState & {
                          if (acc.is_free) {
                              dist_time = acc.offtime.secsTo(now_time);
                              qDebug()<<acc.name<<" now:"<<now_time<<" it:"<<acc.offtime<<" ->"<<dist_time;
                              if (dist_time > max_dist_time) {
                                  last_candidate_pos = i;
                                  max_dist_time = dist_time;
                              }
                          }
                          i++;
                          return acc;
                      });
        if (last_candidate_pos < 0) {
        } else {
            *account = &this->router_pool[last_candidate_pos];
            (*account)->is_free = false;
            found = true;
        }
    } else if (serv_mode == SM_FORWARDER) {
        std::for_each(this->switcher_pool.begin(), this->switcher_pool.end(),
                      [&] (AccountState &acc) -> AccountState & {
                          if (acc.is_free) {
                              dist_time = acc.offtime.secsTo(now_time);
                              if (dist_time > max_dist_time) {
                                  last_candidate_pos = i;
                                  max_dist_time = dist_time;
                              }
                          }
                          i++;
                          return acc;
                      });
        if (last_candidate_pos < 0) {
        } else {
            *account = &this->switcher_pool[last_candidate_pos];
            (*account)->is_free = false;
            found = true;
        }
    } else {
        assert(1 == 2);
    }

    return found;    
}

bool KitRuntimeManager::find_account_by_seq(AccountState **account, int seq)
{
    bool found = false;
    int i = 0;
    if (seq == 0) {
        std::for_each(this->router_pool.begin(), this->router_pool.end(),
                      [&] (AccountState &acc) -> AccountState & {
                          if (acc.line_no == seq) {
                              *account = &this->router_pool[i];
                              found = true;
                              return acc;
                          }
                          i++;
                          return acc;
                      });
    } else {
        std::for_each(this->switcher_pool.begin(), this->switcher_pool.end(),
                      [&] (AccountState &acc) -> AccountState & {
                          if (acc.line_no == seq) {
                              *account = &this->switcher_pool[i];
                              found = true;
                              return acc;
                          }
                          i++;
                          return acc;
                      });
    }
    return found;
}

bool KitRuntimeManager::dump_accounts_info()
{
    
    return true;
}

void KitRuntimeManager::on_error(QProcess::ProcessError error)
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<error<<proc->errorString();

    if (this->skyserv_procs.leftContains(proc)) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<error<<proc->errorString()<<"skyserv process error";        
    }
}

void KitRuntimeManager::on_finished ( int exitCode, QProcess::ExitStatus exitStatus )
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<exitCode<<proc->error()<<proc->errorString();

    QProcess *nproc = nullptr;
    QProcess *sproc = nullptr;
    int screen_no;
    int seq;
    int iret;
    QPair<QString,QString> switcher;
    QString str;
    LineAttribute *attr = nullptr;
    AccountState *acc = nullptr;

    switch (exitStatus) {
    case QProcess::CrashExit:
        if (this->xvfb_procs.leftContains(proc)) {
            seq = this->xvfb_procs.findLeft(proc).value();
            screen_no = seq + SCN_SEQ_BASE;
            this->manual_start_xvfb(proc, seq);
        } else if (this->skyserv_procs.leftContains(proc)) {
            seq = this->skyserv_procs.findLeft(proc).value();
            screen_no = seq + SCN_SEQ_BASE;
            
            nproc = this->xvfb_procs.findRight(seq).value();
            sproc = this->skype_procs.findRight(seq).value();
            if (nproc->state() == QProcess::Running && sproc->state() == QProcess::Running) {
                this->manual_start_skyserv(proc, seq);
            }
        } else if (this->skype_procs.leftContains(proc)) {
            seq = this->skype_procs.findLeft(proc).value();
            screen_no = seq + SCN_SEQ_BASE;

            // 切换用户接着上
            if (!this->find_account_by_seq(&acc, seq)) {
                assert(1==2);
            }
            if (seq == 0) {
            } else {
                this->m_db->setLineState(acc->name, Database::CLS_CRASH);
            }
            acc->offtime = QDateTime::currentDateTime();
            acc->is_free = true;
            acc->line_no = -1;
            qDebug()<<"Ready swtich to LRU router candidate: current: "<<acc->name;

            // skype退出了，需要把Xvfb退出，重新登陆才可。
            // 否则造成skyserv连接不到新登陆的skype上
            // TODO 内在清理。
            nproc = this->xvfb_procs.findRight(seq).value();
            {
                this->disconnect_process_slot(this->skyserv_procs.findRight(seq).value());
                this->disconnect_process_slot(this->skype_procs.findRight(seq).value());
                // this->disconnect_process_slot(this->xvfb_procs.findRight(seq).value());
                this->disconnect_process_slot(this->vnc_procs.findRight(seq).value());

                this->skyserv_procs.removeRight(seq);
                this->skype_procs.removeRight(seq);
                // this->xvfb_procs.removeRight(seq);
                this->vnc_procs.removeRight(seq);
            }
            nproc->kill();
            
            // if (nproc->state() == QProcess::Running) {
            //     // clean it first
            //     this->disconnect_process_slot(proc);
            //     delete proc;
            //     proc = nullptr;

            //     proc = new QProcess(this);
            //     this->connect_process_slot(proc);

            //     sproc = this->skyserv_procs.findRight(seq).value();
            //     this->disconnect_process_slot(sproc);
            //     // sproc->terminate();
            //     ::kill(sproc->pid(), SIGINT);
            //     this->skyserv_procs.removeLeft(sproc);
            //     sproc->waitForFinished();
            //     delete sproc; sproc = nullptr;

            //     this->manual_start_skype(proc, seq);
            //}
        } else {
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Unhandled process crash event.";
        }
        // Q_ASSERT(this->procs.contains(proc));
        // seq = this->procs.value(proc);
        // Q_ASSERT(seq >= 0 && seq < 10000);
        // this->manual_start(proc, seq);
        
        break;
    case QProcess::NormalExit:
        // maybe should exit
        break;
    default:
        break;
    };

    switch (proc->error()) {
    default:
        break;
    };
}

void KitRuntimeManager::on_readyReadStandardError ()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    Q_UNUSED(proc);

    // QByteArray ba = proc->readAllStandardError();
    // qDebug()<<"INFO:"<<this->procs.value(proc)<<ba.trimmed();
    QByteArray ba = proc->readAllStandardError();

    LineAttribute *attr = NULL;
    int seq = -1;
    int iret = -1;

    if (this->skyserv_procs.leftContains(proc)) {
        seq = this->skyserv_procs.findLeft(proc).value();
        attr = this->iattrs.find(seq).value();

        Q_ASSERT(attr != NULL);

        iret = attr->skyserv_log_fp->write(ba);
        attr->skyserv_last_alive_time = time(NULL);
        // qDebug()<<"redirect to log file:"<<attr->skyserv_log_file<<iret;
    } else if (this->skype_procs.leftContains(proc)) {
        seq = this->skype_procs.findLeft(proc).value();
        attr = this->iattrs.find(seq).value();

        Q_ASSERT(attr != NULL);
        attr->skype_log_fp->write(ba);

        if (QString(ba).indexOf("Incorrect Password") != -1) {
            qDebug()<<"I think account forbiddened";
        }
    } else if (this->vnc_procs.leftContains(proc)) {
        seq = this->vnc_procs.findLeft(proc).value();
        attr = this->iattrs.find(seq).value();

        Q_ASSERT(attr != NULL);
        // attr->vnc_log_fp->write(ba);
    } else {
        qDebug()<<"STDERR:"<<proc<<ba.trimmed();
    }

    // new cxx
    int a = 5;
    char *b = "tv";

    
}

void KitRuntimeManager::on_readyReadStandardOutput ()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    Q_UNUSED(proc);

    // QByteArray ba = proc->readAllStandardOutput();
    // qDebug()<<"INFO:"<<this->procs.value(proc)<<ba.trimmed();

    QByteArray ba = proc->readAllStandardOutput();

    LineAttribute *attr = NULL;
    int seq = -1;

    if (this->skyserv_procs.leftContains(proc)) {
        seq = this->skyserv_procs.findLeft(proc).value();
        attr = this->iattrs.find(seq).value();

        Q_ASSERT(attr != NULL);

        attr->skyserv_log_fp->write(ba);
        attr->skyserv_last_alive_time = time(NULL);
    } else {
        qDebug()<<"STDOUT:"<<proc<<ba.trimmed();
    }
    
}

void KitRuntimeManager::on_started ()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    Q_UNUSED(proc);

    QProcess *nproc = nullptr;
    QProcess *sproc = nullptr;
    int screen_no;
    int seq;
    int iret;
    QPair<QString,QString> switcher;
    QString str;
    AccountState *acc = nullptr;

    if (this->xvfb_procs.leftContains(proc)) {
        seq = this->xvfb_procs.findLeft(proc).value();
        screen_no = seq + SCN_SEQ_BASE;

        sleep(1);

        // start openbox
        nproc = new QProcess(this);
        this->connect_process_slot(nproc);

        this->obox_procs.insert(nproc, seq);
        this->manual_start_obox(nproc, seq);

        // start xterm
        nproc = new QProcess(this);
        this->connect_process_slot(nproc);

        this->xterm_procs.insert(nproc, seq);
        this->manual_start_xterm(nproc, seq);

        // start x11vnc,
        nproc = new QProcess(this);
        this->connect_process_slot(nproc);

        this->vnc_procs.insert(nproc, seq);
        this->manual_start_vnc(nproc, seq);

        // start skype
        nproc = new QProcess(this);
        this->connect_process_slot(nproc);

        this->skype_procs.insert(nproc, seq);
        this->manual_start_skype(nproc, seq);
        
    } else if (this->skype_procs.leftContains(proc)) {
        seq = this->skype_procs.findLeft(proc).value();
        screen_no = seq + SCN_SEQ_BASE;

        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"skype started:"<<seq
                << proc->isWritable();

        // switcher = this->switchers.at(seq);
        if (this->find_account_by_seq(&acc, seq)) {
            // str = QString("%1\n%2\n").arg(switcher.first).arg(switcher.second);
            str = QString("%1\n%2\n").arg(acc->name).arg(acc->passwd);
            iret = proc->write(str.toAscii());

            if (this->skyserv_procs.rightContains(seq)) {
                // 重启动skyserv,这个重启动好象有问题
                // 先把这个完全清除掉。
                sproc = this->skyserv_procs.findRight(seq).value();
                qDebug()<<"restart recoperate skyserv:"<<seq<<sproc<<sproc->pid();
                this->disconnect_process_slot(sproc);
                // sproc->kill();
                ::kill(sproc->pid(), SIGINT);
                this->skyserv_procs.removeRight(seq);
                sproc->waitForFinished();
                delete sproc;
                sproc = nullptr;
            }
            {
                // new  skyserv proc
                nproc = new QProcess(this);
                this->connect_process_slot(nproc);

                this->skyserv_procs.insert(nproc, seq);
                this->manual_start_skyserv(nproc, seq);
            }
        } else {
            assert(1 == 2);
        }
    } else if (this->skyserv_procs.leftContains(proc)) {
        seq = this->skyserv_procs.findLeft(proc).value();
        if (this->find_account_by_seq(&acc, seq)) {
            if (seq == 0) {
            } else {
                this->m_db->setLineState(acc->name, Database::CLS_FREE);
            }
        } else {
            qDebug()<<"i dont known which one online.";
        }
    } else {

    }
}

void KitRuntimeManager::on_stateChanged ( QProcess::ProcessState newState )
{
    QProcess *proc = static_cast<QProcess*>(sender());
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<newState<<proc->pid();
    Q_UNUSED(proc);
}

// depcreated???
bool KitRuntimeManager::parserArguments(int argc, char **argv)
{
    bool help = false;
    bool start_all = false;
    bool restart_all = false;
    bool quit_all = false;
    bool reload = false;
    bool fix_all = false;
    std::string config_file;
    int parral_number = 0;
    int start_idx = -1;
    int restart_idx = -1;
    int quit_idx = -1;
    int fix_idx = -1;
    int incr_count = 0;
    int decr_count = 0;
    std::string server_mode; // put parserH
    std::map<std::string,int> serv_modes = {
        {"forward", SM_FORWARDER},
        {"router", SM_ROUTER},
        {"both", SM_BOTH}
    };
    
    GetOpt::GetOpt_pp args(argc, argv);

    args >> GetOpt::OptionPresent('h', "help", help);
    args >> GetOpt::Option('N', "parral", parral_number);
    args >> GetOpt::Option('c', "config", config_file);

    args >> GetOpt::Option('m', "mode", server_mode);

    // ok
    std::for_each (serv_modes.begin(), serv_modes.end(),
                   [&] (std::pair<std::string, int> elm) {
                       if (server_mode == elm.first) {
                           this->server_mode = elm.second;
                       }
                   });

    return true;
}

void KitRuntimeManager::handleArguments(int argc, char **argv, int client_id)
{
    bool help = false;
    bool exit_app = false;
    bool start_all = false;
    bool restart_all = false;
    bool quit_all = false;
    bool reload = false;
    bool quit_skyserv_all = false;
    bool show_status = false;

    std::string config_file;
    int parral_number = 0;
    int start_idx = -1;
    int restart_idx = -1;
    int quit_idx = -1;
    int quit_skyserv_idx = -1;
    // int fix_idx = -1;
    int incr_count = 0;
    int decr_count = 0;
    // int server_mode = SM_FORWARDER; // put parserH
    bool restart_router = false;
    bool quit_router = false;

    QString resp_msg; // to peer msg
    
    GetOpt::GetOpt_pp args(argc, argv);

    args >> GetOpt::OptionPresent('h', "help", help);

    assert(run_as_server == 1);
    if (help) {
        print_help();
        return;
    }

    args >> GetOpt::OptionPresent('E', "exit", exit_app);
    args >> GetOpt::OptionPresent('l', "reload", reload);
    args >> GetOpt::Option('N', "parral", parral_number);
    // omit parral_number

    args >> GetOpt::OptionPresent('0', "status", show_status);
    args >> GetOpt::OptionPresent('l', "reload", reload);
    
    args >> GetOpt::OptionPresent('S', "startall", start_all);
    args >> GetOpt::Option('s', "start", start_idx);

    if (start_all && start_idx >= 0) {
        resp_msg = "param conflict, --startall and --start";
        eapp->sendMessage(resp_msg, client_id);
        qDebug()<<"param conflict, --startall and --start";
        return;
    }

    args >> GetOpt::OptionPresent('R', "restartall", restart_all);
    args >> GetOpt::Option('r', "restart", restart_idx);
    if (restart_all && restart_idx >= 0) {
        qDebug()<<"param conflict, --restartall and --restart";
        return;
    }

    args >> GetOpt::OptionPresent('Q', "quitall", quit_all);
    args >> GetOpt::Option('q', "quit", quit_idx);
    if (quit_all && quit_idx >= 0) {
        qDebug()<<"param conflict, --quitall and --quit";
        return;
    }
    
    if (start_all && restart_all) {
        qDebug()<<"param conflict, --restartall and --startall";
        return;
    }

    if (start_all && quit_all) {
        qDebug()<<"param conflict, --quitall and --startall";
        return;
    }

    if (restart_all && quit_all) {
        qDebug()<<"param conflict, --quitall and --restartall";
        return;
    }

    args >> GetOpt::OptionPresent('0', std::string("quitskyservall"), quit_skyserv_all);
    args >> GetOpt::Option('0', std::string("quitskyserv"), quit_skyserv_idx);
    if (quit_skyserv_all && quit_skyserv_idx >= 0) {
        qDebug()<<"param conflict, --quitskyservall and --quitskyserv idx";
        return;
    }

    args >> GetOpt::Option('i', "incr", incr_count);
    args >> GetOpt::Option('d', "decr", decr_count);
    if (incr_count > 0 && decr_count > 0) {
        qDebug()<<"param conflict, --incr n and --decr n";
        return;
    }

    args >> GetOpt::OptionPresent('0', std::string("restartrouter"), restart_router);
    args >> GetOpt::OptionPresent('0', std::string("quitrouter"), quit_router);

    // now handle it
    QProcess *proc = NULL;
    int seq = -1;
    LineAttribute *attr = NULL;
    bool bret;

    if (exit_app) {
        qDebug()<<"exit xpman now. Cleaning ...";
        for (int idx = 0 ; idx < this->curr_ins_count; idx ++) {
            proc = this->xvfb_procs.findRight(idx).value();
            this->disconnect_process_slot(proc);
            proc->terminate();
            proc->waitForFinished(1000);
            delete proc;
        }
        qApp->quit();
    } else if (show_status) {
        resp_msg = "checking server status now...\n";
        eapp->sendMessage(resp_msg, client_id);

        resp_msg = "No.\t Xvfb \t Skyserv \t Skype \n";
        eapp->sendMessage(resp_msg, client_id);

        QStringList line_elts;

        for (int idx = 0 ; idx < this->curr_ins_count ; idx++) {
            //eapp->sendMessage(resp_msg.left(resp_msg.length()-1)
            //                + QString::number(idx) + QString("\n"), client_id);
            line_elts.clear();
            line_elts << QString::number(idx);
            proc = this->xvfb_procs.findRight(idx).value();
            if (proc->state() == QProcess::Running) {
                line_elts << "OK";
            } else {
                line_elts << "Err";
            }

            proc = this->skyserv_procs.findRight(idx).value();
            if (proc->state() == QProcess::Running) {
                line_elts << "OK";
            } else {
                line_elts << "Err";
            }

            proc = this->skype_procs.findRight(idx).value();
            if (proc->state() == QProcess::Running) {
                line_elts << "OK";
            } else {
                line_elts << "Err";
            }

            resp_msg = line_elts.join("\t") + "\n";
            eapp->sendMessage(resp_msg, client_id);
        }

        
        eapp->clientAck(client_id);
        qDebug()<<resp_msg;
    } else if (reload) {
        qDebug()<<"Reloading config file ...";
        this->old_switchers = this->switchers;
        this->new_switchers = Configs().getSwitchers();

        // diff the two
        // 最简单的，把当前所有的实例全部 关闭，再重新启动所有新的
        // 但这种方法可能影响比较大
        // 还是一个一个对比，如果用户名或者密码出现了变化，则使用新配置重启这个实例
        // 这对有些skype登陆不上有帮助
        // 对于顺序有变化的情况，但是用户名密码总体都没有变，这个不好处理。
        // 对于有顺序变化的情况，还是重启一次比较保险。

        if (this->new_switchers.count() < this->old_switchers.count()) {
            qDebug()<<"Warning: new config has less count than original one.???";
        } else {
            // TODOing
            assert(1==2);
            this->switchers = this->new_switchers;
            for (int idx = 0; idx < this->curr_ins_count; idx ++) {
                if (this->new_switchers.at(idx).first != this->old_switchers.at(idx).first
                    || this->new_switchers.at(idx).second != this->old_switchers.at(idx).second) {
                    proc = this->skype_procs.findRight(idx).value();
                    proc->terminate();
                    
                    proc = this->skyserv_procs.findRight(idx).value();
                    proc->terminate();
                }
            }
        }
        
    } else if (quit_skyserv_all) {
        qDebug()<<"quiting all skyserv process...";
#if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)) && __GXX_EXPERIMENTAL_CXX0X__)
        typedef decltype(this->skyserv_procs.leftBegin()) TmpType;
        TmpType it = this->skyserv_procs.leftBegin(); // ok
#else
        typeof(this->skyserv_procs.leftBegin()) it = this->skyserv_procs.leftBegin(); // ok
#endif
        // QHash<QProcess*,int>::iterator it = this->skyserv_procs.leftBegin();   // ok 
        // KHash2Map<QProcess*, int>::left_iterator it;
        // QString a  = this->skyserv_procs.leftBegin();
        qDebug()<<this->skyserv_procs;
        for (; it != this->skyserv_procs.leftEnd(); it ++) {
            qDebug()<<it.key()<<it.value();
            seq = it.value();
            proc = it.key();

            proc->terminate();
        }
    } else if (quit_skyserv_idx >= 0) {
        qDebug()<<"quiting skyserv #"<<quit_skyserv_idx<<" ...";
        if (!this->skyserv_procs.rightContains(quit_skyserv_idx)) {
            qDebug()<<"Invalid skyserv idx no:"<<quit_skyserv_idx;
        } else {
            proc = this->skyserv_procs.findRight(quit_skyserv_idx).value();
            proc->terminate();
        }
    } else if (incr_count > 0) {
        this->last_ins_count = this->curr_ins_count;
        this->curr_ins_count += incr_count;
        qDebug()<<"Ready incrment instance count, +"<<incr_count<<" to:"<<this->curr_ins_count;

        for (int i = this->last_ins_count + 0; i < this->curr_ins_count ; i ++) {
            attr = new LineAttribute();
            attr->seq = i;
            attr->screen_no = i + SCN_SEQ_BASE;
            attr->vnc_port = i + VNC_PORT_BASE;
            attr->vnc_log_file = QString("%1/vnc_%2.log").arg(this->log_dir).arg(attr->screen_no);
            attr->skype_log_file = QString("%1/skype_%2.log").arg(this->log_dir).arg(attr->screen_no);
            attr->skype_log_fp = new QFile(attr->skype_log_file);
            bret = attr->skype_log_fp->open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Unbuffered);
            attr->skype_log_fp->seek(attr->skype_log_fp->size());
            Q_ASSERT(bret);

            attr->skyserv_log_file = QString("%1/skyserv_%2.log").arg(this->log_dir).arg(attr->screen_no);
            attr->skyserv_log_fp = new QFile(attr->skyserv_log_file);
            bret = attr->skyserv_log_fp->open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Unbuffered);
            attr->skyserv_log_fp->seek(attr->skyserv_log_fp->size());
            Q_ASSERT(bret);

            Q_ASSERT(!this->iattrs.contains(i));
            this->iattrs.insert(i, attr);

            proc = new QProcess(this);
            this->connect_process_slot(proc);
            Q_ASSERT(!this->xvfb_procs.rightContains(i));
            this->xvfb_procs.insert(proc, i);
            this->manual_start_xvfb(proc, i);
        }
    } else if (decr_count > 0) {
        this->last_ins_count = this->curr_ins_count;
        this->curr_ins_count -= decr_count;
        qDebug()<<"Ready decrment instance count, +"<<decr_count<<" to:"<<this->curr_ins_count;
        
        for (int idx = this->last_ins_count - 1; idx >= curr_ins_count ; idx --) {
            proc = this->skype_procs.findRight(idx).value();
            if (proc) {
                this->disconnect_process_slot(proc);
                this->skype_procs.removeRight(idx);
                proc->terminate();
                proc->waitForFinished(1000);
                delete proc;
            }

            proc = this->skyserv_procs.findRight(idx).value();
            if (proc) {
                this->disconnect_process_slot(proc);
                this->skyserv_procs.removeRight(idx);
                proc->terminate();
                proc->waitForFinished(1000);
                delete proc;
            }

            proc = this->xterm_procs.findRight(idx).value();
            if (proc) {
                this->disconnect_process_slot(proc);
                this->xterm_procs.removeRight(idx);
                proc->terminate();
                proc->waitForFinished(1000);
                delete proc;
            }

            proc = this->vnc_procs.findRight(idx).value();
            if (proc) {
                this->disconnect_process_slot(proc);
                this->xterm_procs.removeRight(idx);
                proc->terminate();
                proc->waitForFinished(1000);
                delete proc;
            }

            proc = this->obox_procs.findRight(idx).value();
            if (proc) {
                this->disconnect_process_slot(proc);
                this->xterm_procs.removeRight(idx);
                proc->terminate();
                proc->waitForFinished(1000);
                delete proc;
            }

            proc = this->xvfb_procs.findRight(idx).value();
            if (proc) {
                this->disconnect_process_slot(proc);
                this->xvfb_procs.removeRight(idx);
                proc->terminate();
                proc->waitForFinished(1000);
                delete proc;
            }
            
            
            attr = this->iattrs.find(idx).value();
            this->iattrs.remove(idx);
            delete attr;
        }

    } else {
        qDebug()<<"Unknown/unsupported params";
        resp_msg = "Unknown/unsupported params, use -h for help";;
        eapp->sendMessage(resp_msg, client_id);
        eapp->clientAck(client_id);
    }
    return;
}

// depcreated
void KitRuntimeManager::onOtherInstanceMessageRecived(const QString &msg)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<msg;

    QString args;
    int argc;
    char **argv = NULL;
    
    if (msg.startsWith("sayhello:")) {
        // qDebug()<<__FUNCTION__<<"You says: "<<msg;
    } else if (msg.startsWith("cmdline:")) {
        args = msg.right(msg.length() - 8);
        
        unpackArguments(args, argc, &argv);

        this->handleArguments(argc, argv, 0);

        releaseArguments(argc, argv);

        // if (!args.at(1).startsWith("--")) {
        //     QString arg2;
        //     for (int i = 1; i < args.count(); ++i) {
        //         arg2 += args.at(i) + " ";
        //     }
        //     args.erase(++args.begin(), args.end());
        //     args << arg2.trimmed();
        //     Q_ASSERT(args.count() == 2);
        // }
        // this->handleArguments(args);
    } else {
        qDebug()<<__FUNCTION__<<"Unknown message type: "<<msg;
    }    
}

void KitRuntimeManager::onOtherInstanceMessageRecived(const QString &msg, int client_id)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<msg;

    QString args;
    int argc;
    char **argv = NULL;
    
    if (msg.startsWith("sayhello:")) {
        // qDebug()<<__FUNCTION__<<"You says: "<<msg;
    } else if (msg.startsWith("cmdline:")) {
        args = msg.right(msg.length() - 8);
        
        unpackArguments(args, argc, &argv);

        this->handleArguments(argc, argv, client_id);

        releaseArguments(argc, argv);

    } else {
        qDebug()<<__FUNCTION__<<"Unknown message type: "<<msg;
    }    
}

