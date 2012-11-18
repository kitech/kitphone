# skypekit_path.pri --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-12-01 17:12:11 +0800
# Version: $Id: skypekit_path.pri 965 2011-09-01 05:44:21Z drswinghead $
# 

SKBD =       # skypekit base directory
win32 {
    # SKBD = Z:/cross/skypekit/distro-skypekit-cb3.1_3.1.0.2665_124773
    # SKBD = Z:/cross/skypekit/skypekit-sdk_sdk-3.2.1.2_209059
    # SKBD = Z:/cross/skypekit/skypekit-sdk_sdk-3.3.2.31_257682
    # SKBD = Z:/cross/skypekit/skypekit-sdk_sdk-3.4.1.11_342604
    SKBD = Z:/cross/skypekit/skypekit-sdk_sdk-3.5.1.21_402316
} else {
    HOME=$$system(echo $HOME)
    # SKBD = $$HOME/skypekit/distro-skypekit-cb3.1_3.1.0.2665_124773
    # SKBD = $$HOME/skypekit/distro-skypekit-cb3.1_3.1.0.2965_185661
    # SKBD = $$HOME/skypekit/skypekit-sdk_sdk-3.2.1.2_209059
    # SKBD = $$HOME/skypekit/skypekit-sdk_sdk-3.3.2.31_257682
    # SKBD = $$HOME/skypekit/skypekit-sdk_sdk-3.4.1.11_342604
    SKBD = $$HOME/skypekit/skypekit-sdk_sdk-3.5.1.21_402316
}
SKCED = $$SKBD/interfaces/skype/cpp_embedded/src      # cpp embed
message($$SKBD $$SKCED)
INCLUDEPATH += $$SKCED/api $$SKCED/ipc $$SKCED/types $$SKBD/ipc/cpp \
            $$SKBD/ipc/cpp/platform/se \
            $$SKCED/platform/threading \
            $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/include \
            $$SKBD/ipc/cpp/ssl/cyassl/include \
            $$SKBD/ipc/cpp/ssl/cyassl/include/openssl

win32 {
    INCLUDEPATH += $$SKCED/platform/threading/win
} else {
    INCLUDEPATH += $$SKCED/platform/threading/pthread
}
message($$INCLUDEPATH)
