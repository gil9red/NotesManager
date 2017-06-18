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
include( NavigationPanel/NavigationPanel.pri )


OTHER_FILES += \
    history_ru \
    license \
    README.md

SOURCES += \
    main.cpp \
    utils/execmenu.cpp \
    utils/propertyattachable.cpp \
    utils/mysplashscreen.cpp \
    utils/texttemplateparser.cpp \
    utils/func.cpp \

HEADERS += \
    utils/execmenu.h \
    utils/propertyattachable.h \
    utils/mysplashscreen.h \
    utils/func.h \
    utils/texttemplateparser.h \
    appinfo.h \

RESOURCES += app.qrc

win32:
{
    RC_FILE = app.rc
    OTHER_FILES += app.rc
}
