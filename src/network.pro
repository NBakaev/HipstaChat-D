#-------------------------------------------------
#
# Project created by QtCreator 2011-10-28T11:20:51
#
#-------------------------------------------------

CONFIG += c++11

QT += core gui network widgets
QT += sql

TARGET = network
TEMPLATE = app

#RESOURCES     += index.html
#copydata.commands = $(COPY_DIR) $$PWD/data $$OUT_PWD

#copydata.commands = my_/db.db $$PWD/data $$OUT_PWD
QT += webkitwidgets

!equals(PWD, $${OUT_PWD}){
        copydata.commands = echo "creating destination dirs" ;
        # now make a dir
        copydata.commands += mkdir -p $$OUT_PWD/app ;
        copydata.commands += echo "copying files" ;
        # then copy the files
        copydata.commands += $(COPY_DIR) $$PWD/app/* $$OUT_PWD/app/ ;
        copydata.commands += $(COPY_DIR) $$PWD/my_db.db $$OUT_PWD;
        copydata.commands += $(COPY_DIR) $$PWD/index.html $$OUT_PWD;

        # now make sure the first target is built before copy
        first.depends = $(first) copydata
        export(first.depends)
        export(copydata.commands)
        # now add it as an extra target
        QMAKE_EXTRA_TARGETS += first copydata
}

SOURCES += main.cpp\
        mainwindow.cpp \
    message.cpp \
    requestdispatcher.cpp \
    useraccount.cpp \
    serializeservice.cpp \
    dbconnection.cpp \
    fileservice.cpp \
    hipstahttprequest.cpp \
    hipstaroom.cpp \
    hipstanotification.cpp \
    debuginfo.cpp \
    angularclient.cpp \
    mytcpsocket.cpp

HEADERS  += mainwindow.h \
    message.h \
    requestdispatcher.h \
    useraccount.h \
    serializeservice.h \
    dbconnection.h \
    fileservice.h \
    hipstahttprequest.h \
    hipstaroom.h \
    hipstanotification.h \
    debuginfo.h \
    angularclient.h \
    mytcpsocket.h

FORMS    += mainwindow.ui \
    debuginfo.ui \
    angularclient.ui

DISTFILES += \
    my_db.db \
    Index.html
