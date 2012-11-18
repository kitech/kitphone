#ifndef _LISP_BRIDGE_H_
#define _LISP_BRIDGE_H_

#include <QtCore>

class NLBridge : public QObject
{
    Q_OBJECT;
public:
    NLBridge(QObject *parent = 0);
    virtual ~NLBridge();

    bool initEnv();
    bool load();
};

#endif /* _LISP_BRIDGE_H_ */
