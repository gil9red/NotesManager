QT += script
QT += sql
QT += scripttools

HEADERS += \
    ScriptModule/scriptengine.h \
    ScriptModule/scriptsmanager.h

SOURCES += \
    ScriptModule/scriptengine.cpp \
    ScriptModule/scriptsmanager.cpp

FORMS += \
    ScriptModule/scriptsmanager.ui

RESOURCES += \
    ScriptModule/ScriptModule.qrc
