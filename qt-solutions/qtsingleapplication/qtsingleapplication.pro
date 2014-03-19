TEMPLATE = lib
CONFIG += build_all
TARGET = $$qtLibraryTarget(QtSingleApplication)
DESTDIR = lib
DEFINES += QT_QTSINGLEAPPLICATION_EXPORT

include(src/qtsingleapplication.pri)
