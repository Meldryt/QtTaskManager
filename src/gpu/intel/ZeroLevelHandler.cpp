#include "ZeroLevelHandler.h"

#include <QDebug>

#include <stdlib.h>
#include <memory>
#include <unistd.h>
#include <sys/types.h>
#include <bitset>
#include <fstream>

#define USE_LEVEL_ZERO

#ifdef USE_LEVEL_ZERO
#include "loader/ze_loader.h"

#define BYTES_IN_KB  1024
#define BYTES_IN_MB (1024 * 1024)
#define BYTES_IN_GB (1024 * 1024 * 1024)
#define W_IN_mW 1000

#define ENGINES_LENGTH  12

std::map<int, std::string> engine_types = {
    {ZES_ENGINE_GROUP_ALL,"ALL"},
    {ZES_ENGINE_GROUP_COMPUTE_ALL, "COMPUTE_ALL"},
    {ZES_ENGINE_GROUP_MEDIA_ALL, "MEDIA_ALL"},
    {ZES_ENGINE_GROUP_COPY_ALL, "COPY_ALL"},
    {ZES_ENGINE_GROUP_COMPUTE_SINGLE, "COMPUTE_SINGLE"},
    {ZES_ENGINE_GROUP_RENDER_SINGLE, "RENDER_SINGLE"},
    {ZES_ENGINE_GROUP_MEDIA_DECODE_SINGLE,
     "MEDIA_DECODE_SINGLE"},
    {ZES_ENGINE_GROUP_MEDIA_ENCODE_SINGLE,
     "MEDIA_ENCODE_SINGLE"},
    {ZES_ENGINE_GROUP_COPY_SINGLE, "COPY_SINGLE"},
    {ZES_ENGINE_GROUP_MEDIA_ENHANCEMENT_SINGLE,
     "MEDIA_ENHANCEMENT_SINGLE"},
    {ZES_ENGINE_GROUP_3D_SINGLE, "3D_SINGLE"},
    {ZES_ENGINE_GROUP_3D_RENDER_COMPUTE_ALL,
     "3D_RENDER_COMPUTE_ALL"},
    {ZES_ENGINE_GROUP_RENDER_ALL, "GROUP_RENDER_ALL"},
    {ZES_ENGINE_GROUP_3D_ALL, "3D_ALL"},
    {ZES_ENGINE_GROUP_FORCE_UINT32, "FORCE_UINT32"}
};

std::map<int, std::string> mem_health_types = {
    {ZES_MEM_HEALTH_UNKNOWN, "UNKNOWN"},
    {ZES_MEM_HEALTH_OK, "OK"},
    {ZES_MEM_HEALTH_DEGRADED, "DEGRADED"},
    {ZES_MEM_HEALTH_CRITICAL, "CRITICAL"},
    {ZES_MEM_HEALTH_REPLACE, "REPLACE"},
    {ZES_MEM_HEALTH_FORCE_UINT32, "FORCE_UINT32"}
};

std::map<int, std::string> mem_location = {
    {ZES_MEM_LOC_SYSTEM, "SYSTEM"},
    {ZES_MEM_LOC_DEVICE, "DEVICE"},
    {ZES_MEM_LOC_FORCE_UINT32, "FORCE_UINT32"}
};

std::map<int, std::string> mem_types = {
    {ZES_MEM_TYPE_HBM, "HBM"},
    {ZES_MEM_TYPE_DDR, "DDR"},
    {ZES_MEM_TYPE_DDR3, "DDR3"},
    {ZES_MEM_TYPE_DDR4, "DDR4"},
    {ZES_MEM_TYPE_DDR5, "DDR5"},
    {ZES_MEM_TYPE_LPDDR, "LPDDR"},
    {ZES_MEM_TYPE_LPDDR3, "LPDDR3"},
    {ZES_MEM_TYPE_LPDDR4, "LPDDR4"},
    {ZES_MEM_TYPE_LPDDR5, "LPDDR5"},
    {ZES_MEM_TYPE_SRAM, "SRAM"},
    {ZES_MEM_TYPE_L1, "L1"},
    {ZES_MEM_TYPE_L3, "L3"},
    {ZES_MEM_TYPE_GRF, "GRF"},
    {ZES_MEM_TYPE_SLM, "SLM"},
    {ZES_MEM_TYPE_FORCE_UINT32, "FORCE_UINT32"}
};

