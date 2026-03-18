###
# @author Bradley Whited
###

include_guard(GLOBAL)

function(cybel_add_cppcheck_target)
  cmake_parse_arguments(PARSE_ARGV 0 arg
      # Flags.
      ""
      # One-value opts.
      "CXX_V"
      # Multi-value opts.
      "ARGS"
  )

  set(cppcheck_bin "${CMAKE_CXX_CPPCHECK}")

  if(NOT cppcheck_bin)
    set(cppcheck_bin "cppcheck")
  endif()
  if(NOT arg_CXX_V)
    if(CMAKE_CXX_STANDARD)
      set(arg_CXX_V "${CMAKE_CXX_STANDARD}")
    else()
      set(arg_CXX_V "20")
    endif()
  endif()

  set(bin_dir "${CMAKE_CURRENT_BINARY_DIR}")
  set(cppcheck_dir "${bin_dir}/cppcheck")
  set(cppcheck_build_dir "${cppcheck_dir}/build")

  if(EXISTS "${bin_dir}" AND NOT EXISTS "${cppcheck_build_dir}")
    file(MAKE_DIRECTORY "${cppcheck_build_dir}")
  endif()

  add_custom_target(cppcheck
      COMMAND "${cppcheck_bin}"
        --quiet --inline-suppr
        --enable=all --check-level=exhaustive --inconclusive
        --platform=native "--language=c++" "--std=c++${arg_CXX_V}"

        "--relative-paths=${CMAKE_CURRENT_SOURCE_DIR}"
        "--checkers-report=${cppcheck_dir}/checkers_report.txt"
        "--cppcheck-build-dir=${cppcheck_build_dir}"

        ${arg_ARGS}

#        "-I=${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include"
#        "${CYBEL_SRC_DIR}"
        "--project=${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      USES_TERMINAL
      VERBATIM
  )
endfunction()
