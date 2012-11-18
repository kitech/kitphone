#ifndef _LIBSKYPEAPI_H_
#define _LIBSKYPEAPI_H_

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

    gboolean is_skype_running();
    gboolean exec_skype();
    gboolean skype_connect();
    void skype_disconnect();
    void send_message(const char* message);
    char *skype_send_message(char *message, ...);
    void skype_send_message_nowait(char *message, ...);
    void skype_message_received(char *orig_message);
    // void skype_debug_info(const char *cat, const char *format, ...);
    // void skype_debug_error(const char *cat, const char *format, ...);

#define skype_debug_info(cat, format, args...) fprintf(stderr, cat);fprintf (stderr, format , ##args);
#define skype_debug_error(cat, format, args...) fprintf(stderr, cat);fprintf (stderr, format , ##args);


    //////
    const char * skype_get_account_username();
    const char *skype_get_account_alias();
    const char * skype_callto(const char *who);

#ifdef __cplusplus
}
#endif

#endif /* _LIBSKYPEAPI_H_ */
