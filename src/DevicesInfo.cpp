#include "DevicesInfo.h"

#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <cfgmgr32.h>
//#include <portabledeviceapi.h>
//#include <wrl.h>
//#include <sysinfoapi.h>

#include <intsafe.h>
#include <string>
#include <iostream>
#include <map>

#include <stddef.h>


typedef unsigned __int64 QWORD;

//System Management BIOS (SMBIOS) Reference Specification, DSO0134 3.6.0, 20 Jun 2022
static const std::map<uint32_t,std::string> SMBIOS_PROCESSOR_UPGRADE_SPECS = {
    {0x08, "Slot 1"},
    {0x09, "Slot 2"},
    {0x0A, "370-pin socket"},
    {0x0B, "Slot A"},
    {0x0C, "Slot M"},
    {0x0D, "Socket 423"},
    {0x0E, "Socket A (Socket 462)"},
    {0x0F, "Socket 478"},
    {0x10, "Socket 754"},
    {0x11, "Socket 940"},
    {0x12, "Socket 939"},
    {0x13, "Socket mPGA604"},
    {0x14, "Socket LGA771"},
    {0x15, "Socket LGA775"},
    {0x16, "Socket S1"},
    {0x17, "Socket AM2"},
    {0x18, "Socket F (1207)"},
    {0x19, "Socket LGA1366"},
    {0x1A, "Socket G34"},
    {0x1B, "Socket AM3"},
    {0x1C, "Socket C32"},
    {0x1D, "Socket LGA1156"},
    {0x1E, "Socket LGA1567"},
    {0x1F, "Socket PGA988A"},
    {0x20, "Socket BGA1288"},
    {0x21, "Socket rPGA988B"},
    {0x22, "Socket BGA1023"},
    {0x23, "Socket BGA1224"},
    {0x24, "Socket LGA1155"},
    {0x25, "Socket LGA1356"},
    {0x26, "Socket LGA2011"},
    {0x27, "Socket FS1"},
    {0x28, "Socket FS2"},
    {0x29, "Socket FM1"},
    {0x2A, "Socket FM2"},
    {0x2B, "Socket LGA2011-3"},
    {0x2C, "Socket LGA1356-3"},
    {0x2D, "Socket LGA1150"},
    {0x2E, "Socket BGA1168"},
    {0x2F, "Socket BGA1234"},
    {0x30, "Socket BGA1364"},
    {0x31, "Socket AM4"},
    {0x32, "Socket LGA1151"},
    {0x33, "Socket BGA1356"},
    {0x34, "Socket BGA1440"},
    {0x35, "Socket BGA1515"},
    {0x36, "Socket LGA3647-1"},
    {0x37, "Socket SP3"},
    {0x38, "Socket SP3r2"},
    {0x39, "Socket LGA2066"},
    {0x3A, "Socket BGA1392"},
    {0x3B, "Socket BGA1510"},
    {0x3C, "Socket BGA1528"},
    {0x3D, "Socket LGA4189"},
    {0x3E, "Socket LGA1200"},
    {0x3F, "Socket LGA4677"},
    {0x40, "Socket LGA1700"},
    {0x41, "Socket BGA1744"},
    {0x42, "Socket BGA1781"},
    {0x43, "Socket BGA1211"},
    {0x44, "Socket BGA2422"},
    {0x45, "Socket LGA1211"},
    {0x46, "Socket LGA2422"},
    {0x47, "Socket LGA5773"},
    {0x48, "Socket BGA5773"},
};

struct RawSMBIOSData
{
    uint8_t    Used20CallingMethod;
    uint8_t    SMBIOSMajorVersion;
    uint8_t    SMBIOSMinorVersion;
    uint8_t    DmiRevision;
    uint32_t   Length;
    uint8_t    SMBIOSTableData[ANYSIZE_ARRAY];
};

struct RawSMBIOSTable
{
    uint8_t    Type;
    uint8_t    Length;
    uint16_t   Handle;
};

BYTE RawSMBIOSInfoType = 0;

