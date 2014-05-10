INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/texteditor.h \
    $$PWD/abstractnote.h \
    $$PWD/dictionary.h \
    $$PWD/completer.h \
    $$PWD/AttachPanel.h \
    $$PWD/RichTextNote.h \
    $$PWD/abstractnotehead.h \
    $$PWD/abstractnotebody.h \
    $$PWD/NoteShared.h \
    Note/settingsnotedialog.h

SOURCES += \
    $$PWD/texteditor.cpp \
    $$PWD/abstractnote.cpp \
    $$PWD/dictionary.cpp \
    $$PWD/completer.cpp \
    $$PWD/AttachPanel.cpp \
    $$PWD/RichTextNote.cpp \
    $$PWD/abstractnotehead.cpp \
    $$PWD/abstractnotebody.cpp \
    Note/settingsnotedialog.cpp

RESOURCES += \
    $$PWD/Note.qrc \

FORMS += \
    $$PWD/AttachPanel.ui \
    Note/settingsnotedialog.ui
