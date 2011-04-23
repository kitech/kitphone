
#include "configs.h"

#include "newlisp_api.h"

#include "lisp_bridge.h"

/////////
NLBridge::NLBridge(QObject *parent)
    : QObject(parent)
{
}
NLBridge::~NLBridge()
{
}

bool NLBridge::initEnv()
{
    newlispEvalStr("(+ 1 1)");
    return true;
}

bool NLBridge::load()
{
    QString conf_path = QFileInfo(Configs()._conf_file_path()).absolutePath();
    // QString test_path = conf_path + "/test.lisp";
    QString test_path = conf_path + "/init.lisp";

    char evbuf[1000] = {0};
    snprintf(evbuf, sizeof(evbuf)-1, "(load \"%s\")", test_path.toAscii().data());
    char *pret = newlispEvalStr(evbuf);

    qDebug()<<"run :"<<evbuf<<" -->"<<pret;

    return true;
}

