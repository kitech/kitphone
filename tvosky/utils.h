#ifndef UTILS_H
#define UTILS_H

class QString;

#define q_debug()  qDebug()<<"DEBUG: "<<__FUNCTION__<<" at "<<__FILE__<<" on "<<__LINE__<<"\n    "

#define q_warning()  qWarning()<<"DEBUG: "<<__FUNCTION__<<" at "<<__FILE__<<" on "<<__LINE__<<"\n    "

#define q_cretical()  qCretical()<<"DEBUG: "<<__FUNCTION__<<" at "<<__FILE__<<" on "<<__LINE__<<"\n    "

#define q_fetal()  qFetal()<<"DEBUG: "<<__FUNCTION__<<" at "<<__FILE__<<" on "<<__LINE__<<"\n    "

#ifdef __cplusplus
extern "C"{
#endif



#ifdef __cplusplus
};
#endif



class Utils
{
public:
    Utils();
    ~Utils();

    static QString get_so_file_name(QString so_mod_name);
    static QString get_mod_name_from_file_name(QString mod_file_name);
};
#endif