struct RawSMBIOSInfo : public RawSMBIOSTable
{
    BYTE    Vendor;
    BYTE    BIOSVersion;
    WORD    BIOSStartingAddressSegment;
    BYTE    BIOSReleaseDate;
    BYTE    BIOSROMSize;
    QWORD   BIOSCharacteristics;
    BYTE    BIOSCharaczeristicsExtensionBytes[ANYSIZE_ARRAY];
    BYTE    SystemBIOSMajorRelease;
    BYTE    SystemBIOSMinorRelease;
    BYTE    EmbeddedControllerFirmwareMajorRelease;
    BYTE    EmbeddedControllerFirmwareMinorRelease;
};

BYTE RawSMBIOSSystemInfoType = 1;

struct RawSMBIOSSystemInfo : public RawSMBIOSTable
{
    BYTE    Manufacturer;
    BYTE    ProductName;
    BYTE    Version;
    BYTE    SerialNumber;
    // Ver 2.1 beyond here
    BYTE    UUID[16];
    BYTE    Wakeup_Type;
    // Ver 2.4 beyond here
    BYTE    SKUNumber;
    BYTE    Family;
};

BYTE RawSMBIOSBaseboardInfoType = 2;

struct RawSMBIOSBaseboardInfo : public RawSMBIOSTable
{
    BYTE    Manufacturer;
    BYTE    ProductName;
    BYTE    Version;
    BYTE    SerialNumber;
    BYTE    AssetTag;
    BYTE    FeatureFlags;
    BYTE    LocationInChassis;
    WORD    ChassisHandle;
    BYTE    BoardType;
    BYTE    NumberOfContainedObjectHandles;
    WORD    ContainedObjectHandles[ANYSIZE_ARRAY];
};

BYTE RawSMBIOSProcessorInfoType = 4;

struct RawSMBIOSProcessorInfo : public RawSMBIOSTable
{
    BYTE    SocketDesignation;          //STRING
    BYTE    ProcessorType;              //ENUM
    BYTE    ProcessorFamily;            //ENUM
    BYTE    ProcessorManufacturer;      //STRING
    QWORD   ProcessorID;                //Varies
    BYTE    ProcessorVersion;           //STRING
    BYTE    Voltage;                    //Varies
    WORD    ExternalClock;              //Varies
    WORD    MaxSpeed;                   //Varies
    WORD    CurrentSpeed;               //Varies
    BYTE    Status;                     //Varies
    BYTE    ProcessorUpgrade;           //ENUM
    WORD    L1CacheHandle;              //Varies
    WORD    L2CacheHandle;              //Varies
    WORD    L3CacheHandle;              //Varies
    BYTE    SerialNumber;               //STRING
    BYTE    AssetTag;                   //STRING
    BYTE    PartNumber;                 //STRING
    BYTE    CoreCount;                  //Varies
    BYTE    CoreEnabled;                //Varies
    BYTE    ThreadCount;                //Varies
    WORD    ProcessorCharacteristics;   //BitField
    WORD    ProcessorFamily2;           //Enum
};

// This is the GUID for the USB device class
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
// (A5DCBF10-6530-11D2-901F-00C04FB951ED)
DEFINE_GUID(GUID_DEVINTERFACE_PCI_DEVICE, 0x4D36E97BL, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);

DEFINE_GUID(GUID_DEVINTERFACE_MONITOR_DEVICE, 0xe6f07b5f, 0xee97, 0x4a90, 0xb0, 0x76, 0x33, 0xf5, 0x7b, 0xf4, 0xea, 0xa7);

DevicesInfo::DevicesInfo()
{

}

void DevicesInfo::init()
{

}

void DevicesInfo::update()
{
    updateBIOSInfo();
    updateConnectedDevices();
    //updateDevicesInfo();
}

static std::string GetClassDesc(const GUID* pGuid)
{
   char buf[MAX_PATH];
   DWORD size;
   if ( SetupDiGetClassDescriptionA(pGuid, buf, sizeof(buf), &size) ) {
      return std::string(buf);
   } else {
      std::cout << "Can't get class description" << std::endl;
      return std::string("");
   }
}

