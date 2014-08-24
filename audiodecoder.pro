#TEMPLATE = app
TARGET = audiodecoder

HEADERS = \
    audiodecoder.h \
    streamer.h \
    wavefilewriter.h
SOURCES = main.cpp \
    audiodecoder.cpp \
    streamer.cpp \
    wavefilewriter.cpp

QT += multimedia
CONFIG += sailfishapp console

#target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiodecoder
#INSTALLS += target

OTHER_FILES += \
    rpm/audiodecoder.spec
