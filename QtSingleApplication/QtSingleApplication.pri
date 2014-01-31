QT += network

HEADERS += \
    qtsingleapplication.h \
    QtSingleApplication \
    qtlockedfile.h \
    QtLockedFile \
    qtlocalpeer.h

SOURCES += \
    qtsingleapplication.cpp \
    qtlockedfile_win.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile.cpp \
    qtlocalpeer.cpp

