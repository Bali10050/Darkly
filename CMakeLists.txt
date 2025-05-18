cmake_minimum_required(VERSION 3.16)

# Get version
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VER_RAW)
string(STRIP ${VER_RAW} VER)

project(
    darkly
    DESCRIPTION "A modern style for qt applications."
    VERSION ${VER}
)

set(KF5_MIN_VERSION "5.102.0")
set(KF6_MIN_VERSION "6.10.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
set(QT5_MIN_VERSION "5.15.2")
set(QT_MIN_VERSION "6.7.0")

include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(FeatureSummary)
include(GNUInstallDirs)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
#find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(CMakePackageConfigHelpers)
include(ECMInstallIcons)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(GtkUpdateIconCache)

option(FOR_FLATPAK "Build Lightly for Flatpak" OFF)
option(BUILD_QT5 "Build with QT5" ON)
option(BUILD_QT6 "Build with QT6" ON)

set(QT_NO_CREATE_VERSIONLESS_TARGETS ON)
set(QT_NO_CREATE_VERSIONLESS_FUNCTIONS ON)

if(WIN32 OR APPLE OR FOR_FLATPAK)
    set(WITH_DECORATIONS OFF)
endif()

function(build_qt5)
    set(QT_MAJOR_VERSION 5)
    include(KDEInstallDirs5)
    include(KDECMakeSettings)
    option(WITH_DECORATIONS "Build Darkly window decorations for KWin" ON)

    find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets DBus)
    find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
            CoreAddons
            Config
            GuiAddons
            I18n
            IconThemes
            WindowSystem)
        if(NOT WIN32 AND NOT APPLE)
            find_package(KF5KCMUtils ${KF5_MIN_VERSION})
            set_package_properties(KF5KCMUtils PROPERTIES
                TYPE REQUIRED
                DESCRIPTION "Helps create configuration modules"
                PURPOSE "KCMUtils used for the configuration modules or the decoration and Qt Style"
            )
        endif()

    find_package(Qt5 ${QT5_MIN_VERSION} OPTIONAL_COMPONENTS Quick)
        if(${Qt5Quick_FOUND})
        find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2)
        endif()

    find_package(KF5FrameworkIntegration ${KF5_MIN_VERSION} CONFIG )
        set_package_properties(KF${QT_MAJOR_VERSION}FrameworkIntegration PROPERTIES
            DESCRIPTION "KF${QT_MAJOR_VERSION} Framework Integration"
            URL "https://projects.kde.org/projects/frameworks/frameworkintegration"
            TYPE OPTIONAL
            PURPOSE "Required to use KStyle convenience functionalities in style")

    set(DARKLY_HAVE_KSTYLE ${KF5FrameworkIntegration_FOUND})
    set(DARKLY_HAVE_QTQUICK ${Qt5Quick_FOUND})


    #add_subdirectory(colors)
    add_subdirectory(kstyle kstyle5)
    add_subdirectory(libdarklycommon libdarklycommon5)
    #add_subdirectory(misc)
    unset(QUERY_EXECUTABLE CACHE)
endfunction()
if(BUILD_QT5)
    message("-- **** Build QT5 - On ****")
    build_qt5()
endif()

function(build_qt6)
    set(QT_MAJOR_VERSION 6)
    include(KDEInstallDirs6)
    include(KDECMakeSettings)

    option(WITH_DECORATIONS "Build Darkly window decorations for KWin" ON)

    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets DBus)
    find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
            CoreAddons
            ColorScheme
            Config
            GuiAddons
            I18n
            IconThemes
            WindowSystem)
        if(NOT WIN32 AND NOT APPLE)
            find_package(KF6KCMUtils ${KF6_MIN_VERSION})
            set_package_properties(KF6KCMUtils PROPERTIES
                TYPE REQUIRED
                DESCRIPTION "Helps create configuration modules"
                PURPOSE "KCMUtils used for the configuration modules or the decoration and Qt Style"
            )
        endif()

    find_package(Qt6 ${QT_MIN_VERSION} OPTIONAL_COMPONENTS Quick)
        if(${Qt6Quick_FOUND})
            find_package(KF6KirigamiPlatform ${KF6_MIN_VERSION} REQUIRED)
        endif()

    find_package(KF6FrameworkIntegration ${KF6_MIN_VERSION} CONFIG )
        set_package_properties(KF${QT_MAJOR_VERSION}FrameworkIntegration PROPERTIES
            DESCRIPTION "KF${QT_MAJOR_VERSION} Framework Integration"
            URL "https://projects.kde.org/projects/frameworks/frameworkintegration"
            TYPE OPTIONAL
            PURPOSE "Required to use KStyle convenience functionalities in style")

    set(DARKLY_HAVE_KSTYLE ${KF6FrameworkIntegration_FOUND})
    set(DARKLY_HAVE_QTQUICK ${Qt6Quick_FOUND})


    add_library(Qt::Core ALIAS Qt6::Core)
    add_library(Qt::Gui ALIAS Qt6::Gui)
    add_library(Qt::OpenGL ALIAS Qt6::OpenGL)
    add_library(Qt::Network ALIAS Qt6::Network)
    if(Qt6_VERSION_MINOR GREATER 6)
        add_library(Qt::PlatformModuleInternal ALIAS Qt6::PlatformModuleInternal)
    endif()

    #add_subdirectory(colors)
    add_subdirectory(kstyle kstyle6)
    add_subdirectory(libdarklycommon libdarklycommon6)
    #add_subdirectory(misc)

    if(WITH_DECORATIONS)
        find_package(KDecoration3 REQUIRED)
        add_subdirectory(kdecoration)
    endif()

    ki18n_install(po)

    # create a Config.cmake and a ConfigVersion.cmake file and install them
    include(ECMSetupVersion)
    ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX DARKLY
                    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/DarklyConfigVersion.cmake"
                    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kstyle6/config/darklystyleversion.h"
    )



    set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Darkly")

    if (NOT FOR_FLATPAK)
        configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/DarklyConfig.cmake.in"
                                        "${CMAKE_CURRENT_BINARY_DIR}/DarklyConfig.cmake"
                                        PATH_VARS KDE_INSTALL_FULL_DATADIR
                                        INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                                    )

        install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/DarklyConfig.cmake"
                        "${CMAKE_CURRENT_BINARY_DIR}/DarklyConfigVersion.cmake"
                    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
                    COMPONENT Devel
                )
    endif()
    unset(QUERY_EXECUTABLE CACHE)
endfunction()

if(BUILD_QT6)
    message("-- **** Build QT6 - On ****")
    build_qt6()
endif()

function(build_colors)
    include(KDEInstallDirs)
    add_subdirectory(colors)
endfunction()

if (NOT FOR_FLATPAK)
    build_colors()
endif()

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