#if defined(_WIN32)
#define putenv_safe _putenv
#else
#define putenv_safe putenv
#endif

#endif

static std::string GetEnginesString (uint64_t engines) {
    std::string engines_string;
    std::bitset<6> bits(engines);
    std::vector<std::string> engine_flags = {
                                             "OTHER", "COMPUTE", "3D", "MEDIA", "DMA", "RENDER"};

    if (engines == 0) {
        return "UNKNOWN";
    }

    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits[i]) {
            engines_string += engine_flags[i] + ";";
        }
    }

    engines_string.pop_back();
    return engines_string;
}

inline std::vector<ze_driver_handle_t> GetDriverList() {
        ze_result_t status = ZE_RESULT_SUCCESS;

        uint32_t driver_count = 0;
        status = zeDriverGet(&driver_count, nullptr);
        assert(status == ZE_RESULT_SUCCESS);

        if (driver_count == 0) {
            return std::vector<ze_driver_handle_t>();
        }

        std::vector<ze_driver_handle_t> driver_list(driver_count);
        status = zeDriverGet(&driver_count, driver_list.data());
        assert(status == ZE_RESULT_SUCCESS);

        return driver_list;
}

inline std::vector<ze_device_handle_t> GetDeviceList(ze_driver_handle_t driver) {
        assert(driver != nullptr);
        ze_result_t status = ZE_RESULT_SUCCESS;

        uint32_t device_count = 0;
        status = zeDeviceGet(driver, &device_count, nullptr);
        assert(status == ZE_RESULT_SUCCESS);

        if (device_count == 0) {
            return std::vector<ze_device_handle_t>();
        }

        std::vector<ze_device_handle_t> device_list(device_count);
        status = zeDeviceGet(driver, &device_count, device_list.data());
        assert(status == ZE_RESULT_SUCCESS);

        return device_list;
}

inline std::vector<ze_device_handle_t> GetDeviceList() {
        std::vector<ze_device_handle_t> device_list;
        for (auto driver : GetDriverList()) {
            for (auto device : GetDeviceList(driver)) {
                device_list.push_back(device);
            }
        }
        return device_list;
}

inline std::vector<ze_device_handle_t> GetSubDeviceList(
    ze_device_handle_t device) {
        assert(device != nullptr);
        ze_result_t status = ZE_RESULT_SUCCESS;

        uint32_t sub_device_count = 0;
        status = zeDeviceGetSubDevices(device, &sub_device_count, nullptr);
        assert(status == ZE_RESULT_SUCCESS);

        if (sub_device_count == 0) {
            return std::vector<ze_device_handle_t>();
        }

        std::vector<ze_device_handle_t> sub_device_list(sub_device_count);
        status = zeDeviceGetSubDevices(
            device, &sub_device_count, sub_device_list.data());
        assert(status == ZE_RESULT_SUCCESS);

        return sub_device_list;
}

inline ze_device_handle_t GetGpuDevice() {
        std::vector<ze_device_handle_t> device_list;

        for (auto driver : GetDriverList()) {
            for (auto device : GetDeviceList(driver)) {
                ze_device_properties_t props{ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES, };
                ze_result_t status = zeDeviceGetProperties(device, &props);
                assert(status == ZE_RESULT_SUCCESS);
                if (props.type == ZE_DEVICE_TYPE_GPU) {
                    device_list.push_back(device);
                }
            }
        }

        if (device_list.empty()) {
            return nullptr;
        }

        //std::string value = getenv("PTI_DEVICE_ID");
        uint32_t device_id = 0;
        assert(device_id >= 0 && device_id < device_list.size());

        std::vector<ze_device_handle_t> sub_device_list =
            GetSubDeviceList(device_list[device_id]);
        if (sub_device_list.empty()) {
            return device_list[device_id];
        }

        //value = getenv("PTI_SUB_DEVICE_ID");
        //if (value.empty()) {
            return device_list[device_id];
        //}

            uint32_t sub_device_id = 0;//value.empty() ? 0 : std::stoul(value);
        assert(sub_device_id >= 0 && sub_device_id < sub_device_list.size());
        return sub_device_list[sub_device_id];
}

