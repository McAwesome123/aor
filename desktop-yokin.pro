######################################################################
# Automatically generated by qmake (3.1) Sun Nov 6 01:04:38 2022
######################################################################

TEMPLATE = app
TARGET = aor
INCLUDEPATH += .
CONFIG += debug_and_release
CONFIG += static
CONFIG += c++17
RC_ICONS = assets/icon.ico
QMAKE_CXXFLAGS += "-g -pg -Wfatal-errors"
QMAKE_LFLAGS += "-pg"

RESOURCES += images.qrc
FORMS += assets/ui/main.ui
FORMS += assets/ui/cheat.ui
FORMS += assets/ui/tooltip.ui
FORMS += assets/ui/encyclopedia.ui
FORMS += assets/ui/log.ui

QT += gui
QT += widgets
QT += network
QT += multimedia

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
SOURCES += $$files($$PWD/src/*.cpp)
SOURCES += $$files($$PWD/src/slot/*.cpp)
HEADERS += $$files($$PWD/src/*.h)
HEADERS += $$files($$PWD/src/slot/*.h)
