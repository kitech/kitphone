
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "libskypeapi.h"

const char *skype_get_account_username()
{
	char *ret;
	static char *username = NULL;
	
	if (username != NULL)
		return username;
	
	ret = skype_send_message("GET CURRENTUSERHANDLE");
	if (!ret || !strlen(ret))
	{
		g_free(ret);
		return NULL;
	}
	username = g_strdup(&ret[18]);
	g_free(ret);

	return username;
}

const char *skype_get_account_alias()
{
	char *ret;
	char *alias;

    static char *display_name = NULL;
    
    if (display_name != NULL) {
        return display_name;
    }

	ret = skype_send_message("GET PROFILE FULLNAME");
	display_name = alias = g_strdup(&ret[17]);
	g_free(ret);
	// purple_account_set_alias(acct, alias);
	// g_free(alias);
    return display_name;
}

const char * skype_callto(const char *who)
{
	gchar *temp;
	gchar *callnumber_string;
	
	temp = skype_send_message("CALL %s", who);
	if (!temp || !strlen(temp))
	{
		g_free(temp);
		return false;
	}
	callnumber_string = g_new(gchar, 10+1);
	sscanf(temp, "CALL %s ", callnumber_string);
	
	return callnumber_string;
}
