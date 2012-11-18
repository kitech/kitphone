#include <QtCore>

#include "utils.h"

Utils::Utils()
{
}
Utils::~Utils()
{
}

QString Utils::get_so_file_name(QString so_mod_name)
{
    QString so_file_name = so_mod_name;

#ifdef Q_OS_WIN
    so_file_name = QString("%1.dll").arg(so_mod_name);
#else
    so_file_name = QString("lib%1.so").arg(so_mod_name);
#endif

    return so_file_name;
}


QString Utils::get_mod_name_from_file_name(QString mod_file_name)
{
    QString mod_name;

#ifdef Q_OS_WIN
    mod_name = mod_file_name.left(mod_file_name.length() - 4);
#else
    mod_name = mod_file_name.left(mod_file_name.length()-3);
    mod_name = mod_name.right(mod_name.length()-3);
#endif

    return mod_name;
}
