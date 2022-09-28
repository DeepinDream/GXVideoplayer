# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FFmpegPath = $$_PRO_FILE_PWD_/FFmpeg


greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET_ARCH=$${QT_ARCH}
} else {
    TARGET_ARCH=$${QMAKE_HOST.arch}
}

win32 {
    contains(TARGET_ARCH, x86_64) {
        #x64
        LIBS += -L$$FFmpegPath/lib/win64
        BinPath = $$_PRO_FILE_PWD_/bin/win64
        message("win64")
    } else {
        # 32
        LIBS += -L$$FFmpegPath/lib/win32
        BinPath = $$_PRO_FILE_PWD_/bin/win32
        message("win32")
    }
}
macos {
    LIBS += -L$$FFmpegPath/lib/macos
    BinPath = $$_PRO_FILE_PWD_/bin/macos
    message("macos")
}

INCLUDEPATH += $$FFmpegPath/include
LIBS += -lavutil -lavformat -lavcodec -lavdevice -lavfilter -lpostproc -lswresample -lswscale
