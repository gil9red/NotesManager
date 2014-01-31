QT += core gui
TEMPLATE = app
TARGET = NotesManager
DESTDIR += ../bin

TRANSLATIONS += translations/NotesManager_ru.ts \
                translations/NotesManager_uk.ts

include( Manager/Manager.pri )
include( Note/Note.pri )
include( FindAndReplace/FindAndReplace.pri )
include( FadLineEdit/FadLineEdit.pri )
include( FormattingToolbar/FormattingToolbar.pri )
include( quazip/quazip.pri )
include( QtSingleApplication/QtSingleApplication.pri )

INCLUDEPATH += quazip
DEPENDPATH += quazip
INCLUDEPATH += QtSingleApplication
DEPENDPATH += QtSingleApplication


RC_FILE = app.rc
OTHER_FILES += app.rc \
               history \
               license \

SOURCES += \
    main.cpp \
    utils/execmenu.cpp \
    utils/propertyattachable.cpp \
    utils/mysplashscreen.cpp \

HEADERS += \
    version.h \
    utils/execmenu.h \
    utils/propertyattachable.h \
    utils/mysplashscreen.h \
    utils/func.h

RESOURCES += \
    app.qrc
