#pragma once

#include <Windows.h>
#include <QString>

struct C_avcodec_ExportFuncs;
struct C_avformat_ExportFuncs;
struct C_avutil_ExportFuncs;
struct C_swresample_ExportFuncs;
struct C_swscale_ExportFuncs;

class FFMPegWrapper
{
public:
    static FFMPegWrapper& instance();

private:
    FFMPegWrapper();
    ~FFMPegWrapper();
    FFMPegWrapper(const FFMPegWrapper&);
    FFMPegWrapper& operator=(const FFMPegWrapper&);

public:
    bool init(const QString& dir);
    void uninit();
    bool isReady();

    C_avcodec_ExportFuncs& get_avcodec_ExportFuncs();
    C_avformat_ExportFuncs& get_avformat_ExportFuncs();
    C_avutil_ExportFuncs& get_avutil_ExportFuncs();
    C_swresample_ExportFuncs& get_swrespamle_ExportFuncs();
    C_swscale_ExportFuncs& get_swscale_ExportFuncs();

private:
    bool loadDlls();
    bool load_avcodec_Module();
    bool load_avformat_Module();
    bool load_avutil_Module();
    bool load_swresample_Module();
    bool load_swscale_Module();
    HMODULE loadDll(const QString& moduleName);

    bool getProcAddress();
    bool get_avcodec_ModuleProc();
    bool get_avformat_ModuleProc();
    bool get_avutil_ModuleProc();
    bool get_swresample_ModuleProc();
    bool get_swscale_ModuleProc();

    void freeAllDlls();
    void clearProcAddress();

private:
    bool m_init;

    HMODULE m_h_avcodec;
    HMODULE m_h_avformat;
    HMODULE m_h_avutil;
    HMODULE m_h_swresample;
    HMODULE m_h_swscale;

    C_avcodec_ExportFuncs *m_avcodec_ExportFuncs;
    C_avformat_ExportFuncs *m_avformat_ExportFuncs;
    C_avutil_ExportFuncs *m_avutil_ExportFuncs;
    C_swresample_ExportFuncs *m_swresample_ExportFuncs;
    C_swscale_ExportFuncs *m_swscale_ExportFuncs;

    QString m_dllDir;
};
