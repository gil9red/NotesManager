QT += core gui

SOURCES += \
    Manager/Manager.cpp \
    Manager/pages/page_settings.cpp \
    Manager/pages/page_notes.cpp \
    Manager/pages/page_about.cpp \
    Manager/FancyTabBar/stylehelper.cpp \
    Manager/FancyTabBar/fancytabbar.cpp \
    Manager/FancyTabBar/fancytab.cpp \
    Manager/ImageCropper/imagecropper.cpp \
    Manager/ImageCropper/fullscreenshotcropper.cpp \
    Manager/pages/page_trash.cpp

FORMS += \
    Manager/Manager.ui \
    Manager/pages/page_settings.ui \
    Manager/pages/page_notes.ui \
    Manager/pages/page_about.ui \
    Manager/pages/page_trash.ui

RESOURCES += \
    Manager/Manager.qrc

HEADERS += \
    Manager/pages/page_settings.h \
    Manager/pages/page_notes.h \
    Manager/pages/page_about.h \
    Manager/FancyTabBar/stylehelper.h \
    Manager/FancyTabBar/fancytabbar.h \
    Manager/FancyTabBar/fancytab.h \
    Manager/Manager.h \
    Manager/ImageCropper/imagecropper_p.h \
    Manager/ImageCropper/imagecropper_e.h \
    Manager/ImageCropper/imagecropper.h \
    Manager/ImageCropper/fullscreenshotcropper.h \
    Manager/pages/page_trash.h