static std::string GetProcessName(uint32_t pid) {
        std::string process_name;
        std::string file_name = "/proc/" + std::to_string(pid) + "/cmdline";

        std::ifstream file;
        file.open(file_name.data());

        if (!file.is_open()) {
            return std::string();
        }

        std::getline(file, process_name, '\0');
        file.close();

        return process_name;
}

//static uint64_t GetDeviceMemSize(ze_device_handle_t device) {
//    assert(device != nullptr);
//    ze_result_t status = ZE_RESULT_SUCCESS;

//    uint32_t props_count = 0;
//    status = zeDeviceGetMemoryProperties(device, &props_count, nullptr);
//    assert(status == ZE_RESULT_SUCCESS);

//    if (props_count == 0) {
//        return 0;
//    }

//    std::vector<ze_device_memory_properties_t> props_list(
//        props_count, {ZE_STRUCTURE_TYPE_DEVICE_MEMORY_PROPERTIES, });
//    status = zeDeviceGetMemoryProperties(device, &props_count, props_list.data());
//    assert(status == ZE_RESULT_SUCCESS);

//    uint64_t total_mem_size = 0;
//    for (auto& props : props_list) {
//        total_mem_size += props.totalSize;
//    }

//    return total_mem_size;
//}

ZeroLevelHandler::ZeroLevelHandler()
{
    qDebug() << __FUNCTION__;
}

ZeroLevelHandler::~ZeroLevelHandler()
{
    qDebug() << __FUNCTION__;
}

bool ZeroLevelHandler::init()
{ 
    setenv("ZES_ENABLE_SYSMAN", "1", 1);
    ze_result_t status = zeInit(ZE_INIT_FLAG_GPU_ONLY);
    if(status == ZE_RESULT_SUCCESS)
    {
        m_initialized = true;
    }

    return m_initialized;
}

void ZeroLevelHandler::readStaticInfo()
{
    if(!m_initialized)
    {
        return;
    }

    m_zeDevice = GetGpuDevice();
    if (m_zeDevice == nullptr) {
        qDebug() << "[WARNING] GPU device was not found";
        return;
    }

    readLoaderVersions();
    readDriverVersion();
    readDeviceFirmware();

    readDeviceProperties();
    readDevicePciProperties();

    //    PrintDeviceInfo(driver, device);
    //    PrintComputeInfo(device);
    //    PrintModuleInfo(device);

    checkSupportedDynamicFunctions();
}

void ZeroLevelHandler::readLoaderVersions()
{
    m_functionsSupportStatus["zelLoaderGetVersions"] = false;

    ze_result_t status = ZE_RESULT_SUCCESS;

    zel_component_version_t* versions;
    size_t size = 0;
    status = zelLoaderGetVersions(&size, nullptr);
    if(status != ZE_RESULT_SUCCESS)
    {
        return;
    }

    qDebug() << "zelLoaderGetVersions number of components found: " << size;
    versions = new zel_component_version_t[size];
    status = zelLoaderGetVersions(&size, versions);
    if(status != ZE_RESULT_SUCCESS)
    {
        return;
    }

    for (size_t i = 0; i < size; i++) {
        qDebug() << "Version " << i;
        qDebug() << "Name: " << versions[i].component_name;
        qDebug() << "Major: " << versions[i].component_lib_version.major;
        qDebug() << "Minor: " << versions[i].component_lib_version.minor;
        qDebug() << "Patch: " << versions[i].component_lib_version.patch;
    }

    delete[] versions;

        m_functionsSupportStatus["zelLoaderGetVersions"] = true;
}

void ZeroLevelHandler::readDeviceProperties()
{
    m_functionsSupportStatus["zesDeviceGetProperties"] = false;

    ze_result_t status = ZE_RESULT_SUCCESS;

    zes_device_properties_t props{ZES_STRUCTURE_TYPE_DEVICE_PROPERTIES, };
    status = zesDeviceGetProperties(m_zeDevice, &props);
    if(status != ZE_RESULT_SUCCESS)
    {
        return;
    }

    qDebug() << "GPU " << m_deviceID;
    qDebug() << "Brand Number," << props.boardNumber;
    qDebug() << "Brand Name," << props.brandName;
    qDebug() << "Kernel Driver Version," << props.driverVersion;
    qDebug() << "Serial Number," << props.serialNumber;
    qDebug() << "Model Name," << props.modelName;
    qDebug() << "Vendor," << props.vendorName;
    qDebug() << "Subdevices," << props.numSubdevices;

    m_functionsSupportStatus["zesDeviceGetProperties"] = true;
}

