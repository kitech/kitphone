
#ifndef _CLIENT_MAIN_H
#define _CLIENT_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "skype-object.h"
#include "helper-functions.h"

// #include "cskype.h"

#ifdef SKYPEKIT_SURFACE_RENDERING
#if defined(GL_WINDOW)
class glWindow;
extern glWindow *preview_window;
extern glWindow *receive_window;
#elif defined(X11_WINDOW)
class X11Window;
extern X11Window *preview_window;
extern X11Window *receive_window;
#else
#error "Defined SKYPEKIT_SURFACE_RENDERING but no XX_WINDOW defined"
#endif
#endif

int skype_helper_invoke_skype_connected(SERootObject *root);
int skype_helper_invoke_skype_disconnected(SERootObject *root);
int skype_helper_invoke_skype_onconversation_list_change(SERootObject *root, const SEReference& conversation, const int type, const bool added);
int skype_helper_invoke_skype_onmessage(SERootObject *root, const SEReference & message, const bool& changesInboxTimestamp, const SEReference& supersedesHistoryMessage, const SEReference& conversation);
int skype_helper_invoke_account_onchange(SERootObject *root, SEReference &account, int prop);
int skype_helper_invoke_conversation_onchange(SERootObject *root, SEReference &conversation, int prop);
int skype_helper_invoke_participant_onchange(SERootObject *root, SEReference &participant, int prop);
int skype_helper_invoke_participant_onincoming_dtmf(SERootObject *root, SEReference &participant, int dtmf);

#endif // _CLIENT_MAIN_H





