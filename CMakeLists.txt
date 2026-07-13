cmake_minimum_required(VERSION 3.22)

###
# @author Bradley Whited
###
project("CybelEngine"
    VERSION "1.0"
    HOMEPAGE_URL "https://github.com/esotericpig/CybelEngine"
    DESCRIPTION "Cybel Engine is a simple C++ game engine using SDL2, OpenGL, and Emscripten."
    LANGUAGES C CXX
)

#===========================================
# User Config
#===========================================
option(CYBEL_OBJECT_LIB "Build as an Object Library?" ON)
option(CYBEL_USE_LIBS "Use external libs? (OFF for running standalone custom targets/scripts faster.)" ON)
option(CYBEL_USE_CONFIG_PKGS
    "Use CONFIG in find_package()? (ON to use vcpkg libs; OFF to use system libs only.)"
    ON
)

option(CYBEL_BUILD_EXAMPLES "Build examples?" OFF)
option(CYBEL_USE_SANITIZE "Use sanitizers in Debug build to detect memory leaks?" OFF)
option(CYBEL_USE_METRICS "Use metrics, such as profiling and draw count?" OFF)

set(CYBEL_RENDERER "GL" CACHE STRING "Desktop Renderer to use: GL or GLES.")
set_property(CACHE CYBEL_RENDERER PROPERTY STRINGS "GL" "GLES")

