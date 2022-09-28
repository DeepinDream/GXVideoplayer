#include "ffmpegwrapper.h"
#include "ffmpegdllfuncdef.h"

static const QString K_avcodec_DLLName = "avcodec-58.dll";
static const QString K_avformat_DLLName = "avformat-58.dll";
static const QString K_avutil_DLLName = "avutil-56.dll";
static const QString K_swresample_DLLName = "swresample-3.dll";
static const QString K_swscale_DLLName = "swscale-5.dll";

FFMPegWrapper& FFMPegWrapper::instance()
{
    static FFMPegWrapper s_Instance;
    return s_Instance;
}

FFMPegWrapper::FFMPegWrapper()
    : m_init(false)
    , m_h_avcodec(NULL)
    , m_h_avformat(NULL)
    , m_h_avutil(NULL)
    , m_h_swscale(NULL)
{
    m_avcodec_ExportFuncs = new C_avcodec_ExportFuncs;
    m_avformat_ExportFuncs = new C_avformat_ExportFuncs;
    m_avutil_ExportFuncs = new C_avutil_ExportFuncs;
    m_swresample_ExportFuncs = new C_swresample_ExportFuncs;
    m_swscale_ExportFuncs = new C_swscale_ExportFuncs;
    clearProcAddress();
}

FFMPegWrapper::~FFMPegWrapper()
{
    uninit();
    delete m_avcodec_ExportFuncs;
    delete m_avformat_ExportFuncs;
    delete m_avutil_ExportFuncs;
    delete m_swresample_ExportFuncs;
    delete m_swscale_ExportFuncs;
}

bool FFMPegWrapper::isReady()
{
    return m_init;
}

bool FFMPegWrapper::init(const QString& dir)
{
    if (m_init)
    {
        return true;
    }

    m_dllDir = dir;
    if (!loadDlls())
    {
        return false;
    }

    if (!getProcAddress())
    {
        return false;
    }

    m_avformat_ExportFuncs->av_register_allPtr();
    m_init = true;
    return true;
}

void FFMPegWrapper::uninit()
{
    m_init = false;
    clearProcAddress();
    freeAllDlls();
}

C_avcodec_ExportFuncs& FFMPegWrapper::get_avcodec_ExportFuncs()
{
    return *m_avcodec_ExportFuncs;
}

C_avformat_ExportFuncs& FFMPegWrapper::get_avformat_ExportFuncs()
{
    return *m_avformat_ExportFuncs;
}

C_avutil_ExportFuncs& FFMPegWrapper::get_avutil_ExportFuncs()
{
    return *m_avutil_ExportFuncs;
}

C_swresample_ExportFuncs &FFMPegWrapper::get_swrespamle_ExportFuncs()
{
    return *m_swresample_ExportFuncs;
}

C_swscale_ExportFuncs& FFMPegWrapper::get_swscale_ExportFuncs()
{
    return *m_swscale_ExportFuncs;
}

bool FFMPegWrapper::loadDlls()
{
    if (!load_avcodec_Module() ||
        !load_avformat_Module() ||
        !load_avutil_Module() ||
        !load_swresample_Module() ||
        !load_swscale_Module())
    {
        return false;
    }
    return true;
}

bool FFMPegWrapper::load_avcodec_Module()
{
    if (m_h_avcodec != NULL)
    {
        return true;
    }

    m_h_avcodec = loadDll(K_avcodec_DLLName);
    if (m_h_avcodec != NULL)
    {
        return true;
    }
    return false;
}

bool FFMPegWrapper::load_avformat_Module()
{
    if (m_h_avformat != NULL)
    {
        return true;
    }

    m_h_avformat = loadDll(K_avformat_DLLName);
    if (m_h_avformat != NULL)
    {
        return true;
    }
    return false;
}

bool FFMPegWrapper::load_avutil_Module()
{
    if (m_h_avutil != NULL)
    {
        return true;
    }

    m_h_avutil = loadDll(K_avutil_DLLName);
    if (m_h_avutil != NULL)
    {
        return true;
    }
    return false;
}

bool FFMPegWrapper::load_swresample_Module()
{
    if (m_h_swresample != NULL)
    {
        return true;
    }

    m_h_swresample = loadDll(K_swresample_DLLName);
    if (m_h_swresample != NULL)
    {
        return true;
    }
    return false;
}

bool FFMPegWrapper::load_swscale_Module()
{
    if (m_h_swscale != NULL)
    {
        return true;
    }

    m_h_swscale = loadDll(K_swscale_DLLName);
    if (m_h_swscale != NULL)
    {
        return true;
    }
    return false;
}

HMODULE FFMPegWrapper::loadDll(const QString& moduleName)
{
    QString path;
    if (m_dllDir.endsWith("\\"))
        path = m_dllDir + moduleName;
    else
        path = m_dllDir + "\\" + moduleName;
    auto test = path.toStdWString();
    return LoadLibraryExW(path.toStdWString().c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
}

bool FFMPegWrapper::getProcAddress()
{
    if (!get_avcodec_ModuleProc() ||
        !get_avformat_ModuleProc() ||
        !get_avutil_ModuleProc() ||
        !get_swresample_ModuleProc() ||
        !get_swscale_ModuleProc())
    {
        return false;
    }
    return true;
}

bool FFMPegWrapper::get_avcodec_ModuleProc()
{
    if (m_h_avcodec == NULL)
    {
        return false;
    }

    return m_avcodec_ExportFuncs->getProcAddess(m_h_avcodec);
}

bool FFMPegWrapper::get_avformat_ModuleProc()
{
    if (m_h_avformat == NULL)
    {
        return false;
    }

    return m_avformat_ExportFuncs->getProcAddess(m_h_avformat);
}

bool FFMPegWrapper::get_avutil_ModuleProc()
{
    if (m_h_avutil == NULL)
    {
        return false;
    }

    return m_avutil_ExportFuncs->getProcAddess(m_h_avutil);
}

bool FFMPegWrapper::get_swresample_ModuleProc()
{
    if (m_h_swresample == NULL)
    {
        return false;
    }

    return m_swresample_ExportFuncs->getProcAddess(m_h_swresample);
}

bool FFMPegWrapper::get_swscale_ModuleProc()
{
    if (m_h_swscale == NULL)
    {
        return false;
    }

    return m_swscale_ExportFuncs->getProcAddess(m_h_swscale);
}

void FFMPegWrapper::clearProcAddress()
{
    memset(m_avcodec_ExportFuncs, 0, sizeof(C_avcodec_ExportFuncs));
    memset(m_avformat_ExportFuncs, 0, sizeof(C_avformat_ExportFuncs));
    memset(m_avutil_ExportFuncs, 0, sizeof(C_avutil_ExportFuncs));
    memset(m_swresample_ExportFuncs, 0, sizeof(C_swresample_ExportFuncs));
    memset(m_swscale_ExportFuncs, 0, sizeof(C_swscale_ExportFuncs));
}

void FFMPegWrapper::freeAllDlls()
{
    FreeLibrary(m_h_avcodec);
    FreeLibrary(m_h_avformat);
    FreeLibrary(m_h_avutil);
    FreeLibrary(m_h_swresample);
    FreeLibrary(m_h_swscale);
}
