# cppwrapper.pro --- 
# 
# Author: liuguangzhao
# Copyright (C) 2007-2010 liuguangzhao@users.sf.net
# URL: 
# Created: 2010-12-01 17:12:26 +0800
# Version: $Id: cppwrapper.pro 598 2010-12-01 10:27:00Z drswinghead $
# 

TEMPLATE = lib
QT -= core gui
CONFIG += staticlib console
TARGET = skypekit-cyassl_lib

SOURCES += 
HEADERS += 

include(../skypekit_path.pri)
win32 {
} else {
    QMAKE_CXXFLAGS += -g
    QMAKE_CXXFLAGS += -fno-rtti   # must used by skypekit lib, -fno-exceptions
}
DEFINES += OPENSSL_EXTRA # for openssl api wraypper in cyassl
QMAKE_CXXFLAGS += -MMD -MP -DNO_RABBIT -DNO_HC128 -DNO_DES -DNO_DSA -DNO_MD4 -DNO_FILESYSTEM -DHAVE_CONFIG_H
QMAKE_CFLAGS = $$QMAKE_CXXFLAGS

SOURCES += $$SKBD/ipc/cpp/ssl/cyassl/src/cyassl_io.c \
        $$SKBD/ipc/cpp/ssl/cyassl/src/ssl.c \
        $$SKBD/ipc/cpp/ssl/cyassl/src/tls.c \
        $$SKBD/ipc/cpp/ssl/cyassl/src/cyassl_int.c \
        $$SKBD/ipc/cpp/ssl/cyassl/src/keys.c \
        $$SKBD/ipc/cpp/ssl/cyassl/src/sniffer.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/des3.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/dh.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/sha512.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/hmac.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/sha.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/_md4.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/_md5.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/hc128.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/tfm.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/pwdbased.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/_sha256.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/rabbit.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/coding.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/dsa.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/aes.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/asm.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/random.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/ripemd.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/asn.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/rsa.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/arc4.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/misc.c \
        $$SKBD/ipc/cpp/ssl/cyassl/ctaocrypt/src/integer.c


        