DWORD
FindSmBiosTable(
    _In_ const RawSMBIOSData* SMBIOSData,
    _In_ BYTE Type,
    _Out_ RawSMBIOSTable** SMBIOSTable
    )
{
    DWORD error = S_OK;
    ULONG i = 0;
    RawSMBIOSTable* smbiosTable = NULL;
    bool properTermination = false;

    *SMBIOSTable = NULL;

    // Find SMBIOS Table
    do
    {
        properTermination = false;

        // Check that the table header fits in the buffer.
        if (i + sizeof(RawSMBIOSTable) < SMBIOSData->Length)
        {

            if (SMBIOSData->SMBIOSTableData[i] == Type)
            {
                // Found table
                smbiosTable = (RawSMBIOSTable*)&SMBIOSData->SMBIOSTableData[i];
            }

            // Set i to the end of the formated section.
            i += SMBIOSData->SMBIOSTableData[i + 1];

            // Look for the end of the struct that must be terminated by \0\0
            while (i + 1 < SMBIOSData->Length)
            {
                if (0 == SMBIOSData->SMBIOSTableData[i] &&
                    0 == SMBIOSData->SMBIOSTableData[i + 1])
                {
                    properTermination = true;
                    i += 2;
                    break;
                }

                ++i;
            }
        }
    }
    while (properTermination && !smbiosTable);

    if (properTermination) {
        if (!smbiosTable) {
            // The table was not found.
            error = ERROR_FILE_NOT_FOUND;
        }
    }
    else
    {
        // A table was not double null terminated within the buffer.
        error = ERROR_INVALID_DATA;
    }

    if (ERROR_SUCCESS == error) {
        *SMBIOSTable = smbiosTable;
    }

    return error;
}

