/* utils.h --- 
 * 
 * Author: liuguangzhao
 * Copyright (C) 2007-2010 liuguangzhao@users.sf.net
 * URL: http://www.qtchina.net http://nullget.sourceforge.net
 * Created: 2008-06-01 09:58:33 +0800
 * Version: $Id: utils.h 722 2010-12-07 09:08:24Z liuguangzhao $
 */

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <map>
#include <string>
#include <assert.h>

/*******************/
/*
  ls: level string
  fn: file name
  ln: line no
  yorn: ???
  
 */
#define log_printf(ls, fn, ln, yorn, ... ) do {                         \
        char log[1000] = {0};                                           \
        char time_str[32] = {0};                                        \
        time_t time_ival = time(NULL);                                  \
        ctime_r(&time_ival, time_str);                                  \
        time_str[strlen(time_str)-1] = '\0';                            \
        snprintf(log, sizeof(log)-1, __VA_ARGS__ );                       \
        fn != NULL ? fprintf(stdout, "[%s] %s[] %s:%d %s: %s\n", time_str, ls, fn, ln, __FUNCTION__, log) : fprintf(stdout, "[%s] %s[] %s\n %s\n", time_str, ls, __FUNCTION__, log); \
        yorn == 'y' ? (1==1) : (1==1) ;                                 \
    } while(0);
 
#define log_error( ... ) log_printf("ERROR", __FILE__, __LINE__, 'n', __VA_ARGS__ )
#ifndef NDEBUG
#define log_debug( ... ) log_printf("DEBUG", __FILE__, __LINE__, 'n', __VA_ARGS__ )
#else
#define log_debug( ... ) do {} while(0);
#endif
#define log_fetal( ... ) log_printf("FETAL", __FILE__, __LINE__, 'y', __VA_ARGS__ )
// #define qlog( ... ) log_printf("Info:", (char*)0, 0, 'n', __VA_ARGS__ )
//#define qlog( ... ) log_printf("INFO", __FILE__, __LINE__, 'y', __VA_ARGS__ );
#define qlog( ... ) 

/************* log end ********/

#ifdef __cplusplush
};
#endif

#endif
