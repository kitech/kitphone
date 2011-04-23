
include(../skykit/qmake/skypekit_path.pri)

SKCD=$$SKCED/client

SKYKITCLIENT_HEADERS = 
SKYKITCLIENT_SOURCES = $$SKCD/caccount.cpp     \
$$SKCD/ccontact.cpp     \
$$SKCD/ccontactgroup.cpp     \
$$SKCD/ccontactsearch.cpp     \
$$SKCD/cconversation.cpp     \
$$SKCD/cmessage.cpp     \
$$SKCD/cparticipant.cpp     \
$$SKCD/cskype.cpp     \
$$SKCD/csms.cpp     \
$$SKCD/ctransfer.cpp     \
$$SKCD/cvideo.cpp     \
$$SKCD/cvideowindow.cpp     \
$$SKCD/cvoicemail.cpp     \
$$SKCD/glwindow.cpp     \
$$SKCD/helper-functions.cpp     \
$$SKCD/iosurfacetransport.cpp     \
#     $$SKCD/main.cpp     \
$$SKCD/posixshmtransport.cpp     \
$$SKCD/sysvshmtransport.cpp     \
$$SKCD/x11window.cpp

HEADERS += $$SKYKITCLIENT_HEADERS
SOURCES += $$SKYKITCLIENT_SOURCES