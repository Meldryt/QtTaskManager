#include "GpuInfo.h"
#include "GpuInfoNVidia.h"
#include "GpuInfoAmd.h"

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QDebug>

GpuInfo::GpuInfo()
{
}

void GpuInfo::init()
{
    m_gpuInfoAmd = new GpuInfoAmd();
    m_gpuInfoNVidia = new GpuInfoNVidia();

    detectGpu();

    if(m_gpuDetected)
    {
        fetchStaticInfo();
    }
}

void GpuInfo::update()
{
    if(m_gpuDetected)
    {
        fetchDynamicInfo();
    }
}

void GpuInfo::detectGpu()
{
    QOpenGLContext context;
    context.create();

    QOffscreenSurface surface;
    surface.create();
    context.makeCurrent(&surface);

    QOpenGLFunctions* gl = context.functions();

    const std::string vendor = reinterpret_cast<const char*>(gl->glGetString(GL_VENDOR));
    const std::string renderer = reinterpret_cast<const char*>(gl->glGetString(GL_RENDERER));

    m_staticInfo.gpuModel = renderer;
    //const std::string version = reinterpret_cast<const char*>(gl->glGetString(GL_VERSION));
    //const std::string extension = reinterpret_cast<const char*>(gl->glGetString(GL_EXTENSIONS));
    const bool isAmd = renderer.find("AMD") != std::string::npos || vendor.find("ATI") != std::string::npos;
    const bool isNVidia = renderer.find("NVIDIA") != std::string::npos || vendor.find("NVIDIA") != std::string::npos;

    if (isAmd && m_gpuInfoAmd->detectGpu())
    {
        m_gpuManufacturer = GpuManufacturer::AMD;
        m_staticInfo.chipDesigner = "AMD";
        m_gpuDetected = true;
    }
    else if (isNVidia && m_gpuInfoNVidia->detectGpu())
    {
        m_gpuManufacturer = GpuManufacturer::NVIDIA;
        m_staticInfo.chipDesigner = "NVIDIA";
        m_gpuDetected = true;
    }
    else
    {
        m_gpuManufacturer = GpuManufacturer::UNKNOWN;
        m_gpuDetected = false;
    }
}

void GpuInfo::fetchStaticInfo()
{
    if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        m_gpuInfoNVidia->fetchStaticInfo();
        m_staticInfo = m_gpuInfoNVidia->staticInfo();
    }
    else if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        m_gpuInfoAmd->fetchStaticInfo();
        m_staticInfo = m_gpuInfoAmd->staticInfo();
    }
}

void GpuInfo::fetchDynamicInfo()
{
    if(m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        m_gpuInfoNVidia->fetchDynamicInfo();
        m_dynamicInfo = m_gpuInfoNVidia->dynamicInfo();
    }
    else
    {
        m_gpuInfoAmd->fetchDynamicInfo();
        m_dynamicInfo = m_gpuInfoAmd->dynamicInfo();
    }
}
