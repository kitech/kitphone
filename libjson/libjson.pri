# QT *= core gui

LIBJSON_HEADERS = 
LIBJSON_SOURCES = $$PWD/Source/JSONMemory.cpp    \
                $$PWD/Source/JSONWorker.cpp    \
                $$PWD/Source/JSONIterators.cpp    \
                $$PWD/Source/JSONAllocator.cpp    \
                $$PWD/Source/JSONNode.cpp    \
                $$PWD/Source/JSONChildren.cpp    \
                $$PWD/Source/libjson.cpp    \
                $$PWD/Source/JSONNode_Mutex.cpp    \
                $$PWD/Source/JSONWriter.cpp    \
                $$PWD/Source/JSONPreparse.cpp    \
                $$PWD/Source/internalJSONNode.cpp    \
                $$PWD/Source/JSON_Base64.cpp    \
                $$PWD/Source/JSONValidator.cpp    \
                $$PWD/Source/JSONDebug.cpp    \
                $$PWD/Source/JSONStream.cpp

# INCLUDEPATH += $$PWD/include
DEFINES += NDEBUG

HEADERS += $$LIBJSON_HEADERS
SOURCES += $$LIBJSON_SOURCES