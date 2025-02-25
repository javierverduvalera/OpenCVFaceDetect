
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DeteccionDnn.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DeteccionDnn.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += "C:\OpenCV\OpenCV4.10.0G\include"
LIBS += -L"C:\OpenCV\OpenCV4.10.0G\lib"\
 -llibopencv_world4100

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