DWORD
GetSmBiosByte(
    _In_ const RawSMBIOSTable* SmBiosTable,
    _In_ size_t Offset,
    _Out_ PBYTE* ResultByte
    )
{
    DWORD error = ERROR_SUCCESS;
    PSTR currentByte = NULL;
    BYTE currentByteIndex = 1;
    ULONG stringLen = 0;
    ULONG bufferLen = 0;
    HRESULT hr = S_OK;

    *ResultByte = NULL;

    currentByte = (PSTR)(((BYTE*)SmBiosTable) + SmBiosTable->Length);

    // find the string in the multisz string
    while (currentByteIndex < Offset)
    {
        ++currentByte;
        ++currentByteIndex;
    }

    if (!*currentByte)
    {
        // String was not found in the string table.
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    // Convert the string to UNICODE
//    stringLen = MultiByteToWideChar(
//                    CP_ACP,
//                    MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
//                    currentByte,
//                    -1,
//                    NULL,
//                    0);

    if (0 == stringLen)
    {
        error = GetLastError();
        goto exit;
    }

    // SMBIOS strings are limited to 64 characters
    if (stringLen > 65)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    hr = ULongMult(stringLen, sizeof(WCHAR), &bufferLen);
    if (hr != S_OK)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    //*ResultString = (PBYTE) HeapAlloc(GetProcessHeap(), 0, bufferLen);
    if (!*ResultByte)
    {
        error = ERROR_OUTOFMEMORY;
        goto exit;
    }

//    stringLen = MultiByteToWideChar(
//                   CP_ACP,
//                   MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
//                   currentString,
//                   -1,
//                   *ResultString,
//                   stringLen);

    if (0 == stringLen)
    {
        error = GetLastError();
    }

exit:
    if (error != ERROR_SUCCESS)
    {
        //HeapFree(GetProcessHeap(),  0, *ResultString);
        *ResultByte = NULL;
    }

    return error;
}

DWORD
GetSmBiosString(
    _In_ const RawSMBIOSTable* SmBiosTable,
    _In_ BYTE StringIndex,
    _Out_ PWSTR* ResultString
    )
{
    DWORD error = ERROR_SUCCESS;
    PSTR currentString = NULL;
    BYTE CurrentStringIndex = 1;
    ULONG stringLen = 0;
    ULONG bufferLen = 0;
    HRESULT hr = S_OK;
    PBYTE currentByte = NULL;
    *ResultString = NULL;
    uint32_t offset = 0;

    // 0 index implies the empty string
    if (StringIndex <= 0)
    {
        goto exit;
    }

    currentString = (PSTR)(((BYTE*)SmBiosTable) + SmBiosTable->Length);
    currentByte = (PBYTE)(((BYTE*)SmBiosTable) + SmBiosTable->Length + 25);

    // find the string in the multisz string
    while (*currentString)
    {
        if (CurrentStringIndex == StringIndex)
        {
            break;
        }

        ++currentString;
        ++currentByte;

        if (!*currentString)
        {
            ++currentString;
            ++currentByte;
            ++CurrentStringIndex;
        }
        ++offset;
    }

    if (!*currentString)
    {
        // String was not found in the string table.
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    // Convert the string to UNICODE
    stringLen = MultiByteToWideChar(
                    CP_ACP,
                    MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                    currentString,
                    -1,
                    NULL,
                    0);

    if (0 == stringLen)
    {
        error = GetLastError();
        goto exit;
    }

    // SMBIOS strings are limited to 64 characters
    if (stringLen > 65)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    hr = ULongMult(stringLen, sizeof(WCHAR), &bufferLen);
    if (hr != S_OK)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    *ResultString = (PWSTR) HeapAlloc(GetProcessHeap(), 0, bufferLen);
    if (!*ResultString)
    {
        error = ERROR_OUTOFMEMORY;
        goto exit;
    }

    stringLen = MultiByteToWideChar(
                   CP_ACP,
                   MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                   currentString,
                   -1,
                   *ResultString,
                   stringLen);

    if (0 == stringLen)
    {
        error = GetLastError();
    }

exit:
    if (error != ERROR_SUCCESS)
    {
        HeapFree(GetProcessHeap(),  0, *ResultString);
        *ResultString = NULL;
    }

    return error;
}


void DevicesInfo::updateBIOSInfo()
{
//    PVOID EnumACPIBuffer = NULL, EnumRSMBBuffer = NULL, EnumFIRMBuffer = NULL;
//    DWORD EnumACPIBufferSize = 0, EnumSMBIOSBufferSize = 0, EnumFIRMBufferSize = 0;

//    EnumACPIBufferSize = EnumSystemFirmwareTables('ACPI', NULL, EnumACPIBufferSize);
//    EnumSMBIOSBufferSize = EnumSystemFirmwareTables('RSMB', NULL, EnumSMBIOSBufferSize);
//    EnumFIRMBufferSize = EnumSystemFirmwareTables('FIRM', NULL, EnumFIRMBufferSize);

//    EnumACPIBuffer = malloc(EnumACPIBufferSize);
//    memset(EnumACPIBuffer, 0, EnumACPIBufferSize);
//    EnumSystemFirmwareTables('ACPI', EnumACPIBuffer, EnumACPIBufferSize);

//    EnumRSMBBuffer = malloc(EnumSMBIOSBufferSize);
//    memset(EnumRSMBBuffer, 0, EnumSMBIOSBufferSize);
//    EnumSystemFirmwareTables('ACPI', EnumRSMBBuffer, EnumSMBIOSBufferSize);

//    EnumFIRMBuffer = malloc(EnumFIRMBufferSize);
//    memset(EnumFIRMBuffer, 0, EnumFIRMBufferSize);
//    EnumSystemFirmwareTables('ACPI', EnumFIRMBuffer, EnumFIRMBufferSize);

    DWORD error = ERROR_SUCCESS;
    DWORD smBiosDataSize = 0;
    RawSMBIOSData* smBiosData = NULL;
    DWORD bytesWritten = 0;
    RawSMBIOSInfo* biosInfo = NULL;
    RawSMBIOSSystemInfo* systemInfo = NULL;
    RawSMBIOSProcessorInfo* processorInfo = NULL;

    //DWORD firmwareTableProviderSignature = 'ACPI';
    //PVOID pFirmwareTableEnumBuffer = NULL;
    //DWORD bufferSize = 0;
    //'ACPI','FIRMA','RSMB'
    //EnumSystemFirmwareTables(firmwareTableProviderSignature, pFirmwareTableEnumBuffer, bufferSize);

    //Win32_PhysicalMemory();
    // Query size of SMBIOS data.
    smBiosDataSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);

    // Allocate memory for SMBIOS data
    smBiosData = (RawSMBIOSData*) HeapAlloc(GetProcessHeap(), 0, smBiosDataSize);
    if (!smBiosData) {
        error = ERROR_OUTOFMEMORY;
        return;
    }

    // Retrieve the SMBIOS table
    bytesWritten = GetSystemFirmwareTable('RSMB', 0, smBiosData, smBiosDataSize);

    if (bytesWritten != smBiosDataSize) {
        error = ERROR_INVALID_DATA;
        return;
    }

    auto smb = reinterpret_cast<RawSMBIOSData*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytesWritten));

    error = FindSmBiosTable(smBiosData, RawSMBIOSInfoType, (RawSMBIOSTable**)&biosInfo);
    if (error == ERROR_SUCCESS)
    {
        PWSTR vendor = NULL;
        PWSTR biosVersion = NULL;
        PWSTR biosStartingAddressSegment = NULL;
        PWSTR biosReleaseDate = NULL;
        PWSTR biosROMSize = NULL;
        PWSTR systemBIOSMajorRelease = NULL;
        PWSTR systemBIOSMinorRelease = NULL;

        error = GetSmBiosString(biosInfo, biosInfo->Vendor, &vendor);
        error = GetSmBiosString(biosInfo, biosInfo->BIOSVersion, &biosVersion);
        error = GetSmBiosString(biosInfo, biosInfo->BIOSStartingAddressSegment, &biosStartingAddressSegment);
        error = GetSmBiosString(biosInfo, biosInfo->BIOSReleaseDate, &biosReleaseDate);
        error = GetSmBiosString(biosInfo, biosInfo->BIOSROMSize, &biosROMSize);
        error = GetSmBiosString(biosInfo, biosInfo->SystemBIOSMajorRelease, &systemBIOSMajorRelease);
        error = GetSmBiosString(biosInfo, biosInfo->SystemBIOSMinorRelease, &systemBIOSMinorRelease);


//        BYTE    Vendor;
//        BYTE    BIOSVersion;
//        WORD    BIOSStartingAddressSegment;
//        BYTE    BIOSReleaseDate;
//        BYTE    BIOSROMSize;
//        QWORD   BIOSCharacteristics;
//        BYTE    BIOSCharaczeristicsExtensionBytes[ANYSIZE_ARRAY];
//        BYTE    SystemBIOSMajorRelease;
//        BYTE    SystemBIOSMinorRelease;
//        BYTE    EmbeddedControllerFirmwareMajorRelease;
//        BYTE    EmbeddedControllerFirmwareMinorRelease;
    }

    error = FindSmBiosTable(smBiosData, RawSMBIOSSystemInfoType, (RawSMBIOSTable**)&systemInfo);
    if (error == ERROR_SUCCESS)
    {
        PWSTR manufacturerName = NULL;
        PWSTR productName = NULL;
        PWSTR versionName = NULL;
        PWSTR serialNumber = NULL;
        PWSTR family = NULL;

        error = GetSmBiosString(systemInfo, systemInfo->Manufacturer, &manufacturerName);
        error = GetSmBiosString(systemInfo, systemInfo->ProductName, &productName);
        error = GetSmBiosString(systemInfo, systemInfo->Version, &versionName);
        error = GetSmBiosString(systemInfo, systemInfo->SerialNumber, &serialNumber);
        error = GetSmBiosString(systemInfo, systemInfo->Family, &family);
    }

    error = FindSmBiosTable(smBiosData, RawSMBIOSProcessorInfoType, (RawSMBIOSTable**)&processorInfo);
    if (error == ERROR_SUCCESS)
    {
        PWSTR manufacturerName = NULL;
        PWSTR versionName = NULL;
        PWSTR serialNumber = NULL;
        PWSTR coreCount = NULL;
        PWSTR coreEnabled = NULL;
        PWSTR threadCount = NULL;
        PWSTR maxSpeed = NULL;
        PWSTR currentSpeed = NULL;
        PWSTR processorUpgrade;

        error = GetSmBiosString(processorInfo, processorInfo->ProcessorManufacturer, &manufacturerName);
        error = GetSmBiosString(processorInfo, processorInfo->ProcessorVersion, &versionName);
        error = GetSmBiosString(processorInfo, processorInfo->SerialNumber, &serialNumber);
        error = GetSmBiosString(processorInfo, processorInfo->CoreCount, &coreCount);
        error = GetSmBiosString(processorInfo, processorInfo->CoreEnabled, &coreEnabled);
        error = GetSmBiosString(processorInfo, processorInfo->ThreadCount, &threadCount);
        error = GetSmBiosString(processorInfo, processorInfo->MaxSpeed, &maxSpeed);
        error = GetSmBiosString(processorInfo, processorInfo->CurrentSpeed, &currentSpeed);
        error = GetSmBiosString(processorInfo, processorInfo->ProcessorUpgrade, &processorUpgrade);
        std::cout<< std::endl;
    }

    std::cout<< std::endl;

