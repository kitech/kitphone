// simplelog.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-01 15:45:06 +0800
// Version: $Id$
// 


#ifndef _SIMPLE_LOG_H_
#define _SIMPLE_LOG_H_

#include <QtCore>

#include "boost/smart_ptr.hpp"

// 好象这个log功能还有问题，非qt的多线程有时会崩溃。
class FileLog // : public QObject
{
//    Q_OBJECT;
public:
    virtual ~FileLog();
    static boost::shared_ptr<FileLog> instance();
    QFile *stream();

protected:
    explicit FileLog();

private:
    static boost::shared_ptr<FileLog> mInst;
    QFile* mStream;
};

class XQDebug : public QDebug
{
public:
    XQDebug(QIODevice *device) : QDebug(device) {
    }

    ~XQDebug() {
        #ifdef WIN32
        *this<<"\r\n";
        #else
        *this<<"\n";
        #endif
    }
};

// 很不错
#ifdef WIN32
#include <windows.h>
#define qLogx() XQDebug(FileLog::instance()->stream())<<"["<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")<<"]"<<__FILE__<<__LINE__<<__FUNCTION__<<QString("T%1").arg(GetCurrentThreadId())
#else
#include <syscall.h>
#define qLogx() XQDebug(FileLog::instance()->stream())<<"["<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")<<"]"<<__FILE__<<__LINE__<<__FUNCTION__<<QString("T%1").arg(syscall(__NR_gettid))
#endif


// 代码段执行计时
static int __rtc_seq = 0;
static QHash<int,QDateTime> __rtc_hash;
inline int TIMER_BEGIN() {
    QDateTime t = QDateTime::currentDateTime();
    int nseq = ++__rtc_seq;
    __rtc_hash.insert(nseq, t);
    return nseq;
}

inline void TIMER_END(int seq) {
    QDateTime e = QDateTime::currentDateTime();
    QDateTime t = __rtc_hash.value(seq);

    qLogx()<<"UT: from ["<<t<<"] to ["<<e<<", eclapse:"<<(e.msecsTo(t));
}

#endif /* _LOG_H_ */








