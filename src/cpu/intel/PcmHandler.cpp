#include "PcmHandler.h"

#include "../../Globals.h"

#ifdef __linux__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#endif

#include <QDebug>

#include <stdio.h>
//#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>

//#ifdef PCM_EXPORTS
// pcm-lib.h includes cpucounters.h
#ifdef _WIN32
#include "windows\pcm-lib.h"
#endif
//#else
//#include "cpucounters.h"
//#endif
#include "msr.h"
#include "pci.h"
#include "types.h"
#include "utils.h"
#include "topology.h"
#include <vector>

using namespace pcm;

PCM* m_pcmInstance;

#define PCM_DELAY_DEFAULT 1.0       // in seconds
#define PCM_DELAY_MIN 0.015         // 15 milliseconds is practical on most modern CPUs
double delay = -1.0;

uint64 BeforeTime = 0, AfterTime = 0;

std::vector<CoreCounterState> beforeCoreCounterState;
std::vector<CoreCounterState> afterCoreCounterState;

std::vector<ServerUncoreCounterState> beforeUncoreCounterState;
std::vector<ServerUncoreCounterState> afterUncoreCounterState;

SystemCounterState dummySystemState;
std::vector<CoreCounterState> dummyCoreStates;
std::vector<SocketCounterState> beforeSocketState, afterSocketState;

void PcmHandler::updateKPIs()
{
    AfterTime = m_pcmInstance->getTickCount();

    //for (uint32_t i = 0; i < m_cpuCoreCount; ++i)
    //{
    //    afterCoreCounterState[i] = m_pcmInstance->getCoreCounterState(i);
    //}

    for (uint32_t i = 0; i < m_cpuSocketCount; ++i)
    {
        afterUncoreCounterState[i] = m_pcmInstance->getServerUncoreCounterState(i);
    }

    m_pcmInstance->getAllCounterStates(dummySystemState, afterSocketState, afterCoreCounterState);

    for (uint32_t core_id = 0; core_id < m_cpuCoreCount; ++core_id)
    {
        m_cpuCoreFrequencies[core_id] = getAverageFrequency(beforeCoreCounterState[core_id], afterCoreCounterState[core_id]) / 1000000;
        m_cpuCoreUsages[core_id] = getExecUsage(beforeCoreCounterState[core_id], afterCoreCounterState[core_id]);
        m_cpuCoreIpc[core_id] = getIPC(beforeCoreCounterState[core_id], afterCoreCounterState[core_id]);

        //m_cpuCoreUsages[core_id] = getTotalExecUsage(beforeCoreCounterState[core_id], afterCoreCounterState[core_id]);
    }

    for (uint32_t socket_id = 0; socket_id < m_cpuSocketCount; ++socket_id)
    {
        m_cpuSocketPackagePower[socket_id] = 1000. * double(getConsumedJoules(beforeUncoreCounterState[socket_id], afterUncoreCounterState[socket_id])) / double(AfterTime - BeforeTime);
        m_cpuSocketDrawPower[socket_id] = 1000. * double(getDRAMConsumedJoules(beforeUncoreCounterState[socket_id], afterUncoreCounterState[socket_id])) / double(AfterTime - BeforeTime);
        //qDebug() << "PackagePower " << PackagePower[socket_id] << " DRAMPower " << DRAMPower[socket_id];   
    }

    //qDebug() << "beforeSocketState[0].getThermalHeadroom() " << afterSocketState[0].getThermalHeadroom();

    m_packagePower = m_cpuSocketPackagePower[0];
    m_cpuTemperature = afterSocketState[0].getThermalHeadroom();

    //std::swap(beforeCoreCounterState, afterCoreCounterState);
    std::swap(beforeUncoreCounterState, afterUncoreCounterState);
    std::swap(BeforeTime, AfterTime);
    std::swap(beforeSocketState, afterSocketState);
}


PcmHandler::PcmHandler()
{
    qDebug() << __FUNCTION__;
}

PcmHandler::~PcmHandler()
{
    qDebug() << __FUNCTION__;
}

void PcmHandler::init()
{
    readStaticInfo();
}

void PcmHandler::update()
{
    readDynamicInfo();
}

void PcmHandler::readStaticInfo()
{
    m_pcmInstance = PCM::getInstance();

    const pcm::PCM::ErrorCode status = m_pcmInstance->program();
    if (status != PCM::Success)
    {
        qWarning() << __FUNCTION__ << " : " <<status;
    }

    uint32_t cpu_model = m_pcmInstance->getCPUModel();
    uint32_t cpu_family = m_pcmInstance->getCPUFamily();
    m_cpuBrand = m_pcmInstance->getCPUBrandString();
    m_cpuFamilyModel = m_pcmInstance->getCPUFamilyModelString();
    std::string codeName = m_pcmInstance->getUArchCodename();
    
    m_cpuThermalDesignPower = m_pcmInstance->getPackageThermalSpecPower();
    m_cpuHyperThreadingEnabled = m_pcmInstance->getSMT();

    if (!m_pcmInstance->packageEnergyMetricsAvailable())
    {
        qWarning() << "Energy metrics unavailable!";
    }

    if (!m_pcmInstance->uncoreFrequencyMetricAvailable())
    {
        qWarning() << "uncoreFrequencyMetricAvailable unavailable!";
    }

    if (!m_pcmInstance->isActiveRelativeFrequencyAvailable())
    {
        qWarning() << "isActiveRelativeFrequencyAvailable unavailable!";
    }

    if (!m_pcmInstance->packageThermalMetricsAvailable())
    {
        qWarning() << "packageThermalMetricsAvailable unavailable!";
    }

    m_cpuSocketCount = m_pcmInstance->getNumSockets();
    m_cpuCoreCount = m_pcmInstance->getNumCores();
    m_cpuThreadCount = m_pcmInstance->getThreadsPerCore() * m_cpuCoreCount;

    m_cpuCoreFrequencies.resize(m_cpuCoreCount);
    m_cpuCoreUsages.resize(m_cpuCoreCount);
    m_cpuCoreIpc.resize(m_cpuCoreCount);

    m_cpuSocketPackagePower.resize(m_cpuSocketCount);
    m_cpuSocketDrawPower.resize(m_cpuSocketCount);

    beforeCoreCounterState.resize(m_cpuCoreCount);
    afterCoreCounterState.resize(m_cpuCoreCount);

    beforeUncoreCounterState.resize(m_cpuSocketCount);
    afterUncoreCounterState.resize(m_cpuSocketCount);

    //for (uint32_t i = 0; i < m_cpuCoreCount; ++i)
    //{
    //    beforeCoreCounterState[i] = m_pcmInstance->getCoreCounterState(i);
    //}

    for (uint32_t i = 0; i < m_cpuSocketCount; ++i)
    {
        beforeUncoreCounterState[i] = m_pcmInstance->getServerUncoreCounterState(i);
    }

    m_pcmInstance->getAllCounterStates(dummySystemState, beforeSocketState, beforeCoreCounterState);

    BeforeTime = m_pcmInstance->getTickCount();


}

void PcmHandler::readDynamicInfo()
{
    m_pcmInstance = PCM::getInstance();

    updateKPIs();
}
