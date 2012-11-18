# QT *= core gui

JSONCPP_HEADERS = $$PWD/include/json/autolink.h  $$PWD/include/json/config.h  \
                $$PWD/include/json/features.h  $$PWD/include/json/forwards.h  \
                $$PWD/include/json/json.h  $$PWD/include/json/reader.h   \
                $$PWD/include/json/value.h  $$PWD/include/json/writer.h
JSONCPP_SOURCES = $$PWD/src/lib_json/json_reader.cpp  $$PWD/src/lib_json/json_value.cpp  \
                $$PWD/src/lib_json/json_writer.cpp

INCLUDEPATH += $$PWD/include

HEADERS += $$JSONCPP_HEADERS
SOURCES += $$JSONCPP_SOURCES