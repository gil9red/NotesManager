TEMPLATE = lib
CONFIG += build_all
TARGET = $$qtLibraryTarget(QuaZIP)
DESTDIR = lib
DEFINES += QUAZIP_BUILD

include(src/quazip.pri)
