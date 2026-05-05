###
# See:
# - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging
###

# TEST: Set SOURCE_PATH to local dir for testing purposes.
#set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../..")
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO "esotericpig/CybelEngine"
#    REF "v${VERSION}"
#    SHA512 "0"
    HEAD_REF "main"
)

set(FEATURE_OPTIONS "")
vcpkg_check_features(
    OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
      metrics CYBEL_USE_METRICS
)

if("gles" IN_LIST FEATURES)
  list(APPEND FEATURE_OPTIONS
      -DCYBEL_RENDERER=GLES
  )
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
      -DCYBEL_OBJECT_LIB=OFF
      ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME "CybelEngine"
    CONFIG_PATH "lib/cmake/CybelEngine"
)
vcpkg_fixup_pkgconfig()

vcpkg_copy_pdbs()
file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