void ZeroLevelHandler::readDevicePciProperties()
{
    m_functionsSupportStatus["zesDevicePciGetProperties"] = false;

    ze_result_t status = ZE_RESULT_SUCCESS;

    zes_pci_properties_t pci_props{ZES_STRUCTURE_TYPE_PCI_PROPERTIES, };
    status = zesDevicePciGetProperties(m_zeDevice, &pci_props);
    if(status != ZE_RESULT_SUCCESS)
    {
        return;
    }

    qDebug() << "PCI Bus,"
             << pci_props.address.domain << ":"
             << pci_props.address.bus << ":"
             << pci_props.address.device << "."
             << pci_props.address.function;

    qDebug() << "PCI Generation," << ((pci_props.maxSpeed.gen == -1) ?
                                          "unknown" : std::to_string(pci_props.maxSpeed.gen));

    qDebug() << "PCI Max Brandwidth(GB/s)," <<
        ((pci_props.maxSpeed.maxBandwidth == -1) ? "unknown" :
             std::to_string(pci_props.maxSpeed.maxBandwidth / BYTES_IN_GB));

    qDebug() << "PCI Width," <<
        ((pci_props.maxSpeed.width == -1) ?
             "unknown" : std::to_string(pci_props.maxSpeed.width));

    m_functionsSupportStatus["zesDevicePciGetProperties"] = true;
}

void ZeroLevelHandler::readDriverVersion()
{
    m_functionsSupportStatus["zeDriverGetProperties"] = false;

    ze_result_t status = ZE_RESULT_SUCCESS;

    std::vector<ze_driver_handle_t> driver_list = GetDriverList();
    if (driver_list.empty()) {
        return;
    }

    for (size_t i = 0; i < driver_list.size(); ++i)
    {
        ze_driver_properties_t driver_props{
            ZE_STRUCTURE_TYPE_DRIVER_PROPERTIES, };
        status = zeDriverGetProperties(driver_list.at(i), &driver_props);
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        uint32_t major = driver_props.driverVersion >> 24;
        uint32_t minor = (driver_props.driverVersion >> 16) & 0xFF;
        uint32_t rev = driver_props.driverVersion & 0xFFFF;

        qDebug() << "Level Zero GPU Driver Version," << " Driver " << i <<
            std::to_string(major) + "." +
            std::to_string(minor) + "." +
            std::to_string(rev);
    }

    m_functionsSupportStatus["zeDriverGetProperties"] = true;
}

void ZeroLevelHandler::readDeviceFirmware()
{
    m_functionsSupportStatus["zesDeviceEnumFirmwares"] = false;

    ze_result_t status = ZE_RESULT_SUCCESS;

    uint32_t firmwares_count = 0;
    status = zesDeviceEnumFirmwares(m_zeDevice, &firmwares_count, nullptr);
    if(status != ZE_RESULT_SUCCESS)
    {
        return;
    }

    if (firmwares_count > 0) {
        std::vector<zes_firmware_handle_t> firmwares(firmwares_count);
        status = zesDeviceEnumFirmwares(m_zeDevice, &firmwares_count, firmwares.data());
        if(status != ZE_RESULT_SUCCESS)
        {
                return;
        }

        for (uint32_t i = 0; i < firmwares_count; ++i) {
                zes_firmware_properties_t firmwares_props{ZES_STRUCTURE_TYPE_FIRMWARE_PROPERTIES, };
                status = zesFirmwareGetProperties(firmwares[i], &firmwares_props);
                if(status != ZE_RESULT_SUCCESS)
                {
                    return;
                }

                if((firmwares_props.onSubdevice && firmwares_props.subdeviceId == m_subDeviceID) ||
                    (!firmwares_props.onSubdevice && m_subDeviceID == UINT32_MAX))
                {
                    qDebug() << "Firmware Name," << firmwares_props.name;

                    qDebug() << "Flashing Firmware," <<
                        (firmwares_props.canControl ? "Yes" : "No");

                    qDebug() << "Firmware Version," << firmwares_props.version;

                    m_functionsSupportStatus["zesDeviceEnumFirmwares"] = true;
                }
        }
    }
}

