cmake_minimum_required(VERSION 3.16)

set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_OUTPUT_FILE_PREFIX "${PROJECT_SOURCE_DIR}/packages")
set(CPACK_VERBATIM_VARIABLES YES)

set(SourceIgnoreFiles 
    ".cache"
    ".clang-format"
    ".clangd"
    ".git/"
    ".gitea/"
    ".github/"
    ".gitignore"
    ".idea"
    "CMakeCache.txt"
    "CMakeFiles/"
    "CPackConfig.cmake$"
    "CPackSourceConfig.cmake"
    "CTestTestfile.cmake"
    "Makefile"
    "_CPack_Packages/"
    "build/"
    "cmake-build*"
    "cmake_install.cmake"
    "dist/"
    "packages/"
)

# Escape any '.' and '/' characters
string(REPLACE "." "\\\." SourceIgnoreFiles "${SourceIgnoreFiles}")
string(REPLACE "/" "\\\/" SourceIgnoreFiles "${SourceIgnoreFiles}")

# Override install prefix for package target
set(CMAKE_INSTALL_PREFIX ".")
string(REGEX REPLACE "^/(.*)" "\\1" 
    CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}"
)

if(BUILD_RPMS)
    set(CPACK_SET_DESTDIR OFF)
    set(CMAKE_INSTALL_PREFIX "/usr")
    set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set(CPACK_RPM_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}.x86_64")
endif()
if(BUILD_DEBS)
    set(CPACK_SET_DESTDIR OFF)
    set(CMAKE_INSTALL_PREFIX "/usr")
    set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set(CPACK_DEBIAN_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}_amd64")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "DeltaCopy")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
endif()

include(CPack)

add_custom_target(spackages
    COMMAND "${CMAKE_COMMAND}"
        --build "${CMAKE_BINARY_DIR}"
        --target package_source
    VERBATIM
    USES_TERMINAL
)

# vim: ts=4:sw=4:sts=4:et:syntax=cmake
