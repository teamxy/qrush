#-------------------------------------------------
#
# Project created by QtCreator 2014-01-13T16:57:23
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qrush
TEMPLATE = app

CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7
QMAKE_LFLAGS += -v -std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7

SOURCES += main.cpp \
    MainWindow.cpp \
    GLWidget.cpp \
    Brush.cpp

HEADERS  += \
    MainWindow.h \
    GLWidget.h \
    Brush.h

FORMS    += \
    MainWindow.ui

OTHER_FILES += \
    local.pri

include(local.pri)

!isEmpty(v8) {
    message("Your v8 path is $$v8")
} else {
    error("Variable v8 path not set in local.pri")
}

INCLUDEPATH += $$v8/include -I

LIBS += $$v8/out/native/{libv8_{base.x64,snapshot},libicu{uc,i18n,data}}.a
LIBS += -stdlib=libc++ -mmacosx-version-min=10.7