void ZeroLevelHandler::checkSupportedDynamicFunctions()
{
    m_functionsSupportStatus["zesDeviceProcessesGetState"] = false;
    m_functionsSupportStatus["zesDeviceEnumEngineGroups"] = false;
    m_functionsSupportStatus["zesDeviceEnumFrequencyDomains"] = false;
    m_functionsSupportStatus["zesDeviceEnumMemoryModules"] = false;
    m_functionsSupportStatus["zesDeviceEnumTemperatureSensors"] = false;
    m_functionsSupportStatus["zesDeviceEnumFans"] = false;
    m_functionsSupportStatus["zesDeviceEnumPowerDomains"] = false;

    if(!m_zeDevice)
    {
        return;
    }

    ze_result_t status = ZE_RESULT_SUCCESS;

    uint32_t proc_count = 0;
    status = zesDeviceProcessesGetState(m_zeDevice, &proc_count, nullptr);
    if (status == ZE_RESULT_SUCCESS)
    {
        m_functionsSupportStatus["zesDeviceProcessesGetState"] = true;
    }

    status = zesDeviceEnumEngineGroups(m_zeDevice, &m_zesEngineGroupsCount, nullptr);
    if(status == ZE_RESULT_SUCCESS && m_zesEngineGroupsCount > 0)
    {
        m_functionsSupportStatus["zesDeviceEnumEngineGroups"] = true;
    }

    status = zesDeviceEnumFrequencyDomains(m_zeDevice, &m_zesFrequencyDomainCount, nullptr);
    if(status == ZE_RESULT_SUCCESS && m_zesFrequencyDomainCount > 0)
    {
        m_functionsSupportStatus["zesDeviceEnumFrequencyDomains"] = true;
    }

    status = zesDeviceEnumMemoryModules(m_zeDevice, &m_zesMemoryModulesCount, nullptr);
    if(status == ZE_RESULT_SUCCESS && m_zesMemoryModulesCount > 0)
    {
        m_functionsSupportStatus["zesDeviceEnumMemoryModules"] = true;
    }

    status = zesDeviceEnumTemperatureSensors(m_zeDevice, &m_zesTemperatureSensorCount, nullptr);
    if(status == ZE_RESULT_SUCCESS && m_zesTemperatureSensorCount > 0)
    {
        m_functionsSupportStatus["zesDeviceEnumTemperatureSensors"] = true;
    }

    status = zesDeviceEnumFans(m_zeDevice, &m_zesFanCount, nullptr);
    if(status == ZE_RESULT_SUCCESS && m_zesFanCount > 0)
    {
        m_functionsSupportStatus["zesDeviceEnumFans"] = true;
    }

    status = zesDeviceEnumPowerDomains(m_zeDevice, &m_zesPowerDomainCount, nullptr);
    if(status == ZE_RESULT_SUCCESS && m_zesPowerDomainCount > 0)
    {
        m_functionsSupportStatus["zesDeviceEnumPowerDomains"] = true;
    }
}

void ZeroLevelHandler::readDynamicInfo()
{
    if (!m_initialized)
    {
        return;
    }

    readProcesses();
    readGpuEngine();
    readGpuFrequencies();
    readGpuMemory();
    readGpuTemperature();
    readGpuFanSpeed();
    readGpuPower();
}

