#-------------------------------------------------
#
# Project created by QtCreator 2018-07-27T10:48:27
#
#-------------------------------------------------

QT       += core gui widgets
QT       += xml
QT       += printsupport
QT       += network
TARGET = Electronic_Signature
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
PKGCONFIG += poppler-qt5
INCLUDEPATH += /usr/local/ssl/include
LIBS += -L/usr/local/ssl/lib -lcrypto \
                -L/usr/local/ssl/lib -lssl \
                -ldl


CONFIG += c++11 link_pkgconfig

SOURCES += \
    src/controller/businessmodel.cpp \
    src/controller/pdfcanvas.cpp \
    src/controller/pdfdocument.cpp \
    src/interface/batchvalidation.cpp \
    src/interface/display_info.cpp \
    src/interface/electronicseal.cpp \
    src/interface/mainwindow.cpp \
    src/interface/makesealwindow.cpp \
    src/interface/menuitemwidget.cpp \
    src/interface/pagejump.cpp \
    src/interface/softinfodialog.cpp \
    src/interface/unsealpasswdui.cpp \
    src/interface/unsealui.cpp \
    src/interface/usermanualdialog.cpp \
    src/utils/calculator.cpp \
    src/utils/constructor.cpp \
    src/utils/pdfutils.cpp \
    src/utils/pdfwriter.cpp \
    src/utils/sealpictrue.cpp \
    src/utils/unseal.cpp \
    main.cpp \
    src/network/network.cpp \
    src/interface/logindialog.cpp \
    src/interface/changepassworddialog.cpp \
    src/interface/chaincodeinfodialog.cpp

HEADERS += \
    include/controller/businessmodel.h \
    include/controller/pdfcanvas.h \
    include/controller/pdfdocument.h \
    include/interface/batchvalidation.h \
    include/interface/display_info.h \
    include/interface/electronicseal.h \
    include/interface/mainwindow.h \
    include/interface/makesealwindow.h \
    include/interface/menuitemwidget.h \
    include/interface/pagejump.h \
    include/interface/softinfodialog.h \
    include/interface/unsealpasswdui.h \
    include/interface/unsealui.h \
    include/interface/usermanualdialog.h \
    include/utils/calculator.h \
    include/utils/constructor.h \
    include/utils/pdfutils.h \
    include/utils/pdfwriter.h \
    include/utils/sealpictrue.h \
    include/utils/unseal.h \
    include/network/network.h \
    include/interface/logindialog.h \
    include/interface/changepassworddialog.h \
    include/interface/chaincodeinfodialog.h

FORMS += \
    resource/ui/mainwindow.ui \
    resource/ui/batchvalidation.ui \
    resource/ui/pagejump.ui \
    resource/ui/electronicseal.ui \
    resource/ui/makesealdialog.ui \
    resource/ui/display_info.ui \
    resource/ui/unsealui.ui \
    resource/ui/unsealpasswdui.ui \
    resource/ui/softinfodialog.ui \
    resource/ui/usermanualdialog.ui \
    resource/ui/logindialog.ui \
    resource/ui/changepassworddialog.ui \
    resource/ui/chaincodeinfodialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resource/图片/open.png \
    resource/图片/save 保存文档.png \
    resource/图片/上一页.png \
    resource/图片/下一页.png \
    resource/图片/书.png \
    resource/图片/全屏.png \
    resource/图片/关闭.png \
    resource/图片/制作印章.png \
    resource/图片/另存为.png \
    resource/图片/撤章.png \
    resource/图片/放大.png \
    resource/图片/最后一页.png \
    resource/图片/盖章.png \
    resource/图片/第一页.png \
    resource/图片/缩小.png \
    resource/图片/跳转页面.png \
    resource/图片/退出.png \
    resource/图片/适合页面.png \
    resource/图片/隐藏.png \
    resource/图片/验证.png

RESOURCES += \
    图标资源.qrc \
    menu.qrc

