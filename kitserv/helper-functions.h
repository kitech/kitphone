
#ifndef _HELPERFUNCTIONS_H
#define _HELPERFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#define	KB_BUF_SIZE 4096

#include "SidPlatform.hpp"

Sid::String ask(Sid::String what);
int ask_list(Sid::String what, const char* possibilities[]);
int ask_yesno(Sid::String what);
int ask_int(Sid::String what);
bool load_file(Sid::String filename, Sid::Binary&, const char* mode = "r"); // return false if failed to load

#endif // _HELPERFUNCTIONS_H

