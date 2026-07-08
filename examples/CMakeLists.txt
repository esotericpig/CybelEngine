cmake_minimum_required(VERSION 3.22)

#===========================================
# Project Setup
#===========================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(CybelEngineExamplesBuild INTERFACE)
add_library(CybelEngine::ExamplesBuild ALIAS CybelEngineExamplesBuild)

#===========================================
# Build Setup
#===========================================
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(CybelEngineExamplesBuild INTERFACE
      /utf-8
#      /W4 /permissive-
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(CybelEngineExamplesBuild INTERFACE
      "-finput-charset=UTF-8"

      -Wall -Wconversion -Wextra -Wpedantic -Wundef
      -fno-common

      # Options added for GCC to match Clang.
      -Wmismatched-tags -Wsign-conversion
  )
endif()

#===========================================
# Web Setup
#===========================================
if(EMSCRIPTEN)
  # Options:
  # - https://emscripten.org/docs/tools_reference/emcc.html
  #
  # Settings:
  # - https://github.com/emscripten-core/emscripten/blob/main/src/settings.js

  set(web_compile_and_link_opts
      # Options.
      -fno-rtti

      $<$<CONFIG:Debug>:
        --minify 0
      >

      # Settings.
      -sDISABLE_EXCEPTION_CATCHING=0

      # Libraries (Ports).
      # - Show available ports:
      #     emcc --show-ports
      # - Show a port's options (e.g., sdl2_image's formats):
      #     emcc --use-port=sdl2_image:help
      --use-port=sdl2
      --use-port=sdl2_image:formats=png
      --use-port=sdl2_mixer
  )
  target_compile_options(CybelEngineExamplesBuild INTERFACE
      ${web_compile_and_link_opts}
  )
  target_link_options(CybelEngineExamplesBuild INTERFACE
      ${web_compile_and_link_opts}

      # Link Options.
      --emrun
      --output_eol linux

      # Link Settings.
      -sALLOW_MEMORY_GROWTH=1
      -sMINIFY_HTML=0
      -sSINGLE_FILE=1

      # - WebGL 2.0 (OpenGL ES 3.0).
      -sMIN_WEBGL_VERSION=2
      -sMAX_WEBGL_VERSION=2

      $<$<CONFIG:Debug>:
        -sEXCEPTION_DEBUG=1
        -sEXCEPTION_STACK_TRACES=1
        -sGL_ASSERTIONS=1
        -sSAFE_HEAP=1
      >

      # Assets.
      # TODO: Add asset files for examples.
#      "--embed-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
  )
endif()

#===========================================
# Executables
#===========================================
set(examples
    00_window
)

foreach(example IN LISTS examples)
  add_executable("${example}" WIN32
      "${example}.cpp"
  )

  if(EMSCRIPTEN)
    set_target_properties("${example}" PROPERTIES
        SUFFIX ".html"
    )
  endif()

  target_link_libraries("${example}" PRIVATE
      CybelEngine::ExamplesBuild
      CybelEngine::CybelEngine
  )
  target_include_directories("${example}" PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}"
  )
endforeach()
