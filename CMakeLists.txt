cmake_minimum_required(VERSION 3.5)

project(QtTaskManager VERSION 0.1 LANGUAGES CXX)

set(RYZEN_MASTER_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/RyzenMasterMonitoringSDK" CACHE PATH "Path to Downloaded Ryzen Master Monitoring SDK")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts Concurrent OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts Concurrent OpenGLWidgets)

find_library(NVML_LIBRARY
    NAMES libnvidia-ml nvidia-ml
    PATHS /usr/lib
          /usr/lib64
          /usr/local/lib
          /usr/local/lib64
)

if(NVML_LIBRARY)
    set(NVML_FOUND ${NVML_LIBRARY})
endif(NVML_LIBRARY)

add_subdirectory(external)

set(PROJECT_SOURCES
    src/Globals.h

    src/Utils.h

    src/main/BaseInfo.h
    src/main/BaseInfo.cpp

    src/cpu/CpuInfo.h
    src/cpu/CpuInfo.cpp

    src/cpu/amd/CpuInfoAmd.h
    src/cpu/amd/CpuInfoAmd.cpp
    src/cpu/intel/CpuInfoIntel.h
    src/cpu/intel/CpuInfoIntel.cpp

    src/gpu/GlGlobals.h
    src/gpu/GpuInfo.h
    src/gpu/GpuInfo.cpp

    src/gpu/amd/GpuInfoAmd.h
    src/gpu/amd/GpuInfoAmd.cpp
    src/gpu/intel/GpuInfoIntel.h
    src/gpu/intel/GpuInfoIntel.cpp
    src/gpu/intel/ZeroLevelHandler.h
    src/gpu/intel/ZeroLevelHandler.cpp
    src/gpu/nvidia/GpuInfoNVidia.h
    src/gpu/nvidia/GpuInfoNVidia.cpp

    src/memory/MemoryInfo.h
    src/memory/MemoryInfo.cpp

    src/network/NetworkInfo.h
    src/network/NetworkInfo.cpp

    src/process/ProcessInfo.h
    src/process/ProcessInfo.cpp

    src/tabs/TabHardware.cpp
    src/tabs/TabHardware.h
    src/tabs/TabProcesses.cpp
    src/tabs/TabProcesses.h
    src/tabs/TabPerformance.cpp
    src/tabs/TabPerformance.h
    src/tabs/TabBenchmark.cpp
    src/tabs/TabBenchmark.h
    src/tabs/TabApiSupport.cpp
    src/tabs/TabApiSupport.h
    src/tabs/TabSystemInfo.cpp
    src/tabs/TabSystemInfo.h
    
    src/system/SystemInfo.h
    src/system/SystemInfo.cpp

    #src/DevicesInfo.h
    #src/DevicesInfo.cpp

    src/SystemManager.cpp
    src/SystemManager.h
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/Worker.h
    src/Worker.cpp
    src/GlWidget.cpp
	src/GlWidget.h
    src/GlWindow.cpp
	src/GlWindow.h
    src/MainWindow.ui
)

if (WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES}

        src/cpu/amd/RyzenMasterSdkHandler.h
        src/cpu/amd/RyzenMasterSdkHandler.cpp
        src/cpu/intel/PcmHandler.h
        src/cpu/intel/PcmHandler.cpp
        src/cpu/windows/CpuInfoWindows.h
        src/cpu/windows/CpuInfoWindows.cpp

        src/gpu/amd/AdlxHandler.h
        src/gpu/amd/AdlxHandler.cpp
        src/gpu/amd/AgsHandler.h
        src/gpu/amd/AgsHandler.cpp

        src/gpu/intel/IgclHandler.h
        src/gpu/intel/IgclHandler.cpp
        src/gpu/intel/GenericIGCLApp.h
        src/gpu/intel/ZeroLevelHandler.h
        src/gpu/intel/ZeroLevelHandler.cpp

        src/gpu/nvidia/NvapiHandler.h
        src/gpu/nvidia/NvapiHandler.cpp
        src/gpu/nvidia/NvmlHandler.h
        src/gpu/nvidia/NvmlHandler.cpp

        src/process/windows/ProcessInfoWindows.h
        src/process/windows/ProcessInfoWindows.cpp

        src/system/SystemInfoWindows.h
        src/system/SystemInfoWindows.cpp

        src/windows/DxHandler.h
        src/windows/DxHandler.cpp
        src/windows/WmiInfo.h
        src/windows/WmiInfo.cpp

        external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h
        external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.cpp
        external/ADLX/SDK/platform/Windows/WinAPIs.cpp

        #external/drivers.gpu.control-library/Source/GenericIGCLApp.h
        external/drivers.gpu.control-library/Source/cApiWrapper.cpp
    )
else ()
    set(PROJECT_SOURCES ${PROJECT_SOURCES}
        src/cpu/linux/CpuInfoLinux.h
        src/cpu/linux/CpuInfoLinux.cpp

        src/gpu/linux/GpuInfoLinux.h
        src/gpu/linux/GpuInfoLinux.cpp

        src/system/SystemInfoLinux.h
        src/system/SystemInfoLinux.cpp
    )
endif (WIN32)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QtTaskManager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# if user wants to get CPU temperature from AMD CPU, detect AMD Ryzen Master SDK (Cannot download from Github because EULA)
if(EXISTS "${RYZEN_MASTER_SDK_PATH}")
    if(NOT CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "(AMD|x86_)64")
        message (FATAL_ERROR "AMD Ryzen Master SDK only supports 64-bit system!")
    endif()
    add_custom_command (TARGET ${PROJECT_NAME} PRE_BUILD
    #OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Platform.dll
    COMMAND ${CMAKE_COMMAND} -E copy
            ${RYZEN_MASTER_SDK_PATH}/bin/Platform.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/.)
    add_custom_command (TARGET ${PROJECT_NAME} PRE_BUILD
    #OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Device.dll
    COMMAND ${CMAKE_COMMAND} -E copy
            ${RYZEN_MASTER_SDK_PATH}/bin/Device.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/.)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${RYZEN_MASTER_SDK_PATH}/lib/Device.lib 
            ${RYZEN_MASTER_SDK_PATH}/lib/Platform.lib)
    add_definitions ( -DHAS_RYZEN_MASTER_SDK )
else()
    message (WARNING "If you want to read Ryzen Cpu Infos you must download Ryzen Master SDK & copy RyzenMasterMonitoringSDK to external!")
endif()


    target_include_directories(${PROJECT_NAME} PRIVATE
        external/pcm/src
    )
if (WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE
        external/AGS_SDK/ags_lib/inc
        external/ADLX/SDK/Include
        external/nvapi
        external/nvml
        external/drivers.gpu.control-library/include/
        external/drivers.gpu.control-library/Samples/inc
        external/level-zero/include
        #external/level-zero/samples/include
        external/DirectX-Headers/include
    )
else()
    target_include_directories(${PROJECT_NAME} PRIVATE
        external/level-zero/include
        external/level-zero/samples/include
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Opengl32
        pdh
        Setupapi
        debug ${CMAKE_CURRENT_SOURCE_DIR}/external/AGS_SDK/ags_lib/lib/amd_ags_x64_2022_MDd.lib
        optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/AGS_SDK/ags_lib/lib/amd_ags_x64_2022_MD.lib
        pcm-lib
        ze_loader
        d3d12.lib
        dxguid.lib
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ze_loader
    )
    if(NVML_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${NVML_LIBRARY}
        )
    endif(NVML_FOUND)
endif (WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif (WIN32)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
