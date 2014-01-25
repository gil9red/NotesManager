QT += core gui

HEADERS += \
    Note/texteditor.h \
    Note/abstractnote.h \
    Note/dictionary.h \
    Note/completer.h \
    Note/AttachPanel.h \
    Note/RichTextNote.h \
    Note/abstractnotehead.h \
    Note/abstractnotebody.h \
    Note/NoteShared.h

SOURCES += \
    Note/texteditor.cpp \
    Note/abstractnote.cpp \
    Note/dictionary.cpp \
    Note/completer.cpp \
    Note/AttachPanel.cpp \
    Note/RichTextNote.cpp \
    Note/abstractnotehead.cpp \
    Note/abstractnotebody.cpp \

RESOURCES += \
    Note/Note.qrc \

FORMS += \
    Note/AttachPanel.ui \
