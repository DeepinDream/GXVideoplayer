QT       += core gui opengl multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += depends/FFmpeg/include

SOURCES += \
    audioplayer.cpp \
    audioresample.cpp \
    ffmpegwrapper/ffmpeg_avcodecdllfuncdef.cpp \
    ffmpegwrapper/ffmpeg_avformatdllfuncdef.cpp \
    ffmpegwrapper/ffmpeg_avutildllfuncdef.cpp \
    ffmpegwrapper/ffmpeg_resampledllfuncdef.cpp \
    ffmpegwrapper/ffmpeg_swscaledllfuncdef.cpp \
    ffmpegwrapper/ffmpegwrapper.cpp \
    halaudiooutput.cpp \
    main.cpp \
    mediadecoder.cpp \
    mediademuxer.cpp \
    mediaplayer.cpp \
    myavpacket.cpp \
    myslider.cpp \
    threadconfig.cpp \
    videoplayer.cpp \
    videorender.cpp \
    videowidget.cpp \
    widget.cpp

HEADERS += \
    IHalAudioOutput.h \
    IVideoCall.h \
    audioplayer.h \
    audioresample.h \
    ffmpegwrapper/ffmpeg_avcodecdllfuncdef.h \
    ffmpegwrapper/ffmpeg_avformatdllfuncdef.h \
    ffmpegwrapper/ffmpeg_avutildllfuncdef.h \
    ffmpegwrapper/ffmpeg_resampledllfuncdef.h \
    ffmpegwrapper/ffmpeg_swscaledllfuncdef.h \
    ffmpegwrapper/ffmpegcommondef.h \
    ffmpegwrapper/ffmpegdllfuncdef.h \
    ffmpegwrapper/ffmpegwrapper.h \
    halaudiooutput.h \
    mediadecoder.h \
    mediademuxer.h \
    mediaplayer.h \
    myavpacket.h \
    myslider.h \
    threadconfig.h \
    videoplayer.h \
    videorender.h \
    videowidget.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