//    struct RawSMBIOSSystemInfo : public RawSMBIOSTable
//    {
//        BYTE    Manufacturer;
//        BYTE    ProductName;
//        BYTE    Version;
//        BYTE    SerialNumber;
//        // Ver 2.1 beyond here
//        BYTE    UUID[16];
//        BYTE    Wakeup_Type;
//        // Ver 2.4 beyond here
//        BYTE    SKUNumber;
//        BYTE    Family;
//    };

//    if (manufacturerName != NULL)
//    {
//        if (Size <  (DWORD) wcslen(manufacturerName))
//        {
//            error = ERROR_BUFFER_OVERFLOW;
//            goto exit;
//        }

//        wcscpy_s(ManufacturerName, Size, manufacturerName);
//        HeapFree(GetProcessHeap(), 0, manufacturerName);
//    }
//    else
//    {
//        error = E_UNEXPECTED;
//    }
}

void DevicesInfo::updateConnectedDevices()
{
    HDEVINFO                         devInfoHandle;
    SP_DEVICE_INTERFACE_DATA         devIntData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA devIntDetailData;
    SP_DEVINFO_DATA                  devData;

    devInfoHandle = SetupDiGetClassDevs( NULL, NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_ALLCLASSES | DIGCF_PRESENT );
    if ( devInfoHandle == INVALID_HANDLE_VALUE ) {
        //qCritical() << "Failed to get handle for  GUID_DEVINTERFACE_USB_DEVICE";
        return;
    }

    devIntData.cbSize = sizeof(devIntData);
    DWORD index = 0;

    SetupDiEnumDeviceInterfaces(devInfoHandle, NULL, &GUID_DEVINTERFACE_MONITOR_DEVICE, index, &devIntData);
    while( GetLastError() != ERROR_NO_MORE_ITEMS )
    {
        //qInfo() << "Found interface " << index;

        DWORD detail_size;
        devData.cbSize = sizeof(devData);
        SetupDiGetDeviceInterfaceDetail(devInfoHandle, &devIntData, NULL, 0, &detail_size, NULL);

        devIntDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, detail_size);
        devIntDetailData->cbSize = sizeof(*devIntDetailData);

        if ( SetupDiGetDeviceInterfaceDetail(devInfoHandle, &devIntData, devIntDetailData, detail_size, &detail_size, &devData)) {
            WCHAR bufFriendlyName[1024];
            WCHAR bufDeviceDesc[1024];
            WCHAR bufDeviceObjectName[1024];
            std::string friendlyName = "";
            std::string deviceDesc = "";
            std::string deviceObjectName = "";
            //DWORD nSize = 0;

            if ( SetupDiGetDeviceRegistryProperty(devInfoHandle, &devData,
               SPDRP_FRIENDLYNAME, NULL, (PBYTE)bufFriendlyName, 2048, 0) ) {
                std::wstring ws( bufFriendlyName );
                friendlyName = std::string( ws.begin(), ws.end() );
                //friendlyName = QString::fromWCharArray(bufFriendlyName);
            }
            if ( SetupDiGetDeviceRegistryProperty(devInfoHandle, &devData,
               SPDRP_DEVICEDESC, NULL, (PBYTE)bufDeviceDesc, 2048, 0) ) {
                std::wstring ws( bufDeviceDesc );
                deviceDesc = std::string( ws.begin(), ws.end() );
                //deviceDesc = QString::fromWCharArray(bufDeviceDesc);
            }
            if ( SetupDiGetDeviceRegistryProperty(devInfoHandle, &devData,
               SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (PBYTE)bufDeviceObjectName, 2048, 0) ) {
                std::wstring ws( bufDeviceObjectName );
                deviceObjectName = std::string( ws.begin(), ws.end() );
                //deviceObjectName = QString::fromWCharArray(bufDeviceObjectName);
            }

            //qInfo() << "Detail: " << devIntDetailData->DevicePath << " classDesc: " << GetClassDesc(&(devData.ClassGuid)).c_str()
            //        << " friendlyName: " << friendlyName << " deviceDesc: " << deviceDesc << " deviceObjectName: " << deviceObjectName;
            //devIntDetailData->
        } else {
            //qCritical() << "Failed to get detail info for index " << index;
        }

        HeapFree(GetProcessHeap(), 0, devIntDetailData);

        SetupDiEnumDeviceInterfaces(devInfoHandle, NULL, &GUID_DEVINTERFACE_USB_DEVICE, ++index, &devIntData);
    }
}

