INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/quickfind.h \
    $$PWD/findfadlineedit.h \
    $$PWD/findandreplace.h \
    $$PWD/find_highlighter.h \
    $$PWD/findshared.h

SOURCES += \
    $$PWD/quickfind.cpp \
    $$PWD/findfadlineedit.cpp \
    $$PWD/findandreplace.cpp \
    $$PWD/find_highlighter.cpp \
    FindAndReplace/findshared.cpp

FORMS += \
    $$PWD/findandreplace.ui

RESOURCES += \
    $$PWD/findandreplace.qrc
