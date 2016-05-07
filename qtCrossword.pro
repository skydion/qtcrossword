# -------------------------------------------------
# Project created by QtCreator 2010-02-14T11:31:25
# -------------------------------------------------
TARGET = qtCrossword
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    previewtemplatedelegate.cpp \
    templatelistwidget.cpp \
    tabletemplatewidget.cpp \
    previewcelldelegate.cpp \
    makecrosswordthread.cpp \
    proxytabletolist.cpp
HEADERS += mainwindow.h \
    previewtemplatedelegate.h \
    templatelistwidget.h \
    tabletemplatewidget.h \
    previewcelldelegate.h \
    crosswordinfo.h \
    makecrosswordthread.h \
    proxytabletolist.h
FORMS += forms/mainwindow.ui \
    forms/SizeOfTemplate.ui
RESOURCES += resource.qrc
QT += gui \
      sql \
      network \
      widgets \
      printsupport \
      xml
CONFIG += debug_and_release
OTHER_FILES += qribbon.qss \
    crossword.sql
