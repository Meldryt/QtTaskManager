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

set(PROJECT_SOURCES
    src/Globals.h
    src/AdlxManager.h
    src/AdlxManager.cpp
    src/CpuInfo.h
    src/CpuInfo.cpp
    src/DevicesInfo.h
    src/DevicesInfo.cpp
    src/GpuInfo.h
    src/GpuInfo.cpp
    src/GpuInfoNVidia.h
    src/GpuInfoNVidia.cpp
    src/GpuInfoAmd.h
    src/GpuInfoAmd.cpp
    src/MemoryInfo.h
    src/MemoryInfo.cpp
    src/ProcessInfo.h
    src/ProcessInfo.cpp
    src/SystemManager.cpp
    src/SystemManager.h
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/TabHardware.cpp
    src/TabHardware.h
    src/TabProcesses.cpp
    src/TabProcesses.h
    src/TabPerformance.cpp
    src/TabPerformance.h
    src/TabBenchmark.cpp
    src/TabBenchmark.h
    src/ProcessWorker.cpp
    src/ProcessWorker.h
    src/CpuWorker.cpp
    src/CpuWorker.h
    src/GpuWorker.cpp
    src/GpuWorker.h
    src/MemoryWorker.cpp
    src/MemoryWorker.h
    src/Worker.h
    src/Worker.cpp
    src/GlWidget.cpp
	src/GlWidget.h
    src/GlWindow.cpp
	src/GlWindow.h
    src/MainWindow.ui
    src/WmiManager.h
    src/WmiManager.cpp

    external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h
    external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.cpp
    external/ADLX/SDK/platform/Windows/WinAPIs.cpp
)

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
    external/AGS_SDK/ags_lib/inc
    external/nvapi
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Opengl32
    pdh
    Setupapi
    debug ${CMAKE_CURRENT_SOURCE_DIR}/external/AGS_SDK/ags_lib/lib/amd_ags_x64_2022_MDd.lib
    optimized ${CMAKE_CURRENT_SOURCE_DIR}/external/AGS_SDK/ags_lib/lib/amd_ags_x64_2022_MD.lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
