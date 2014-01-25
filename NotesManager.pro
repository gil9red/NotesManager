QT += core gui webkit
TEMPLATE = app
TARGET = NotesManager
DESTDIR += ../bin

TRANSLATIONS += translations/NotesManager_ru.ts \
#                translations/NotesManager_uk.ts \
#                translations/NotesManager_hu.ts \
#                translations/NotesManager_ja.ts \
#                translations/NotesManager_ko.ts \
#                translations/NotesManager_lt.ts \
#                translations/NotesManager_pl.ts \
#                translations/NotesManager_pt.ts \
#                translations/NotesManager_sk.ts \
#                translations/NotesManager_sl.ts \
#                translations/NotesManager_sv.ts \
#                translations/NotesManager_zh_CN.ts \
#                translations/NotesManager_zh_TW.ts \

include( Manager/Manager.pri )
include( Note/Note.pri )
include( FindAndReplace/FindAndReplace.pri )
include( FadLineEdit/FadLineEdit.pri )
include( FormattingToolbar/FormattingToolbar.pri )

include( utils/quazip/quazip.pri )
INCLUDEPATH += utils/quazip
DEPENDPATH += utils/quazip


RC_FILE = app.rc
OTHER_FILES += app.rc \
               history \
               license \

CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/ -lQtSingleApplication
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QtSingleApplication/build/lib -lQtSingleApplication
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QtSingleApplication/build/lib -lQtSingleApplicationd
INCLUDEPATH += $$PWD/QtSingleApplication
DEPENDPATH += $$PWD/QtSingleApplication


SOURCES += \
    main.cpp \
    utils/execmenu.cpp \
    utils/propertyattachable.cpp \
    utils/mysplashscreen.cpp \
#    utils/simplearchiver.cpp

HEADERS += \
    version.h \
    utils/execmenu.h \
    utils/propertyattachable.h \
    utils/mysplashscreen.h \
#    utils/simplearchiver.h \
    utils/func.h

RESOURCES += \
    app.qrc