message("
[CybelEngine::UserConfig]
  CMAKE_TOOLCHAIN_FILE:           '${CMAKE_TOOLCHAIN_FILE}'
  VCPKG_CHAINLOAD_TOOLCHAIN_FILE: '${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}'
  VCPKG_TARGET_TRIPLET:           '${VCPKG_TARGET_TRIPLET}'

  CYBEL_OBJECT_LIB?               ${CYBEL_OBJECT_LIB}
  CYBEL_USE_LIBS?                 ${CYBEL_USE_LIBS}
  CYBEL_USE_CONFIG_PKGS?          ${CYBEL_USE_CONFIG_PKGS}

  CYBEL_BUILD_EXAMPLES?           ${CYBEL_BUILD_EXAMPLES}
  CYBEL_USE_SANITIZE?             ${CYBEL_USE_SANITIZE}
  CYBEL_USE_METRICS?              ${CYBEL_USE_METRICS}
  CYBEL_RENDERER:                 '${CYBEL_RENDERER}'
")

#===========================================
# Project Config
#===========================================
set(CYBEL_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CYBEL_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CYBEL_RENDERER_GL OFF)
set(CYBEL_RENDERER_GLES OFF)

if(EMSCRIPTEN OR CYBEL_RENDERER STREQUAL "GLES")
  set(CYBEL_RENDERER_GLES ON)
elseif(CYBEL_RENDERER STREQUAL "GL")
  set(CYBEL_RENDERER_GL ON)
else()
  message(FATAL_ERROR "Invalid CYBEL_RENDERER: '${CYBEL_RENDERER}'.")
endif()

if(NOT "${CYBEL_CMAKE_DIR}/modules" IN_LIST CMAKE_MODULE_PATH)
  list(APPEND CMAKE_MODULE_PATH "${CYBEL_CMAKE_DIR}/modules")

  if(NOT PROJECT_IS_TOP_LEVEL)
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
  endif()
endif()

#===========================================
# Build Config
#===========================================
set(CYBEL_CXX_V "20")
set(CYBEL_SANITIZE_FLAGS "")
set(CYBEL_FIND_PKG_CONFIG "")

if(CYBEL_USE_SANITIZE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  list(APPEND CYBEL_SANITIZE_FLAGS
      -O0 -g
      "-fsanitize=address,leak,undefined"
      -fno-omit-frame-pointer
  )
endif()

if(CYBEL_USE_CONFIG_PKGS)
  list(APPEND CYBEL_FIND_PKG_CONFIG CONFIG)
endif()

if(PROJECT_IS_TOP_LEVEL OR CYBEL_BUILD_EXAMPLES)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif()

#===========================================
# Install Config
#===========================================
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CYBEL_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/CybelEngine")

#===========================================
# Project Setup
#===========================================
if(CYBEL_OBJECT_LIB)
  add_library(CybelEngine OBJECT)
else()
  add_library(CybelEngine)
endif()

add_library(CybelEngine::CybelEngine ALIAS CybelEngine)

# - https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html#creating-a-package-version-file
set_target_properties(CybelEngine PROPERTIES
    COMPATIBLE_INTERFACE_STRING "CybelEngine_MAJOR_VERSION"
    INTERFACE_CybelEngine_MAJOR_VERSION "${PROJECT_VERSION_MAJOR}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    VERSION "${PROJECT_VERSION}"
)

#===========================================
# Build Setup
#===========================================
set_target_properties(CybelEngine PROPERTIES
    CXX_STANDARD "${CYBEL_CXX_V}"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    EXPORT_COMPILE_COMMANDS ON
    POSITION_INDEPENDENT_CODE ON
)
target_compile_features(CybelEngine PUBLIC
    "cxx_std_${CYBEL_CXX_V}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(CybelEngine PRIVATE
      /utf-8
#      /W4 /permissive-
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(CybelEngine PRIVATE
      "-finput-charset=UTF-8"

      $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Emscripten>>:
        -Wall -Wconversion -Wextra -Wpedantic -Wundef
        -fno-common

        # Options added for GCC to match Clang.
        -Wmismatched-tags -Wsign-conversion
      >
  )
endif()

target_compile_options(CybelEngine PRIVATE
    $<$<CONFIG:Debug>:${CYBEL_SANITIZE_FLAGS}>
)
target_link_options(CybelEngine PRIVATE
    $<$<CONFIG:Debug>:${CYBEL_SANITIZE_FLAGS}>
)

#===========================================
# Compile Definitions
#===========================================
set(cybel_compile_defs "")

if(EMSCRIPTEN)
  list(APPEND cybel_compile_defs CYBEL_PLATFORM_WEB)
elseif(APPLE)
  list(APPEND cybel_compile_defs CYBEL_PLATFORM_MACOS)
elseif(WIN32)
  list(APPEND cybel_compile_defs CYBEL_PLATFORM_WINDOWS)
else() # Linux
  list(APPEND cybel_compile_defs CYBEL_PLATFORM_LINUX)
endif()

if(CYBEL_RENDERER_GL)
  list(APPEND cybel_compile_defs CYBEL_RENDERER_GL)
elseif(CYBEL_RENDERER_GLES)
  list(APPEND cybel_compile_defs CYBEL_RENDERER_GLES)
endif()

if(CYBEL_USE_METRICS)
  list(APPEND cybel_compile_defs CYBEL_METRICS)
endif()

target_compile_definitions(CybelEngine PUBLIC
  ${cybel_compile_defs}
)

#===========================================
# Libraries
#===========================================
if(CYBEL_USE_LIBS)
  # See `vcpkg.json` & CMake config output for find_package()/link usage.

  if(NOT EMSCRIPTEN)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED) # GLU should be included.
    find_package(SDL2 ${CYBEL_FIND_PKG_CONFIG} REQUIRED)
    find_package(SDL2_image ${CYBEL_FIND_PKG_CONFIG} REQUIRED)
    find_package(SDL2_mixer ${CYBEL_FIND_PKG_CONFIG} REQUIRED)

    target_link_libraries(CybelEngine PUBLIC
        GLEW::GLEW
        OpenGL::GL
        OpenGL::GLU
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    )
  endif()

  if(CYBEL_RENDERER_GLES)
    find_package(glm ${CYBEL_FIND_PKG_CONFIG} REQUIRED)

    target_link_libraries(CybelEngine PUBLIC
        glm::glm
    )
  endif()
endif()

#===========================================
# Source Code
#===========================================
target_include_directories(CybelEngine PUBLIC
    "$<BUILD_INTERFACE:${CYBEL_SRC_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_sources(CybelEngine PRIVATE
    # @glob_src=src:src/
    "src/cybel/cybel_engine.cpp"
    "src/cybel/asset/asset_loaders.cpp"
    "src/cybel/asset/asset_man.cpp"
    "src/cybel/audio/audio_player.cpp"
    "src/cybel/audio/music.cpp"
    "src/cybel/audio/sound.cpp"
    "src/cybel/gfx/font_atlas.cpp"
    "src/cybel/gfx/gl_util.cpp"
    "src/cybel/gfx/image.cpp"
    "src/cybel/gfx/renderer.cpp"
    "src/cybel/gfx/renderer_gl.cpp"
    "src/cybel/gfx/renderer_gles.cpp"
    "src/cybel/gfx/sprite.cpp"
    "src/cybel/gfx/sprite_atlas.cpp"
    "src/cybel/gfx/texture.cpp"
    "src/cybel/input/game_ctrl.cpp"
    "src/cybel/input/input_man.cpp"
    "src/cybel/input/joystick.cpp"
    "src/cybel/input/key_input.cpp"
    "src/cybel/io/text_reader.cpp"
    "src/cybel/io/text_reader_buf.cpp"
    "src/cybel/metrics/metric_man.cpp"
    "src/cybel/scene/scene_bag.cpp"
    "src/cybel/scene/scene_man.cpp"
    "src/cybel/stubs/glew_stub.cpp"
    "src/cybel/text/rune_iterator.cpp"
    "src/cybel/text/rune_token.cpp"
    "src/cybel/text/rune_util.cpp"
    "src/cybel/text/rune_view.cpp"
    "src/cybel/text/text_util.cpp"
    "src/cybel/types/color.cpp"
    "src/cybel/types/cybel_error.cpp"
    "src/cybel/types/range.cpp"
    "src/cybel/ui/ui_box_node.cpp"
    "src/cybel/ui/ui_flex_grid.cpp"
    "src/cybel/ui/ui_quad.cpp"
    "src/cybel/ui/ui_sprite.cpp"
    "src/cybel/ui/ui_texture.cpp"
    "src/cybel/util/file_sys.cpp"
    "src/cybel/util/rando.cpp"
    "src/cybel/util/timer.cpp"
    "src/cybel/vfx/particle.cpp"
    # @glob_src
)

#===========================================
# Install Setup
#===========================================
if(NOT CYBEL_OBJECT_LIB)
  # See:
  # - https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html#exporting-targets
  #
  # TEST: cmake --install build/default --config Debug --prefix build/test_install

  install(TARGETS CybelEngine
      EXPORT CybelEngineTargets
      ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
      LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
      INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )
  install(DIRECTORY "${CYBEL_CMAKE_DIR}/modules"
      DESTINATION "${CYBEL_INSTALL_CMAKE_DIR}"
  )
  install(DIRECTORY "${CYBEL_SRC_DIR}/"
      TYPE INCLUDE
      FILES_MATCHING PATTERN "*.h"
  )
  install(EXPORT CybelEngineTargets
      DESTINATION "${CYBEL_INSTALL_CMAKE_DIR}"
      NAMESPACE "CybelEngine::"
      FILE "CybelEngineTargets.cmake"
  )

  configure_package_config_file(
      "${CYBEL_CMAKE_DIR}/CybelEngineConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/CybelEngineConfig.cmake"
      INSTALL_DESTINATION "${CYBEL_INSTALL_CMAKE_DIR}"
  )
  write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/CybelEngineConfigVersion.cmake"
      VERSION "${PROJECT_VERSION}"
      COMPATIBILITY SameMajorVersion
  )
  install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/CybelEngineConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/CybelEngineConfigVersion.cmake"
      DESTINATION "${CYBEL_INSTALL_CMAKE_DIR}"
  )
endif()

#===========================================
# Examples
#===========================================
if(CYBEL_BUILD_EXAMPLES)
  add_subdirectory("examples")
endif()

#===========================================
# Custom Targets
#===========================================
if(PROJECT_IS_TOP_LEVEL)
  include(cybel/CustomTargets)

  cybel_add_cppcheck_target(
      CXX_V "${CYBEL_CXX_V}"
      ADD_CYBEL_DEFAULTS
  )
endif()
