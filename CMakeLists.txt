cmake_minimum_required(VERSION 3.20)

project(sgc_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SGC_INCLUDE_PATH $ENV{SGC_INCLUDE_PATH})
set(SGC_LIB_PATH $ENV{SGC_LIB_PATH})
set(SDL_INCLUDE_PATH $ENV{SDL_INCLUDE_PATH})
set(SDL_LIB_PATH $ENV{SDL_LIB_PATH})
set(SDL_IMAGE_LIB_PATH $ENV{SDL_IMAGE_LIB_PATH})
set(SDL_TTF_LIB_PATH $ENV{SDL_TTF_LIB_PATH})

if(NOT SGC_INCLUDE_PATH)
    message(FATAL_ERROR "SGC_INCLUDE_PATH environment variable is not set")
endif()

if(NOT SGC_LIB_PATH)
    message(FATAL_ERROR "SGC_LIB_PATH environment variable is not set")
endif()

if(NOT SDL_INCLUDE_PATH)
    message(FATAL_ERROR "SDL_INCLUDE_PATH environment variable is not set")
endif()

if(NOT SDL_LIB_PATH)
    message(FATAL_ERROR "SDL_LIB_PATH environment variable is not set")
endif()

if(NOT SDL_IMAGE_LIB_PATH)
    message(FATAL_ERROR "SDL_IMAGE_LIB_PATH environment variable is not set")
endif()

if(NOT SDL_TTF_LIB_PATH)
    message(FATAL_ERROR "SDL_TTF_LIB_PATH environment variable is not set")
endif()

set(APP_MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/app.manifest")

add_executable(sgc-editor WIN32    
    src/main.cpp

    src/win32_helpers/create_helpers.cpp
    src/win32_helpers/load_bitmap.cpp

    src/win32_program/windows_init.cpp
    src/win32_program/control_setup.cpp
)

# embed manifest in the exe
target_sources(sgc-editor PRIVATE ${APP_MANIFEST})
set_source_files_properties(${APP_MANIFEST} PROPERTIES
    VS_TOOL_OVERRIDE "EmbeddedResource"
)

target_include_directories(sgc-editor PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${SDL_INCLUDE_PATH}
    ${SGC_INCLUDE_PATH}
)

target_link_directories(sgc-editor PRIVATE
    ${SDL_IMAGE_LIB_PATH}
    ${SDL_LIB_PATH}
    ${SDL_TTF_LIB_PATH}
    ${SGC_LIB_PATH}
)

target_link_libraries(sgc-editor PRIVATE
    Comctl32
    Comdlg32
    windowscodecs
    SDL3
    SDL3_image
    SDL3_ttf
    sdl_game_core
)

target_compile_definitions(sgc-editor PRIVATE
    NOMINMAX
    SDL_MAIN_HANDLED
    UNICODE
    WIN32_LEAN_AND_MEAN
    _UNICODE
)

if(MSVC)
    target_compile_options(sgc-editor PRIVATE /W4 /permissive-)
endif()

file(GLOB RUNTIME_DLLS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/dlls/*.dll"
)

if(RUNTIME_DLLS)
    foreach(dll ${RUNTIME_DLLS})
        add_custom_command(TARGET sgc-editor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll}"
                "$<TARGET_FILE_DIR:sgc-editor>"
            VERBATIM
        )
    endforeach()
endif()
