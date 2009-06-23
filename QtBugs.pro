CONFIG += qt \
    thread \
    release \
    debug_and_release \
    build_all
TEMPLATE = app
TARGET = bugs
DEPENDPATH += .
INCLUDEPATH += ../include \
    ./GeneratedFiles

# Input
FORMS += main_window.ui \
    settings.ui \
    processorpage.ui \
    histogramdialog.ui \
    dnawidget.ui \
    dnapage.ui \
    bugeditdialog.ui
HEADERS += world_params.h \
    petri_dish.h \
    bug.h \
    dna.h \
    processor.h \
    energy_node.h \
    main_window.h \
    petriwidget.h \
    settings.h \
    processorpage.h \
    histogramdialog.h \
    dnawidget.h \
    dnapage.h \
    bugeditdialog.h
SOURCES += main.cpp \
    world_params.cpp \
    petri_dish.cpp \
    bug.cpp \
    dna.cpp \
    processor.cpp \
    energy_node.cpp \
    main_window.cpp \
    petriwidget.cpp \
    settings.cpp \
    histogramdialog.cpp \
    processorpage.cpp \
    dnawidget.cpp \
    dnapage.cpp \
    bugeditdialog.cpp

win32 { 
    QMAKE_CXXFLAGS += -mno-cygwin
    QMAKE_LFLAGS += -mno-cygwin
}
OTHER_FILES += 
CONFIG(release, debug|release) {
    DESTDIR = release
    CXXFLAGS += -O2
}
CONFIG(debug, debug|release):DESTDIR = debug