void ZeroLevelHandler::readProcesses()
{
    if (m_functionsSupportStatus["zesDeviceProcessesGetState"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        uint32_t proc_count = 0;
        status = zesDeviceProcessesGetState(m_zeDevice, &proc_count, nullptr);
        if (status != ZE_RESULT_SUCCESS || proc_count == 0) {
            return;
        }

        std::vector<zes_process_state_t> state_list(proc_count);
        status = zesDeviceProcessesGetState(m_zeDevice, &proc_count, state_list.data());
        if (status != ZE_RESULT_SUCCESS || state_list.empty()) {
            return;
        }

        uint32_t engines_length = ENGINES_LENGTH;
        for (auto& state : state_list) {
            std::string engines = GetEnginesString(state.engines);
            if (engines.size() > engines_length) {
                engines_length = engines.size();
            }
        }

        ++engines_length;

        for (auto& state : state_list) {
            qDebug() << "PID: " << state.processId;
            qDebug() << "Device Memory Used(MB): " << std::to_string(state.memSize / BYTES_IN_MB);
            qDebug() << "Shared Memory Used(MB):  " << std::to_string(state.sharedSize / BYTES_IN_MB);
            qDebug() << "GPU Engines: " << GetEnginesString(state.engines);
            qDebug() << "Executable: " << GetProcessName(state.processId);
        }
    }
}

void ZeroLevelHandler::readGpuEngine()
{
    if (m_functionsSupportStatus["zesDeviceEnumEngineGroups"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        std::vector<zes_engine_handle_t> engine_groups(m_zesEngineGroupsCount);
        status = zesDeviceEnumEngineGroups(m_zeDevice, &m_zesEngineGroupsCount, engine_groups.data());
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        if(m_engineGroupStats.empty())
        {
            m_engineGroupStats.resize(m_zesEngineGroupsCount);
            for(auto&& stat : m_engineGroupStats)
            {
                stat.activeTime = 0;
                stat.timestamp = 0;
            }
        }

        std::map<zes_engine_group_t, uint32_t> engine_map;
        std::map<zes_engine_group_t, double> engine_usage_map;

        for (uint32_t i = 0; i < m_zesEngineGroupsCount; ++i) {
            zes_engine_properties_t engine_props{
                ZES_STRUCTURE_TYPE_ENGINE_PROPERTIES, };
            status = zesEngineGetProperties(engine_groups[i], &engine_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            if((engine_props.onSubdevice && engine_props.subdeviceId == m_subDeviceID) ||
                 (!engine_props.onSubdevice && m_subDeviceID == UINT32_MAX))
            {
                if (engine_map.count(engine_props.type) == 0) {
                    engine_map[engine_props.type] = 1;
                    engine_usage_map[engine_props.type] = 0.0;
                } else {
                    engine_map[engine_props.type] += 1;
                }

                zes_engine_stats_t* stats;
                status = zesEngineGetActivity(engine_groups[i], stats);
                if(status != ZE_RESULT_SUCCESS)
                {
                    return;
                }

                const double deltaActiveTime = stats->activeTime - m_engineGroupStats[i].activeTime;
                const double deltaTimeStamp = stats->timestamp - m_engineGroupStats[i].timestamp;
                const double usage = (100.0 * deltaActiveTime / deltaTimeStamp);

                m_engineGroupStats[i].activeTime = stats->activeTime;
                m_engineGroupStats[i].timestamp = stats->timestamp;

                engine_usage_map[engine_props.type] += usage;
                qDebug() << engine_map[engine_props.type]
                         << " usage: " << usage;
            }
        }

        if (!engine_map.empty()) {
            std::string engines;
            for (auto items : engine_map) {
                    engines += engine_types[items.first] + "(" +
                               std::to_string(items.second) + ");";
            }

            std::string usages;
            for (auto items : engine_usage_map) {
                usages += std::to_string(items.second) + ";";
            }
            engines.pop_back();
            qDebug() << "Engines: " << engines +  " Usages: " << usages;
        }
    }
}

void ZeroLevelHandler::readGpuFrequencies()
{
    if (m_functionsSupportStatus["zesDeviceEnumFrequencyDomains"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        std::vector<zes_freq_handle_t> freq_domain_list(m_zesFrequencyDomainCount);
        status = zesDeviceEnumFrequencyDomains(m_zeDevice, &m_zesFrequencyDomainCount, freq_domain_list.data());
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        std::vector<zes_freq_handle_t> GPU_freq_domains;

        for (uint32_t i = 0; i < m_zesFrequencyDomainCount; ++i) {
            zes_freq_properties_t freq_domain_props{ZES_STRUCTURE_TYPE_FREQ_PROPERTIES, };
            status = zesFrequencyGetProperties(freq_domain_list[i], &freq_domain_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            if (freq_domain_props.type == ZES_FREQ_DOMAIN_GPU)
            {
                GPU_freq_domains.push_back(freq_domain_list[i]);
            }
        }

        for (uint32_t i = 0; i < GPU_freq_domains.size(); ++i)
        {
            zes_freq_properties_t freq_domain_props{ZES_STRUCTURE_TYPE_FREQ_PROPERTIES, };
            status = zesFrequencyGetProperties(GPU_freq_domains[i], &freq_domain_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            zes_freq_state_t state{ZES_STRUCTURE_TYPE_FREQ_STATE, };
            status = zesFrequencyGetState(GPU_freq_domains[i], &state);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            if ((freq_domain_props.onSubdevice &&
                 freq_domain_props.subdeviceId == m_subDeviceID) ||
                (!freq_domain_props.onSubdevice && m_subDeviceID == UINT32_MAX))
            {
                double current_frequency =
                    (state.actual < freq_domain_props.min) ?
                        freq_domain_props.min : state.actual;

                m_gpuClockSpeed = current_frequency;

                qDebug() << "Current Frequency(MHz)," <<
                    current_frequency;

                qDebug() << "Changeable Frequency," <<
                    (freq_domain_props.canControl ? "Yes" : "No");

                qDebug() << "Max Core Frequency(MHz)," <<
                    freq_domain_props.max;

                qDebug() << "Min Core Frequency(MHz)," <<
                    freq_domain_props.min;

                qDebug() << "Current Voltage(V)," <<
                    (state.currentVoltage < 0 ? "unknown" :
                         std::to_string(state.currentVoltage));

                qDebug() <<
                    "Current Frequency Request(MHz)," <<
                    (state.request < 0 ? "unknown" :
                         std::to_string(state.request));

                qDebug() <<
                    "Efficient Min Frequency(MHz)," <<
                    (state.efficient < 0 ? "unknown" :
                         std::to_string(state.efficient));

                qDebug() <<
                    "Max Frequency For Current TDP(MHz)," <<
                    (state.tdp < 0 ? "unknown" : std::to_string(state.tdp));
            }
        }
    }
}

void ZeroLevelHandler::readGpuMemory()
{
    if (m_functionsSupportStatus["zesDeviceEnumMemoryModules"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        std::vector<zes_mem_handle_t> mem_modules(m_zesMemoryModulesCount);
        status = zesDeviceEnumMemoryModules(m_zeDevice, &m_zesMemoryModulesCount, mem_modules.data());
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        for (uint32_t i = 0; i < m_zesMemoryModulesCount; ++i)
        {
            zes_mem_properties_t mem_props{ZES_STRUCTURE_TYPE_MEM_PROPERTIES, };
            status = zesMemoryGetProperties(mem_modules[i], &mem_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            if ((mem_props.onSubdevice  &&
                 mem_props.subdeviceId == m_subDeviceID) ||
                (!mem_props.onSubdevice && m_subDeviceID == UINT32_MAX))
            {
                qDebug() << "Memory Type," << mem_types[mem_props.type];
                qDebug() << "Memory Location," << mem_location[mem_props.location];
                qDebug() << "Memory Bus Width," <<
                    (mem_props.busWidth == -1 ? "unknown" :
                         std::to_string(mem_props.busWidth));
                qDebug() <<
                    "Memory Channels," <<
                    (mem_props.numChannels == -1 ? "unknown" :
                         std::to_string(mem_props.numChannels));
                qDebug() <<
                    "Physical Memory Size(MB)," <<
                    (mem_props.physicalSize == 0 ? "unknown" :
                         std::to_string(mem_props.physicalSize / BYTES_IN_MB));

                zes_mem_state_t mem_state{ZES_STRUCTURE_TYPE_MEM_STATE, };
                status = zesMemoryGetState(mem_modules[i], &mem_state);
                if (status == ZE_RESULT_SUCCESS) {
                    qDebug() <<
                        "Free Memory(MB)," <<
                        std::to_string(mem_state.free / BYTES_IN_MB);

                    qDebug() <<
                        "Total Allocatable Memory(MB)," <<
                        std::to_string(mem_state.size / BYTES_IN_MB);

                    qDebug() <<
                        "Memory Health," << mem_health_types[mem_state.health];
                }
            }
        }
    }
}

void ZeroLevelHandler::readGpuTemperature()
{
    if (m_functionsSupportStatus["zesDeviceEnumTemperatureSensors"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        std::vector<zes_temp_handle_t> sensor_list(m_zesTemperatureSensorCount);
        status = zesDeviceEnumTemperatureSensors(m_zeDevice, &m_zesTemperatureSensorCount, sensor_list.data());
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        for (uint32_t i = 0; i < m_zesTemperatureSensorCount; ++i)
        {
            zes_temp_properties_t temp_props{
                ZES_STRUCTURE_TYPE_TEMP_PROPERTIES, };
            status = zesTemperatureGetProperties(sensor_list[i], &temp_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                    return;
            }

            if((temp_props.onSubdevice && temp_props.subdeviceId == m_subDeviceID) ||
                 (!temp_props.onSubdevice && m_subDeviceID == UINT32_MAX))
            {
                if (temp_props.type == ZES_TEMP_SENSORS_GPU) {
                    double temp = 0.0f;
                    status = zesTemperatureGetState(sensor_list[i], &temp);
                    m_gpuTemperature = temp;
                }

                if (temp_props.type == ZES_TEMP_SENSORS_MEMORY) {
                    double temp = 0.0f;
                    status = zesTemperatureGetState(sensor_list[i], &temp);
                    //m_gpuMemoryTemperature = temp;
                }
            }
        }
    }
}

void ZeroLevelHandler::readGpuFanSpeed()
{
    if (m_functionsSupportStatus["zesDeviceEnumFans"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        std::vector<zes_fan_handle_t> fans(m_zesFanCount);
        status = zesDeviceEnumFans(m_zeDevice, &m_zesFanCount, fans.data());
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        for (uint32_t i = 0; i < m_zesFanCount; ++i)
        {
            zes_fan_properties_t fan_props{ZES_STRUCTURE_TYPE_FAN_PROPERTIES, };
            status = zesFanGetProperties(fans[i], &fan_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            if((fan_props.onSubdevice && fan_props.subdeviceId == m_subDeviceID) ||
                 (!fan_props.onSubdevice && m_subDeviceID == UINT32_MAX))
            {
                zes_fan_speed_units_t unit = zes_fan_speed_units_t::ZES_FAN_SPEED_UNITS_RPM;
                int32_t fanSpeed = 0;
                status = zesFanGetState(fans[i], unit, &fanSpeed);
                if(status == ZE_RESULT_SUCCESS)
                {
                    m_gpuFanSpeed = fanSpeed;
                }
                //fan_props.maxRPM
            }
        }
    }
}


void ZeroLevelHandler::readGpuPower()
{
    if (m_functionsSupportStatus["zesDeviceEnumPowerDomains"])
    {
        ze_result_t status = ZE_RESULT_SUCCESS;

        std::vector<zes_pwr_handle_t> power_domain_list(m_zesPowerDomainCount);
        status = zesDeviceEnumPowerDomains(m_zeDevice, &m_zesPowerDomainCount, power_domain_list.data());
        if(status != ZE_RESULT_SUCCESS)
        {
            return;
        }

        for (uint32_t i = 0; i < m_zesPowerDomainCount; ++i)
        {
            zes_power_properties_t power_domain_props{ZES_STRUCTURE_TYPE_POWER_PROPERTIES, };
            status = zesPowerGetProperties(power_domain_list[i], &power_domain_props);
            if(status != ZE_RESULT_SUCCESS)
            {
                return;
            }

            if((power_domain_props.onSubdevice && power_domain_props.subdeviceId == m_subDeviceID) ||
                (!power_domain_props.onSubdevice && m_subDeviceID == UINT32_MAX))
            {
                qDebug() << "Default TDP Power Limit (W)," <<
                    (power_domain_props.defaultLimit == -1 ? "unknown" :
                    std::to_string(power_domain_props.defaultLimit / W_IN_mW));

                qDebug() << "Changeable Power Limit," <<
                    (power_domain_props.canControl ? "Yes" : "No");

                qDebug() << "Max TDP Power Limit(W)," <<
                    (power_domain_props.maxLimit == -1 ? "unknown" :
                         std::to_string(power_domain_props.maxLimit / W_IN_mW));

                qDebug() << "Min TDP Power Limit(W)," <<
                    (power_domain_props.minLimit == -1 ? "unknown" :
                         std::to_string(power_domain_props.minLimit / W_IN_mW));

                qDebug() << "Supports Energy Threshold Event," <<
                    (power_domain_props.isEnergyThresholdSupported ? "Yes" : "No");
            }
        }
    }
}
