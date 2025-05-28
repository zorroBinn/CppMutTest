QT       += core gui
QT += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IMutation.cpp \
    analyticalsubsystem.cpp \
    arithmeticmutation.cpp \
    buildandrunsubsystem.cpp \
    clangcompiler.cpp \
    configsubsystem.cpp \
    exceptionsmutation.cpp \
    gppcompiler.cpp \
    logicalmutation.cpp \
    main.cpp \
    maincppmuttest.cpp \
    mainguide.cpp \
    mutationmanagersubsystem.cpp \
    numericconstantsmutation.cpp \
    sourcecodeanalysissubsystem.cpp \
    stringconstantsmutation.cpp \
    userclassinfo.cpp

HEADERS += \
    ICompiler.h \
    IMutation.h \
    analyticalsubsystem.h \
    arithmeticmutation.h \
    buildandrunsubsystem.h \
    clangcompiler.h \
    configsubsystem.h \
    exceptionsmutation.h \
    gppcompiler.h \
    listofcompilationresults.h \
    listofmutantstates.h \
    listofsupportedcompilers.h \
    listofsupportedmutations.h \
    listofsupportedtestframeworks.h \
    logicalmutation.h \
    maincppmuttest.h \
    mainguide.h \
    mutationmanagersubsystem.h \
    numericconstantsmutation.h \
    sourcecodeanalysissubsystem.h \
    stringconstantsmutation.h \
    userclassinfo.h

FORMS += \
    maincppmuttest.ui \
    mainguide.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
