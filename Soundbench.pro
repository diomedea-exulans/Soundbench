#######################################################################
# Automatically generated by the Soundbench team at some point in time.
#######################################################################

QMAKE_CXXFLAGS_WARN_ON += -Wall -Wextra -pedantic
QMAKE_CXXFLAGS += -std=c++0x -D IS_SOUNDBENCH
QMAKE_CXXFLAGS_RELEASE += -O1

TEMPLATE = app
TARGET = 
DEPENDPATH += .

unix: INCLUDEPATH += /usr/include/

# Input
HEADERS += architect.h \
           backend.portaudio.h \
           basicwaves.h \
           blueprint.h \
           common.h \
           emitter.h \
           errorpopup.h \
           gentypedialog.h \
           soundbenchmain.h \
           synth.h \
           warningpopup.h \
    application.h \
    meterupdater.h \
    player.h \
    midichain.h \
    problemfix.h \
    backend.h \
    sbutilities.h \
    midienums.h \
    settings_basicgen.h \
    soundbases.h \
    gener_basic.h \
    cpumeter.h \
    midifio.h \
    sfwriter.h \
    sampletable.h \
    midirtio.h

FORMS += errorpopup.ui \
         gentypedialog.ui \
         soundbenchmain.ui \
         warningpopup.ui \
    basicgenersettings.ui

SOURCES += architect.cpp \
           backend.portaudio.cpp \
           basicwaves.cpp \
           emitter.cpp \
           errorpopup.cpp \
           gentypedialog.cpp \
           main.cpp \
           warningpopup.cpp \
    synth.cpp \
    application.cpp \
    meterupdater.cpp \
    soundbenchmain-slots.cpp \
    soundbenchmain-func.cpp \
    settings_basicgen.cpp \
    gener_basic.cpp \
    soundbenchmain-init.cpp \
    midifio-read.cpp \
    midifio-readeropen.cpp \
    midifio-misc.cpp \
    sfwriter.cpp \
    player-load.cpp \
    player-write.cpp \
    sampletable.cpp \
    midirtio.cpp

RESOURCES += sbMainResources.qrc

unix: LIBS += -L/usr/lib/ -lportaudio -lportaudiocpp -lsndfile

OTHER_FILES += \
    ProgrammingStyleRegulations.txt
