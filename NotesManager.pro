QT += core gui
TEMPLATE = app
TARGET = NotesManager
DESTDIR += ../bin

# Список файлов перевода *.ts
include( translations.pri )

include( Manager/Manager.pri )
include( FancyTabBar/fancytabbar.pri )
include( ImageCropper/imagecropper.pri )
include( Note/Note.pri )
include( FindAndReplace/FindAndReplace.pri )
include( FadLineEdit/FadLineEdit.pri )
include( FormattingToolbar/FormattingToolbar.pri )
include( qtsingleapplication/src/qtsingleapplication.pri )
include( NavigationPanel/NavigationPanel.pri )


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/quazip-0.6.1/build/ -llibquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/quazip-0.6.1/build/ -llibquazipd
else:unix: LIBS += -L$$PWD/quazip-0.6.1/build/ -llibquazip
INCLUDEPATH += $$PWD/quazip-0.6.1/quazip
DEPENDPATH += $$PWD/quazip-0.6.1/quazip


#include( quazip-0.6.1/quazip/quazip.pri )
## Сборка только исходников quazip
#DEFINES += QUAZIP_BUILD


#DEFINES += "BUILDDATE=system(date -R)"

## BUILDTIME and BUILDDATE will be shown in the About dialog
#win32 {
#    DEFINES += BUILDTIME=\\\"$$system('echo %time%')\\\"
#    DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"
#} else {
#    DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M.%s')\\\"
#    DEFINES += BUILDDATE=\\\"$$system(date '+%d/%m/%y')\\\"
#}

## Получение даты сборки в формате DD.MM.YYYY_hh:mm:ss
#win32: {
#        BUILDDATE = $$system( date /t ) $$system( time /t )
#        BUILDDATE = $$member(BUILDDATE,0)_$$member(BUILDDATE,1)
#}
#unix:  BUILDDATE = $$system( date "+%d.%m.%Y_%H:%M:%S" )
#DEFINES += BUILDDATE


RC_FILE = app.rc

OTHER_FILES += \
    app.rc \
    history \
    license

SOURCES += \
    main.cpp \
    utils/execmenu.cpp \
    utils/propertyattachable.cpp \
    utils/mysplashscreen.cpp \
    utils/texttemplateparser.cpp \
    utils/func.cpp

HEADERS += \
    utils/execmenu.h \
    utils/propertyattachable.h \
    utils/mysplashscreen.h \
    utils/func.h \
    utils/texttemplateparser.h \
    appinfo.h

RESOURCES += app.qrc
