#-------------------------------------------------
#
# Project created by QtCreator 2014-01-13T16:57:23
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qrush
TEMPLATE = app

SOURCES += main.cpp \
    MainWindow.cpp \
    GLWidget.cpp \
    Highlighter.cpp

HEADERS  += \
    MainWindow.h \
    GLWidget.h \
    Highlighter.h

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
