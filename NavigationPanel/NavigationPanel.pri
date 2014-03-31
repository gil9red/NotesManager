QT += xml

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

HEADERS += \
    NavigationPanel/src/abstractfolderitem.h \
    NavigationPanel/src/basemodel.h \
    NavigationPanel/src/basemodelitem.h \
    NavigationPanel/src/folder.h \
    NavigationPanel/src/folderitemcollection.h \
    NavigationPanel/src/foldermodelitem.h \
    NavigationPanel/src/foldernavigationwidget.h \
    NavigationPanel/src/hierarchymodel.h \
    NavigationPanel/src/modelitemdelegate.h \
    NavigationPanel/src/note.h \
    NavigationPanel/src/notemodelitem.h \
    NavigationPanel/src/notetagscollection.h \
    NavigationPanel/src/separatormodelitem.h \
    NavigationPanel/src/notebook.h \
    NavigationPanel/src/tag.h \
    NavigationPanel/src/tagownerscollection.h \
    NavigationPanel/src/datesmodel.h \
    NavigationPanel/src/datemodelitem.h \
    NavigationPanel/src/datenavigationwidget.h \
    NavigationPanel/src/tagmodelitem.h \
    NavigationPanel/src/tagslineedit.h \
    NavigationPanel/src/tagsmodel.h \
    NavigationPanel/src/tagsnavigationwidget.h \
    NavigationPanel/src/notestabwidget.h \
    NavigationPanel/src/noteeditwidget.h

SOURCES += \
    NavigationPanel/src/abstractfolderitem.cpp \
    NavigationPanel/src/basemodel.cpp \
    NavigationPanel/src/basemodelitem.cpp \
    NavigationPanel/src/folder.cpp \
    NavigationPanel/src/folderitemcollection.cpp \
    NavigationPanel/src/foldermodelitem.cpp \
    NavigationPanel/src/foldernavigationwidget.cpp \
    NavigationPanel/src/hierarchymodel.cpp \
    NavigationPanel/src/modelitemdelegate.cpp \
    NavigationPanel/src/note.cpp \
    NavigationPanel/src/notemodelitem.cpp \
    NavigationPanel/src/notetagscollection.cpp \
    NavigationPanel/src/separatormodelitem.cpp \
    NavigationPanel/src/notebook.cpp \
    NavigationPanel/src/tag.cpp \
    NavigationPanel/src/tagownerscollection.cpp \
    NavigationPanel/src/datemodelitem.cpp \
    NavigationPanel/src/datenavigationwidget.cpp \
    NavigationPanel/src/datesmodel.cpp \
    NavigationPanel/src/tagmodelitem.cpp \
    NavigationPanel/src/tagslineedit.cpp \
    NavigationPanel/src/tagsmodel.cpp \
    NavigationPanel/src/tagsnavigationwidget.cpp \
    NavigationPanel/src/notestabwidget.cpp \
    NavigationPanel/src/noteeditwidget.cpp

RESOURCES += \
    $$PWD/NavigationPanel.qrc

FORMS += \
    NavigationPanel/src/noteeditwidget.ui
