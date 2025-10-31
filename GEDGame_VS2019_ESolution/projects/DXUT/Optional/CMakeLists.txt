project(DXUTOpt CXX)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "DXUTcamera.cpp"
    "DXUTgui.cpp"
    "DXUTguiIME.cpp"
    "DXUTres.cpp"
    "DXUTsettingsdlg.cpp"
    "ImeUi.cpp"
    "SDKmesh.cpp"
    "SDKmisc.cpp"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

add_precompiled_header(${PROJECT_NAME} "DXUT.h" ".")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE DXUTOpt)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "DXUTOpt"
        TARGET_NAME_PROFILE "DXUTOpt"
        TARGET_NAME_RELEASE "DXUTOpt"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/Bin/Desktop_2019_Win10/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_PROFILE "${CMAKE_CURRENT_SOURCE_DIR}/Bin/Desktop_2019_Win10/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/Bin/Desktop_2019_Win10/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_PROFILE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Core"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "DEBUG;"
            "PROFILE"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "PROFILE"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_WINDOWS;"
        "_LIB;"
        "USE_DIRECT3D11_4;"
        "_WIN32_WINNT=0x0600;"
        "_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            ${DEFAULT_CXX_RUNTIME_LIBRARY};
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /MDd
            >
            $<$<CONFIG:Profile>:
                /O2;
                ${DEFAULT_CXX_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /O2;
                ${DEFAULT_CXX_RUNTIME_LIBRARY}
            >
            /permissive-;
            /arch:SSE2;
            /W4;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /fp:fast;
            /Zc:__cplusplus;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            /DEBUG:FULL
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Profile>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /MANIFEST;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

