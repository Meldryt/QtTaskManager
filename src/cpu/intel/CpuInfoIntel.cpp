#include "CpuInfoIntel.h"

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

#include "cpucounters.h"
#include <vector>

using namespace pcm;
static SystemCounterState sysstate;
static std::vector<SocketCounterState> sockstate;
static std::vector<CoreCounterState> corestate;

static SystemCounterState oldsysstate;
static std::vector<SocketCounterState> oldsockstate;
static std::vector<CoreCounterState> oldcorestate;

static void readCounters()
{
  PCM * m = PCM::getInstance();
  sysstate = getSystemCounterState();
  sockstate[0] = getSocketCounterState(0);
  sockstate[1] = getSocketCounterState(1);
  for(uint32_t i=0;i<m->getNumCores();++i)
    corestate[i] = getCoreCounterState(i);
}

float coreUtil[2][16];
float iMCUtil[2] = {0,0};
float QPIUtil[2] = {0,0};
float PackagePower[2] = {0,0};
float DRAMPower[2] = {0,0};

static void updateKPIs()
{
  int sock_it[2] = {0,0};
  
  PCM * m = PCM::getInstance();
  
  std::swap(sysstate,oldsysstate);
  std::swap(sockstate,oldsockstate);
  std::swap(corestate,oldcorestate);
  
  readCounters();
  
  for(uint32_t i=0;i<m->getNumCores();++i)
  {
    int sock = m->getSocketId(i);
    
    // derive metric from max IPS
    coreUtil[sock][sock_it[sock]] = double(getInstructionsRetired(oldcorestate[i],corestate[i]))/ (double(m->getNominalFrequency())*double(m->getMaxIPC()));
    ++(sock_it[sock]);
  }
  
  float maxMemBW = 40.*1024.*1024.*1024.; // not precise
  
  for(uint32_t i=0;i<m->getNumSockets();++i)
    iMCUtil[i] = double(getBytesReadFromMC(oldsockstate[i],sockstate[i])+getBytesWrittenToMC(oldsockstate[i],sockstate[i]))/maxMemBW;
  
  
  float maxQPIBW = 2.*8.*1024.*1024.*1024.;
  
  // link 0
  QPIUtil[0] = double(getIncomingQPILinkBytes(0,0,oldsysstate,sysstate) + getIncomingQPILinkBytes(1,0,oldsysstate,sysstate))/maxQPIBW;
  
  // link 0
  QPIUtil[1] = double(getIncomingQPILinkBytes(0,1,oldsysstate,sysstate) + getIncomingQPILinkBytes(1,1,oldsysstate,sysstate))/maxQPIBW;
  
  float maxPower = 95.; // max TDP
  float maxDRAMPower = 40.; // max TDP, just for reference, no exact value is known...
  
  for(uint32_t i=0;i<m->getNumSockets();++i)
  {
    PackagePower[i] = double(getConsumedJoules(oldsockstate[i],sockstate[i]))/maxPower;
    DRAMPower[i] = double(getDRAMConsumedJoules(oldsockstate[i],sockstate[i]))/maxDRAMPower;
  }
  
}


CpuInfoIntel::CpuInfoIntel()
{
    qDebug() << __FUNCTION__;
}

CpuInfoIntel::~CpuInfoIntel()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& CpuInfoIntel::staticInfo() const
{
    updateKPIs();

    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& CpuInfoIntel::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfoIntel::init()
{
}

void CpuInfoIntel::readStaticInfo()
{
}

void CpuInfoIntel::readDynamicInfo()
{
}