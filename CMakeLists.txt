cmake_minimum_required(VERSION 3.20)

project(sgc_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
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
    src/win32_helpers/file_helpers.cpp

    src/win32_models/layer_list_control.cpp    

    src/win32_program/windows_init.cpp    
    src/win32_program/windows_process.cpp
    src/win32_program/layout_manager.cpp

    src/program/program.cpp
    src/program/layer_manager.cpp
    src/program/editor_mode.cpp
    src/program/editor_update.cpp

    src/sections/section.cpp
    src/sections/tileset_section.cpp
    src/sections/map_section.cpp
    src/sections/toolbar_section.cpp
    src/sections/menu_section.cpp
    src/sections/layers_section.cpp
    src/sections/package_section.cpp

    src/sgc_view/sgc_view.cpp    
    src/sgc_view/tileset_view.cpp
    src/sgc_view/map_view.cpp    

    rcs/new_file_dialog.rc
    rcs/new_map_dialog.rc
    rcs/new_tileset_dialog.rc

    src/debug.cpp

    src/locale/command_lookup.cpp    
    src/locale/string_lookup.cpp
    
    src/command/command_manager.cpp
    src/command/paint_tiles_command.cpp
    src/command/paint_stroke_command.cpp
    src/command/layer_add_command.cpp
    src/command/layer_remove_command.cpp
    src/command/layer_move_command.cpp
    
    src/action/action.cpp
    src/action/action_manager.cpp
    src/action/new_file_action.cpp
    src/action/open_file_action.cpp
    src/action/save_file_action.cpp
    src/action/close_file_action.cpp
    src/action/undo_action.cpp
    src/action/redo_action.cpp
    src/action/layer_add_action.cpp
    src/action/layer_remove_action.cpp
    src/action/layer_move_action.cpp
    src/action/change_layer_mode_action.cpp
    src/action/change_chunk_mode_action.cpp
    src/action/popup_menu_action.cpp
    src/action/file_menu_action.cpp
    src/action/edit_menu_action.cpp
    src/action/map_menu_action.cpp
    src/action/view_menu_action.cpp
    src/action/help_menu_action.cpp    

    src/file/map_file.cpp
    src/file/map_document.cpp
    src/file/asset_manager.cpp
    src/file/file_manager.cpp
    src/file/tileset_file.cpp
    src/file/tileset_document.cpp
)

# embed manifest in the exe
target_sources(sgc-editor PRIVATE ${APP_MANIFEST})
set_source_files_properties(${APP_MANIFEST} PROPERTIES
    VS_TOOL_OVERRIDE "EmbeddedResource"
)

target_include_directories(sgc-editor PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/rcs
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
    sdl-game-core
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
