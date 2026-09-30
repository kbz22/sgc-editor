cmake_minimum_required(VERSION 3.20)

project(sgc_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

set(SGC_INCLUDE_PATH "$ENV{SGC_INCLUDE_PATH}" CACHE PATH "Path to SGC include directory")
set(SGC_LIB_PATH "$ENV{SGC_LIB_PATH}" CACHE PATH "Path to SGC library directory")
set(SDL_INCLUDE_PATH "$ENV{SDL_INCLUDE_PATH}" CACHE PATH "Path to SDL include directory")
set(SDL_LIB_PATH "$ENV{SDL_LIB_PATH}" CACHE PATH "Path to SDL library directory")
set(SDL_IMAGE_LIB_PATH "$ENV{SDL_IMAGE_LIB_PATH}" CACHE PATH "Path to SDL_image library directory")

if(NOT SGC_INCLUDE_PATH)
    message(FATAL_ERROR "SGC_INCLUDE_PATH not specified.")
endif()

if(NOT SGC_LIB_PATH)
    message(FATAL_ERROR "SGC_LIB_PATH not specified.")
endif()

if(NOT SDL_INCLUDE_PATH)
    message(FATAL_ERROR "SDL_INCLUDE_PATH not specified.")
endif()

if(NOT SDL_LIB_PATH)
    message(FATAL_ERROR "SDL_LIB_PATH not specified.")
endif()

if(NOT SDL_IMAGE_LIB_PATH)
    message(FATAL_ERROR "SDL_IMAGE_LIB_PATH not specified.")
endif()

set(APP_MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/app.manifest")

add_executable(sgc-editor WIN32    
    src/main.cpp

    src/win32_helpers/create_helpers.cpp
    src/win32_helpers/load_bitmap.cpp
    src/win32_helpers/file_helpers.cpp

    src/win32_models/layer_list_control.cpp
    src/win32_models/zoom_combo_box.cpp
    src/win32_models/edit_text_box.cpp

    src/win32_program/windows_init.cpp    
    src/win32_program/windows_process.cpp
    src/win32_program/layout_manager.cpp
    src/win32_program/shortcut_manager.cpp
    src/win32_program/map_selection_serializer.cpp    

    src/program/program.cpp
    src/program/layer_manager.cpp
    src/program/editor_mode.cpp
    src/program/editor_update.cpp
    src/program/session.cpp

    src/sections/section.cpp
    src/sections/tileset_section.cpp
    src/sections/map_section.cpp
    src/sections/toolbar_section.cpp
    src/sections/menu_section.cpp
    src/sections/layers_section.cpp
    src/sections/package_section.cpp
    src/sections/status_section.cpp

    src/sgc_view/sgc_view.cpp    
    src/sgc_view/tileset_view.cpp
    src/sgc_view/map_view.cpp    
    
    rcs/new_map_dialog.rc
    rcs/new_tileset_dialog.rc
    rcs/editor_graphics.rc
    rcs/new_package_dialog.rc
    rcs/settings_dialog.rc
    rcs/settings_pages.rc

    src/debug.cpp

    src/locale/string_lookup.cpp
    src/locale/shortcut_to_string.cpp
    
    src/command/command_manager.cpp    
    src/command/paint_command.cpp
    src/command/layer_add_command.cpp
    src/command/layer_remove_command.cpp
    src/command/layer_move_command.cpp
    src/command/delete_chunk_command.cpp
    src/command/paint_selection_command.cpp
    
    src/action/action.cpp
    src/action/action_manager.cpp
    src/action/new_document_action.cpp
    src/action/new_map_document_action.cpp
    src/action/new_tileset_document_action.cpp
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
    src/action/change_brush_mode_action.cpp
    src/action/widget_action.cpp
    src/action/zoom_action.cpp
    src/action/zoom_reset_action.cpp
    src/action/change_brush_erase_mode_action.cpp
    src/action/zoom_select_action.cpp
    src/action/change_grid_mode_action.cpp
    src/action/change_selection_mode_action.cpp
    src/action/selection_clear_action.cpp
    src/action/selection_copy_action.cpp
    src/action/selection_paste_action.cpp
    src/action/selection_cut_action.cpp
    src/action/selection_move_action.cpp
    src/action/save_as_action.cpp
    src/action/export_file_action.cpp
    src/action/export_image_action.cpp
    src/action/new_package_action.cpp
    src/action/change_active_layer_action.cpp
    src/action/settings_action.cpp
    src/action/settings_pages_processes.cpp

    src/file/map_file.cpp
    src/file/map_document.cpp
    src/file/asset_manager.cpp
    src/file/file_manager.cpp
    src/file/tileset_file.cpp
    src/file/tileset_document.cpp
    src/file/package_file.cpp
    # src/file/document_serializer.cpp
    src/file/package_document.cpp

    src/editor_tools/brush.cpp
    src/editor_tools/paint_modes.cpp

    src/sgc_extension/marching_ants_rectangle.cpp
    src/sgc_extension/asset_builder.cpp
    src/sgc_extension/asset_serializer.cpp
    src/sgc_extension/runtime_builder.cpp
    src/sgc_extension/asset_deserializer.cpp

    src/settings/settings_manager.cpp
    src/settings/shortcut_settings_manager.cpp
    src/settings/shortcuts_setting.cpp
    src/settings/default_open_filetype_setting.cpp
    src/settings/auto_restore_files_setting.cpp
    src/settings/preferences.cpp
    src/settings/settings_json_string_lookup.cpp    
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
    ${SGC_LIB_PATH}
)

target_link_libraries(sgc-editor PRIVATE
    Comctl32
    Comdlg32
    windowscodecs
    SDL3
    SDL3_image
    sdl-game-core
    nlohmann_json::nlohmann_json
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