void DevicesInfo::updateDevicesInfo()
{
    HDEVINFO                         hDevInfo;
    SP_DEVICE_INTERFACE_DATA         DevIntfData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA DevIntfDetailData;
    SP_DEVINFO_DATA                  DevData;

    DWORD dwSize, dwType, dwMemberIdx;
    HKEY hKey;
    BYTE lpData[1024];

    // We will try to get device information set for all USB devices that have a
    // device interface and are currently present on the system (plugged in).
    hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_USB_DEVICE, NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (hDevInfo != INVALID_HANDLE_VALUE)
    {
        // Prepare to enumerate all device interfaces for the device information
        // set that we retrieved with SetupDiGetClassDevs(..)
        DevIntfData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        dwMemberIdx = 0;

        // Next, we will keep calling this SetupDiEnumDeviceInterfaces(..) until this
        // function causes GetLastError() to return  ERROR_NO_MORE_ITEMS. With each
        // call the dwMemberIdx value needs to be incremented to retrieve the next
        // device interface information.

        SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_USB_DEVICE,
            dwMemberIdx, &DevIntfData);

        while (GetLastError() != ERROR_NO_MORE_ITEMS)
        {

            // As a last step we will need to get some more details for each
            // of device interface information we are able to retrieve. This
            // device interface detail gives us the information we need to identify
            // the device (VID/PID), and decide if it's useful to us. It will also
            // provide a DEVINFO_DATA structure which we can use to know the serial
            // port name for a virtual com port.

            DevData.cbSize = sizeof(DevData);

            // Get the required buffer size. Call SetupDiGetDeviceInterfaceDetail with
            // a NULL DevIntfDetailData pointer, a DevIntfDetailDataSize
            // of zero, and a valid RequiredSize variable. In response to such a call,
            // this function returns the required buffer size at dwSize.

            SetupDiGetDeviceInterfaceDetail(
                hDevInfo, &DevIntfData, NULL, 0, &dwSize, NULL);

            // Allocate memory for the DeviceInterfaceDetail struct. Don't forget to
            // deallocate it later!
            DevIntfDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
            DevIntfDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevIntfData,
                DevIntfDetailData, dwSize, &dwSize, &DevData))
            {
                // Finally we can start checking if we've found a useable device,
                // by inspecting the DevIntfDetailData->DevicePath variable.
                // The DevicePath looks something like this:
                //
                // \\?\usb#vid_04d8&pid_0033#5&19f2438f&0&2#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
                //
                // The VID for Velleman Projects is always 10cf. The PID is variable
                // for each device:
                //
                //    -------------------
                //    | Device   | PID  |
                //    -------------------
                //    | K8090    | 8090 |
                //    | VMB1USB  | 0b1b |
                //    -------------------
                //
                // As you can see it contains the VID/PID for the device, so we can check
                // for the right VID/PID with string handling routines.
//                auto VID_PID_string = _T("vid_aaaa&pid_8816");

//                //here we detect the connection
//                if (NULL != _tcsstr((TCHAR*)DevIntfDetailData->DevicePath, VID_PID_string))
//                {
//                    printf("device connected\n");
//                    return;
//                }
            }
            HeapFree(GetProcessHeap(), 0, DevIntfDetailData);
            // Continue looping
            SetupDiEnumDeviceInterfaces(
                hDevInfo, NULL, &GUID_DEVINTERFACE_USB_DEVICE, ++dwMemberIdx, &DevIntfData);
        }

        SetupDiDestroyDeviceInfoList(hDevInfo);
    }
    else
    {
        //qDebug() << "SetupDiGetClassDevs failed";
    }
}
