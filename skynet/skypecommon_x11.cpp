/***************************************************************
 * skypeX11.cpp
 * @Author:      Jonathan Verner (jonathan.verner@matfyz.cz)
 * @License:     GPL v2.0 or later
 * @Created:     2008-05-14.
 * @Last Change: 2008-05-14.
 * @Revision:    $Id: skypecommon_x11.cpp 175 2010-10-16 09:02:39Z drswinghead $
 * Description:
 * Usage:
 * TODO:
 * CHANGES:
 ***************************************************************/

#include <sys/stat.h>

#include <QtGui/QApplication>
#include "skypecommon.h"

#ifdef Q_WS_X11
#include <QtGui/QX11Info>

#include <X11/Xatom.h>


static const char *skypemsg = "SKYPECONTROLAPI_MESSAGE";

SkypeCommon::SkypeCommon() { 
    msg = new XMessages( skypemsg, (QWidget *) QApplication::desktop());
    QObject::connect(msg, SIGNAL(gotMessage(int, const QString &)), 
                     this, SLOT(processX11Message(int, const QString &)));
    skype_win = 0;
}

SkypeCommon::~SkypeCommon()
{
    
}

bool SkypeCommon::sendMsgToSkype(const QString &message)
{
    bool ok;
    if ( skype_win > 0 ) {
        ok = msg->sendMessage(skype_win, skypemsg, message);
    }
    return ok;
}

bool SkypeCommon::attachToSkype() {
    Atom skype_inst = XInternAtom(QX11Info::display(), "_SKYPE_INSTANCE", True);
    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret;
    unsigned long bytes_after_ret;
    unsigned char *prop;
    int status;
    QString dbgMsg;

    status = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), skype_inst, 0, 1, False, XA_WINDOW, &type_ret, &format_ret, &nitems_ret, &bytes_after_ret, &prop);

    // sanity check
    if(status != Success || format_ret != 32 || nitems_ret != 1) {
        skype_win = (WId) -1;
        qDebug("skypecommon::connectToInstance(): Skype not detected, status %d\n", status);
        emit this->skypeNotFound();
        return false;
    } else  {
        // skype_win = * (const unsigned long *) prop & 0xffffffff; // for x86
        long fullInt = -1L;
        // skype_win = * (const unsigned long *) prop & 0xffffffffffffffffLL; // test for x64
        skype_win = *(const unsigned long *) prop & fullInt;

        qDebug("skype::connectToInstance(): Skype instance found, window id %d\n", skype_win);
        return true;
    }
}

bool SkypeCommon::detachSkype()
{
    this->skype_win = (WId) -1;
    return true;
}

// put in skype class is nice
bool SkypeCommon::is_skype_running()
{
    char *temp;
    int pid;
    char *stat_path;
    FILE *fh;
    char exec_name[15];
    struct stat statobj;
    
	/* const gchar *temp; */
	/* int pid; */
	/* gchar* stat_path; */
	/* FILE *fh; */
	/* gchar exec_name[15]; */
	/* struct stat *statobj = g_new(struct stat, 1); */
	/* //open /proc */
	/* GDir *procdir = g_dir_open("/proc", 0, NULL); */
	/* //go through directories that are numbers */
	/* while((temp = g_dir_read_name(procdir))) */
	/* { */
	/* 	pid = atoi(temp); */
	/* 	if (!pid) */
	/* 		continue; */
	/* 	// /proc/{pid}/stat contains lots of juicy info */
	/* 	stat_path = g_strdup_printf("/proc/%d/stat", pid); */
	/* 	fh = fopen(stat_path, "r"); */
	/* 	if (!fh) */
	/* 	{ */
	/* 		g_free(stat_path); */
	/* 		continue; */
	/* 	} */
	/* 	pid = fscanf(fh, "%*d (%15[^)]", exec_name); */
	/* 	fclose(fh); */
	/* 	if (!g_str_equal(exec_name, "skype")) */
	/* 	{ */
	/* 		g_free(stat_path); */
	/* 		continue; */
	/* 	} */
	/* 	//get uid/owner of stat file by using fstat() */
	/* 	g_stat(stat_path, statobj); */
	/* 	g_free(stat_path); */
	/* 	//compare uid/owner of stat file (in g_stat->st_uid) to getuid(); */
	/* 	if (statobj->st_uid == getuid()) */
	/* 	{ */
	/* 		//this copy of skype was started by us */
	/* 		g_dir_close(procdir); */
	/* 		g_free(statobj); */
	/* 		return TRUE; */
	/* 	} */
	/* } */
	/* g_dir_close(procdir); */
	/* g_free(statobj); */
    
    return true;
}

void SkypeCommon::processX11Message(int win, const QString &message) {
    if ( win == skype_win ) emit newMsgFromSkype( message );
}


// #include "SkypeCommon.moc"
#endif /* Q_WS_X11 */
