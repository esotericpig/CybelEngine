###
# Functions:
#   cybel_add_cppcheck_target(CXX_V : int?,ADD_CYBEL_DEFAULTS : bool?,ARGS : string[]?)
#
# @author Bradley Whited
###

include_guard(GLOBAL)

# Must enable `CMAKE_EXPORT_COMPILE_COMMANDS`.
# Defaults to `cppcheck` or to `CMAKE_CXX_CPPCHECK` if set.
#
# @arg CXX_V : int? - C++ version to use.
#      - Example: CXX_V 20
#      - Default: `CMAKE_CXX_STANDARD` if set.
# @arg ADD_CYBEL_DEFAULTS : bool? - Add Cybel Engine's default suppressions.
#      - Useful if using Cybel Engine as a non-system library.
# @arg ARGS : string[]? - Args to pass to Cppcheck.
#      - Example: ARGS --suppress=checkersReport --suppress=functionConst
function(cybel_add_cppcheck_target)
  cmake_parse_arguments(PARSE_ARGV 0 arg
      # Flags.
      "ADD_CYBEL_DEFAULTS"
      # One-value opts.
      "CXX_V"
      # Multi-value opts.
      "ARGS"
  )

  set(cppcheck_bin "cppcheck")
  set(cppcheck_args "")

  if(CMAKE_CXX_CPPCHECK)
    set(cppcheck_bin "${CMAKE_CXX_CPPCHECK}")
  endif()

  if(NOT arg_CXX_V AND CMAKE_CXX_STANDARD)
    set(arg_CXX_V "${CMAKE_CXX_STANDARD}")
  endif()
  if(arg_CXX_V)
    list(APPEND cppcheck_args "--std=c++${arg_CXX_V}")
  endif()

  set(bin_dir "${CMAKE_CURRENT_BINARY_DIR}")
  set(cppcheck_dir "${bin_dir}/cppcheck")

  if(EXISTS "${bin_dir}" AND NOT EXISTS "${cppcheck_dir}/build")
    file(MAKE_DIRECTORY "${cppcheck_dir}/build")
  endif()

  list(APPEND cppcheck_args
      --quiet --inline-suppr
      --enable=all --check-level=exhaustive --inconclusive
      --platform=native "--language=c++"

      "--relative-paths=${CMAKE_CURRENT_SOURCE_DIR}"
      "--checkers-report=${cppcheck_dir}/checkers_report.txt"
      "--cppcheck-build-dir=${cppcheck_dir}/build"
  )

  if(arg_ADD_CYBEL_DEFAULTS)
    list(APPEND cppcheck_args
        --suppress=checkersReport
        --suppress=functionConst
        --suppress=functionStatic
        --suppress=missingInclude
        --suppress=missingIncludeSystem
#        "--suppress=noExplicitConstructor:src/cybel/types/"
        --suppress=shadowFunction
        --suppress=unknownMacro # For Emscripten: EM_ASM(), etc.
        --suppress=unmatchedSuppression
        --suppress=unusedFunction
#        --suppress=unusedPrivateFunction
#        --suppress=useStlAlgorithm
#        --suppress=variableScope
    )
  endif()

  list(APPEND cppcheck_args
      ${arg_ARGS}

#      "-I=${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include"
#      "${SRC_DIR}"
      "--project=${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
  )

  add_custom_target(cppcheck
      COMMAND "${cppcheck_bin}" ${cppcheck_args}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      USES_TERMINAL
      VERBATIM
  )
endfunction()
