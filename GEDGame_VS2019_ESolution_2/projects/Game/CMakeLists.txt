project(Game CXX)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "game.cfg"
    "ReadMe.txt"
)
source_group("" FILES ${no_group_source_files})

set(Source
    "src/ConfigParser.cpp"
    "src/ConfigParser.h"
    "src/debug.h"
    "src/Game.cpp"
    "src/GameEffect.h"
    "src/GameObject.h"
    "src/Mesh.cpp"
    "src/Mesh.h"
    "src/Particle.h"
    "src/SpriteRenderer.cpp"
    "src/SpriteRenderer.h"
    "src/T3d.cpp"
    "src/T3d.h"
    "src/Terrain.cpp"
    "src/Terrain.h"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${no_group_source_files}
    ${Source}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Game")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Game)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_PROFILE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_PROFILE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_PROFILE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Tools/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/Effects11/inc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DXUT/Core;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DXUT/Optional;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DirectXTex/DirectXTex;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DirectXTex/WICTextureLoader;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DirectXTex/DDSTextureLoader"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Tools/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/Effects11/inc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DXUT/Core;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DXUT/Optional;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DirectXTex/DirectXTex;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DirectXTex/WICTextureLoader;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../GEDGame_VS2019_ESolutionprojects/DirectXTex/DDSTextureLoader"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_WIN32_WINNT=0x600;"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "_WIN32_WINNT=0x600;"
            "WIN32;"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_WIN32_WINNT=0x600;"
        "WIN32;"
        "_CONSOLE;"
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
            $<$<CONFIG:Debug>:
                /Od;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Profile>:
                /Od;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> robocopy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>projects\\DXUT\\Media" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\Media" * /E /XX /NJH /NJS /NP
        COMMAND   $<CONFIG:Debug> robocopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>." "$<SHELL_PATH:${OUTPUT_DIRECTORY}>." game.cfg /XX /NJH /NJS /NP
        COMMAND   $<CONFIG:Debug> if errorlevel 4 goto error
        COMMAND   $<CONFIG:Debug> exit 0
        COMMAND   $<CONFIG:Debug> :error
        COMMAND   $<CONFIG:Debug> exit 1
        COMMAND $<CONFIG:Profile> robocopy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>projects\\DXUT\\Media" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\Media" * /E /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Profile> robocopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>." "$<SHELL_PATH:${OUTPUT_DIRECTORY}>." game.cfg /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Profile> if errorlevel 4 goto error
        COMMAND $<CONFIG:Profile> exit 0
        COMMAND $<CONFIG:Profile> :error
        COMMAND $<CONFIG:Profile> exit 1
        COMMAND $<CONFIG:Release> robocopy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>projects\\DXUT\\Media" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\Media" * /E /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Release> robocopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>." "$<SHELL_PATH:${OUTPUT_DIRECTORY}>." game.cfg /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Release> if errorlevel 4 goto error
        COMMAND $<CONFIG:Release> exit 0
        COMMAND $<CONFIG:Release> :error
        COMMAND $<CONFIG:Release> exit 1
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> robocopy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>projects\\DXUT\\Media" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\Media" * /E /XX /NJH /NJS /NP
        COMMAND   $<CONFIG:Debug> robocopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>." "$<SHELL_PATH:${OUTPUT_DIRECTORY}>." game.cfg /XX /NJH /NJS /NP
        COMMAND   $<CONFIG:Debug> if errorlevel 4 goto error
        COMMAND   $<CONFIG:Debug> exit 0
        COMMAND   $<CONFIG:Debug> :error
        COMMAND   $<CONFIG:Debug> exit 1
        COMMAND $<CONFIG:Profile> robocopy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>projects\\DXUT\\Media" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\Media" * /E /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Profile> robocopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>." "$<SHELL_PATH:${OUTPUT_DIRECTORY}>." game.cfg /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Profile> if errorlevel 4 goto error
        COMMAND $<CONFIG:Profile> exit 0
        COMMAND $<CONFIG:Profile> :error
        COMMAND $<CONFIG:Profile> exit 1
        COMMAND $<CONFIG:Release> robocopy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>projects\\DXUT\\Media" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\Media" * /E /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Release> robocopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>." "$<SHELL_PATH:${OUTPUT_DIRECTORY}>." game.cfg /XX /NJH /NJS /NP
        COMMAND $<CONFIG:Release> if errorlevel 4 goto error
        COMMAND $<CONFIG:Release> exit 0
        COMMAND $<CONFIG:Release> :error
        COMMAND $<CONFIG:Release> exit 1
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    DirectXTex
    texconv
    DXUT
    DXUTOpt
    Effects11
    ResourceGenerator
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    DirectXTex
    texconv
    DXUT
    DXUTOpt
    Effects11
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "GEDUtilsd"
        ">"
        "$<$<CONFIG:Profile>:"
            "kernel32;"
            "gdi32;"
            "GEDUtils"
        ">"
        "$<$<CONFIG:Release>:"
            "kernel32;"
            "gdi32;"
            "GEDUtils"
        ">"
        "usp10;"
        "comctl32;"
        "dxguid;"
        "d3dcompiler;"
        "comdlg32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "GEDUtilsd"
        ">"
        "$<$<CONFIG:Profile>:"
            "GEDUtilsd"
        ">"
        "$<$<CONFIG:Release>:"
            "kernel32;"
            "gdi32;"
            "GEDUtils"
        ">"
        "usp10;"
        "comctl32;"
        "dxguid;"
        "d3dcompiler;"
        "comdlg32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "$ENV{UniversalCRT_LibraryPath_x64};"
            "$ENV{VC_LibraryPath_x64}"
        ">"
        "$<$<CONFIG:Profile>:"
            "$ENV{UniversalCRT_LibraryPath_x86}"
        ">"
        "$<$<CONFIG:Release>:"
            "$ENV{UniversalCRT_LibraryPath_x64};"
            "$ENV{VC_LibraryPath_x64}"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/Tools/lib/VS2019/${CMAKE_VS_PLATFORM_NAME}/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$ENV{UniversalCRT_LibraryPath_x86};"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/Tools/lib/VS2019/${CMAKE_VS_PLATFORM_NAME}/"
    )
endif()

