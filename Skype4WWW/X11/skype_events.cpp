//

/*
	This function must only be called from the main loop, using purple_timeout_add
*/

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "libskypeapi.h"
#include "Skype4WWWAPI.h"

extern Skype4WWWAPI *root_jsapi_skw;

int skype_handle_received_message(char *message)
{
    std::string call_id;
    fprintf(stderr, "in handle received message: %s\n", message);

    if (0) {
        call_id = std::string("1234567890");
        root_jsapi_skw->hangup_call(call_id);
    }

    char command[255];
    char **string_parts = NULL;
	const char *my_username;
	char *type;
	char *temp;

	sscanf(message, "%s ", command);
	my_username = skype_get_account_username();
	string_parts = g_strsplit(message, " ", 4);

    if (g_str_equal(command, "CALL")) {
		if (g_str_equal(string_parts[2], "STATUS")) {

            root_jsapi_skw->call_status_changed(std::string(string_parts[1]), string_parts[3]);

			if (g_str_equal(string_parts[3], "RINGING")) {
				temp = skype_send_message("GET CALL %s TYPE", string_parts[1]);
				type = g_new0(gchar, 9);
				sscanf(temp, "CALL %*s TYPE %[^_]", type);
				g_free(temp);
				if (g_str_equal(type, "INCOMING")) {
					// skype_handle_incoming_call(gc, string_parts[1]);
                }
			} else if (g_str_equal(string_parts[3], "FINISHED") ||
                       g_str_equal(string_parts[3], "CANCELLED") ||
                       g_str_equal(string_parts[3], "REFUSED") ||
                       g_str_equal(string_parts[3], "MISSED") ||
                       g_str_equal(string_parts[3], "FAILED")) {
				// skype_handle_call_got_ended(string_parts[1]);
                root_jsapi_skw->hangup_call(std::string(string_parts[1]));
			}
		}
    }

    if (g_str_equal(command, "CONNSTATUS")) {
        root_jsapi_skw->connection_status_changed(std::string(string_parts[1]));
    }

	if (string_parts) {
		g_strfreev(string_parts);
	}
	g_free(message);

    return true;
}
