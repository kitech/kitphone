#ifndef _LIMIT_DETECT_H_
#define _LIMIT_DETECT_H_

#include <QtCore>

class StateElem
{
public:
    StateElem(){};
    ~StateElem(){}

    int otype; // object, like USER, CALL, etc
    int prop;   // LIKE CONNECTSTATUS, etc
    int ovalue; // ONLINE, OFFLINE, etc
    QDateTime otime;
    QString handler_name;
    QString status_line;
};
class LimitDetector : public QThread
{
    Q_OBJECT;
public:
    LimitDetector(QObject *parent = 0);
    virtual ~LimitDetector();
    
    virtual void run();
    bool reset();
    bool on_skype_status(QString handler_name, QString status_line);
    bool calc_exception();
public slots:
    void on_schedule_timeout();

signals:
    void exception_detected(int reason);

private:
    QDateTime start_time;
    QDateTime last_live_time;
    QVector<StateElem> conn_states;
    // 转为碰到一个bug #0000475, 把元素值从QVairnat转到QString
    QVector<QVector<QString> > news; // inner: handler_name, status_line, date
    QTimer *sched_timer;
};

#endif /* _LIMIT_DETECT_H_ */
