QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/source/dir.cpp \
    src/source/diskblock.cpp \
    src/source/inode.cpp \
    src/source/main.cpp \
    src/source/superblock.cpp \
    src/source/system.cpp \
    src/source/user.cpp \
    src/source/utils.cpp \
    src/ui/loginwindow.cpp \
    src/ui/mainwindow.cpp

HEADERS += \
    src/header/config.h \
    src/header/dir.h \
    src/header/diskblock.h \
    src/header/inode.h \
    src/header/superblock.h \
    src/header/system.h \
    src/header/user.h \
    src/header/utils.h \
    src/header/view.h \
    src/ui/loginwindow.h \
    src/ui/mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    src/ui/loginwindow.ui \
    src/ui/mainwindow.ui
