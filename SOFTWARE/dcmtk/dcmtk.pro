#-------------------------------------------------
#
# Project created by QtCreator 2017-07-01T22:59:06
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dcmtk

DEFINES += HAVE_CONFIG_H
DEFINES += USE_NULL_SAFE_OFSTRING


TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    video.cpp \
    loading.cpp

DEPENDPATH += "E:/Carrera/Biofisica_Medica/DCMTK/dcmtk-3.6.0-install/include/dcmtk/"
INCLUDEPATH = "E:/Carrera/Biofisica_Medica/DCMTK/dcmtk-3.6.0-install/include/"

LIBS += -L"E:/Carrera/Biofisica_Medica/DCMTK/dcmtk-3.6.0-install/lib"
LIBS += -ldcmimgle -ldcmdata -loflog -lofstd
win32:LIBS += -lAdvapi32 -lofstd -loflog -ldcmdata -ldcmnet -ldcmimage -ldcmimgle -lws2_32 -lnetapi32 -lwsock32


HEADERS  += mainwindow.h \
    video.h \
    loading.h

FORMS    += mainwindow.ui \
    loading.ui \

RESOURCES += \
    resources.qrc
