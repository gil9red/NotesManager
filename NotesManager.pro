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
include( ScriptModule/ScriptModule.pri )


INCLUDEPATH += $$PWD/qt-solutions/qtsingleapplication/src
DEPENDPATH += $$PWD/qt-solutions/qtsingleapplication/src
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/qt-solutions/qtsingleapplication/lib/ -llibQtSingleApplication
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qt-solutions/qtsingleapplication/lib/ -llibQtSingleApplicationd

INCLUDEPATH += $$PWD/quazip-0.6.1/src
DEPENDPATH += $$PWD/quazip-0.6.1/src
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/quazip-0.6.1/lib/ -llibQuaZIP
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/quazip-0.6.1/lib/ -llibQuaZIPd

INCLUDEPATH += $$PWD/QScintilla-gpl-2.8.1/Qt4Qt5
DEPENDPATH += $$PWD/QScintilla-gpl-2.8.1/Qt4Qt5
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QScintilla-gpl-2.8.1/lib/ -llibqscintilla2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QScintilla-gpl-2.8.1/lib/ -llibqscintilla2d


OTHER_FILES += \
    history_ru \
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

win32:
{
#    RC_FILE = app.rc
    OTHER_FILES += app.rc
}
