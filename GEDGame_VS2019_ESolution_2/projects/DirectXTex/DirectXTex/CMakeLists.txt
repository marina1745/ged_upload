project(DirectXTex CXX)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "BC.cpp"
    "BC4BC5.cpp"
    "BC6HBC7.cpp"
    "BCDirectCompute.cpp"
    "BCDirectCompute.h"
    "DirectXTexCompress.cpp"
    "DirectXTexCompressGPU.cpp"
    "DirectXTexConvert.cpp"
    "DirectXTexD3D11.cpp"
    "DirectXTexDDS.cpp"
    "DirectXTexFlipRotate.cpp"
    "DirectXTexHDR.cpp"
    "DirectXTexImage.cpp"
    "DirectXTexMipmaps.cpp"
    "DirectXTexMisc.cpp"
    "DirectXTexNormalMaps.cpp"
    "DirectXTexPMAlpha.cpp"
    "DirectXTexResize.cpp"
    "DirectXTexTGA.cpp"
    "DirectXTexUtil.cpp"
    "DirectXTexWIC.cpp"
    "filters.h"
    "Shaders/BC6HEncode.hlsl"
    "Shaders/BC7Encode.hlsl"
    "Shaders/Compiled/BC6HEncode_EncodeBlockCS.inc"
    "Shaders/Compiled/BC6HEncode_EncodeBlockCS.pdb"
    "Shaders/Compiled/BC6HEncode_TryModeG10CS.inc"
    "Shaders/Compiled/BC6HEncode_TryModeG10CS.pdb"
    "Shaders/Compiled/BC6HEncode_TryModeLE10CS.inc"
    "Shaders/Compiled/BC6HEncode_TryModeLE10CS.pdb"
    "Shaders/Compiled/BC7Encode_EncodeBlockCS.inc"
    "Shaders/Compiled/BC7Encode_EncodeBlockCS.pdb"
    "Shaders/Compiled/BC7Encode_TryMode02CS.inc"
    "Shaders/Compiled/BC7Encode_TryMode02CS.pdb"
    "Shaders/Compiled/BC7Encode_TryMode137CS.inc"
    "Shaders/Compiled/BC7Encode_TryMode137CS.pdb"
    "Shaders/Compiled/BC7Encode_TryMode456CS.inc"
    "Shaders/Compiled/BC7Encode_TryMode456CS.pdb"
    "Shaders/CompileShaders.cmd"
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

add_precompiled_header(${PROJECT_NAME} "DirectXTexP.h" ".")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE DirectXTex)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "DirectXTex"
        TARGET_NAME_PROFILE "DirectXTex"
        TARGET_NAME_RELEASE "DirectXTex"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/Bin/Desktop_2019/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_PROFILE "${CMAKE_CURRENT_SOURCE_DIR}/Bin/Desktop_2019/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/Bin/Desktop_2019/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "PROFILE"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_UNICODE;"
        "UNICODE;"
        "WIN32;"
        "_LIB;"
        "_WIN7_PLATFORM_UPDATE;"
        "_WIN32_WINNT=0x0601;"
        "_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS"
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
            /sdl;
            /arch:SSE2;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /fp:fast;
            /Zc:twoPhase-;
            /Zc:__cplusplus;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /openmp
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

