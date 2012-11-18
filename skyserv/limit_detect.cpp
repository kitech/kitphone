
#include "../utils.h"

#include "skypeobject.h"

#include "limit_detect.h"

/*
  检测skype限制机制：
  5分钟内没有收到任何skype api消息，
  或，20分钟内掉线次数超过5次
 */
LimitDetector::LimitDetector(QObject *parent)
    : QThread(parent)
{
    this->sched_timer = new QTimer();
    this->sched_timer->setInterval(1000 * 10);
    QObject::connect(this->sched_timer, SIGNAL(timeout()),
                     this, SLOT(on_schedule_timeout()));
    this->start_time = QDateTime::currentDateTime();
}

LimitDetector::~LimitDetector()
{
    delete this->sched_timer;
}

// 这个类中已经数据已经算是多线程处理了，需要加锁定相关的同步处理。
// ??? 这个线程的退出怎么会影响到另外一个线程呢？？？
// sip_main_proc: epoll_pwait: 4, 被中断的系统调用
void LimitDetector::run()
{
    QVector<QString> msg;
    int has_new = false;
    QStringList fields;
    QString tmp_line;
    QDateTime tmp_time;

    has_new = this->news.count();
    while(this->news.count() > 0) {
        msg = this->news.first();
        this->news.remove(0, 1);

        StateElem elm;
        elm.handler_name = msg.at(0).trimmed();
        elm.status_line = msg.at(1).trimmed();
        elm.otime = QDateTime::fromString(msg.at(2), "yyyy-MM-dd hh:mm:ss.zzz");

        // "#754 CONNSTATUS OFFLINE"
        // 两种情况都有，
        // "USERSTATUS ONLINE"
        tmp_line = elm.status_line;
        if (tmp_line.startsWith("#")) {
            QRegExp exp;
            QStringList list;
            exp.setPattern("^#([^ ]*)[ ](.*$");
            if (exp.exactMatch(tmp_line)) {
                list = exp.capturedTexts();
                tmp_line = list[2];
            } 
        }
        fields = tmp_line.split(" ");
        if (fields.at(0) == "CONNSTATUS") {
            elm.otype = SO_CONNECTION;
            if (fields.at(1) == "ONLINE") {
                elm.ovalue = OS_ONLINE;
            } else if (fields.at(1) == "OFFLINE") {
                elm.ovalue = OS_OFFLINE;
            }

            this->conn_states.append(elm);
        }
    }

    if (this->last_live_time.secsTo(QDateTime::currentDateTime()) > 5 * 60) {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"by time, emit exception.";
        // qlog("%s", "by time, emit exception.");
        emit this->exception_detected(0);
    } else {
        if (has_new > 0) {
            if (this->calc_exception() == true) {
                qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"by status, emit exception.";
                // qlog("%s", "by time, emit exception.");
                emit this->exception_detected(0);
            }
        } else {
        
        }
    }
}

void LimitDetector::on_schedule_timeout()
{
    if (!this->isRunning()) {
        this->start();
    }
}

bool LimitDetector::reset()
{
    this->sched_timer->stop();
    this->news.clear();
    this->conn_states.clear();
    this->sched_timer->start();
    return true;
}

bool LimitDetector::on_skype_status(QString handler_name, QString status_line)
{
    // this->news.append(QPair<QString,QString>(handler_name, status_line));
    QVector<QString> msg;
    msg.append(handler_name);
    msg.append(status_line);
    msg.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));

    this->news.append(msg);

    if (!this->sched_timer->isActive()) {
        this->sched_timer->start();
    }

    this->last_live_time = QDateTime::currentDateTime();

    return true;
}

bool LimitDetector::calc_exception()
{
    QVector<QVector<StateElem>::iterator> its;
    QVector<StateElem>::iterator it;
    int offline_count = 0;
    int elem_count = 0;
    int time_intval = 20 * 60; // 20 分钟
    int dep_intval = 30 * 60;
    QDateTime min_time;
    QDateTime max_time;
    QDateTime now_date = QDateTime::currentDateTime();
    StateElem elm;

    min_time = now_date;
    max_time = this->start_time;

    for (it = this->conn_states.begin(); it != this->conn_states.end(); it++) {
        elm = *it;
        if (elm.otime.secsTo(now_date) > dep_intval) {
            its.append(it);
            continue;
        }

        elem_count ++;
        if (elm.otime < min_time) {
            min_time = elm.otime;
        }
        if (elm.otime > max_time) {
            max_time = elm.otime;
        }

        if (elm.otype == SO_CONNECTION) {
            if (elm.ovalue == OS_OFFLINE) {
                offline_count ++;
            }
        }
    }

    for (int i = its.count()-1; i >= 0; i--) {
        this->conn_states.erase(its.at(i));
    }

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__
            <<"min_time:"<<min_time
            <<"max_time:"<<max_time
            <<"secs:"<<min_time.secsTo(max_time)
            <<"elems:"<<elem_count
            <<"offline:"<<offline_count
            <<"rate:"<<((elem_count!=0)?(offline_count*1.0/elem_count):8888);

    // qlog("min_time:%s,max_time:%s,secs:%d,elems:%d,offline:%d, rate:%f\n",
    //      min_time.toString().toAscii().data(), max_time.toString().toAscii().data(),
    //      min_time.secsTo(max_time), elem_count, offline_count, 
    //      ((elem_count!=0)?(offline_count*1.0/elem_count):8888));
    
    if (offline_count > 5) {
        // qlog("%s", 
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"I think there is exception case";
        return true;
    }

    return false;
}
