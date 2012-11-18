// main.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-03 15:24:47 +0800
// Version: $Id$
// 

#include <sys/time.h>
#include <signal.h>

#include <QtGui/QApplication>
#include "skyserv.h"
#include "skyservapplication.h"

static int setup_unix_signal_handlers()
{
    struct sigaction hup, term, sint, quit;

    // hup.sa_handler = MyDaemon::hupSignalHandler;
    hup.sa_handler = SkyServ::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0) > 0)
        return 1;

    // term.sa_handler = MyDaemon::termSignalHandler;
    term.sa_handler = SkyServ::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
        return 2;

    // sint.sa_handler = SkyServ::intSignalHandler;
    // sigemptyset(&sint.sa_mask);
    // sint.sa_flags |= SA_RESTART;

    // if (sigaction(SIGINT, &sint, 0) > 0) {
    //     return 3;
    // }

    quit.sa_handler = SkyServ::quitSignalHandler;
    sigemptyset(&quit.sa_mask);
    quit.sa_flags |= SA_RESTART;

    if (sigaction(SIGQUIT, &quit, 0) > 0) {
        return 4;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    srand(tv.tv_sec + tv.tv_usec);

    if (setup_unix_signal_handlers() != 0) {
        fprintf(stderr, "install unix handler faild.\n");
        exit(3);
    }
    
    SkyServApplication a(argc, argv);
    SkyServ w;
    // QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(on_app_want_quit()));
    // w.show();
    return a.exec();
}
