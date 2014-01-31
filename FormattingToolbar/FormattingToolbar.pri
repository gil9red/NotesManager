INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/FormattingToolbar.h \
    $$PWD/DialogInsertHyperlink.h \
    $$PWD/ColorButton.h \
    $$PWD/ButtonUnderlineStyleList.h \
    $$PWD/ButtonSelectionTableSizes.h \
    $$PWD/ButtonOrderedList.h \
    $$PWD/ButtonList.h \
    $$PWD/ButtonBulletedList.h \
    $$PWD/WidgetSelectionTableSizes.h \
    $$PWD/colorbackgroundbutton.h \
    $$PWD/colorpalette.h \
    $$PWD/colortextbutton.h \
    $$PWD/colorselectbutton.h

SOURCES += \
    $$PWD/FormattingToolbar.cpp \
    $$PWD/DialogInsertHyperlink.cpp \
    $$PWD/ColorButton.cpp \
    $$PWD/ButtonSelectionTableSizes.cpp \
    $$PWD/ButtonList.cpp \
    $$PWD/WidgetSelectionTableSizes.cpp \
    $$PWD/colorbackgroundbutton.cpp \
    $$PWD/colorpalette.cpp \
    $$PWD/colortextbutton.cpp \
    $$PWD/colorselectbutton.cpp

FORMS += \
    $$PWD/DialogInsertHyperlink.ui \
    $$PWD/FormattingToolbar.ui

RESOURCES += \
    $$PWD/FormattingToolbar.qrc
