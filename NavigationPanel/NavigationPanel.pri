QT += xml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    NavigationPanel/abstractfolderitem.h \
    NavigationPanel/basemodel.h \
    NavigationPanel/basemodelitem.h \
    NavigationPanel/folder.h \
    NavigationPanel/folderitemcollection.h \
    NavigationPanel/foldermodelitem.h \
    NavigationPanel/foldernavigationwidget.h \
    NavigationPanel/hierarchymodel.h \
    NavigationPanel/modelitemdelegate.h \
    NavigationPanel/note.h \
    NavigationPanel/notemodelitem.h \
    NavigationPanel/notetagscollection.h \
    NavigationPanel/separatormodelitem.h \
    NavigationPanel/notebook.h \
    NavigationPanel/tag.h \
    NavigationPanel/tagownerscollection.h \
    NavigationPanel/datesmodel.h \
    NavigationPanel/datemodelitem.h \
    NavigationPanel/datenavigationwidget.h \
    NavigationPanel/tagmodelitem.h \
    NavigationPanel/tagslineedit.h \
    NavigationPanel/tagsmodel.h \
    NavigationPanel/tagsnavigationwidget.h \
    NavigationPanel/notestabwidget.h \
    NavigationPanel/noteeditwidget.h \

SOURCES += \
    NavigationPanel/abstractfolderitem.cpp \
    NavigationPanel/basemodel.cpp \
    NavigationPanel/basemodelitem.cpp \
    NavigationPanel/folder.cpp \
    NavigationPanel/folderitemcollection.cpp \
    NavigationPanel/foldermodelitem.cpp \
    NavigationPanel/foldernavigationwidget.cpp \
    NavigationPanel/hierarchymodel.cpp \
    NavigationPanel/modelitemdelegate.cpp \
    NavigationPanel/note.cpp \
    NavigationPanel/notemodelitem.cpp \
    NavigationPanel/notetagscollection.cpp \
    NavigationPanel/separatormodelitem.cpp \
    NavigationPanel/notebook.cpp \
    NavigationPanel/tag.cpp \
    NavigationPanel/tagownerscollection.cpp \
    NavigationPanel/datemodelitem.cpp \
    NavigationPanel/datenavigationwidget.cpp \
    NavigationPanel/datesmodel.cpp \
    NavigationPanel/tagmodelitem.cpp \
    NavigationPanel/tagslineedit.cpp \
    NavigationPanel/tagsmodel.cpp \
    NavigationPanel/tagsnavigationwidget.cpp \
    NavigationPanel/notestabwidget.cpp \
    NavigationPanel/noteeditwidget.cpp \

RESOURCES += \
    NavigationPanel/NavigationPanel.qrc

FORMS += \
    NavigationPanel/noteeditwidget.ui \
    NavigationPanel/foldernavigationwidget.ui \
    NavigationPanel/tagsnavigationwidget.ui \
    NavigationPanel/datenavigationwidget.ui
