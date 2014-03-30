INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/Manager.cpp \
    $$PWD/pages/page_settings.cpp \
    $$PWD/pages/page_notes.cpp \
    $$PWD/pages/page_about.cpp

FORMS += \
    $$PWD/Manager.ui \
    $$PWD/pages/page_settings.ui \
    $$PWD/pages/page_notes.ui \
    $$PWD/pages/page_about.ui \

RESOURCES += \
    $$PWD/Manager.qrc

HEADERS += \
    $$PWD/pages/page_settings.h \
    $$PWD/pages/page_notes.h \
    $$PWD/pages/page_about.h \
    $$PWD/Manager.h
