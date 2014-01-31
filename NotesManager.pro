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
include( quazip-0.6.1/quazip/quazip.pri )
include( qtsingleapplication/src/qtsingleapplication.pri )

# Сборка только исходников quazip
DEFINES += QUAZIP_BUILD


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
    utils/texttemplateparser.cpp

HEADERS += \
    version.h \
    utils/execmenu.h \
    utils/propertyattachable.h \
    utils/mysplashscreen.h \
    utils/func.h \
    utils/texttemplateparser.h

RESOURCES += \
    app.qrc
