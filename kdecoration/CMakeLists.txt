add_definitions(-DTRANSLATION_DOMAIN="darkly_kwin_deco")

add_subdirectory(libdbusmenuqt)

################# includes #################
include_directories(${CMAKE_SOURCE_DIR}/libdarklycommon)
include_directories(${CMAKE_BINARY_DIR}/libdarklycommon)

################# newt target #################
### plugin classes
set(darklydecoration_SRCS
    darklyapplicationmenu.cpp
    darklybutton.cpp
    darklydecoration.cpp
    darklysettingsprovider.cpp
    darklyexceptionlist.cpp
)

### build library
kconfig_add_kcfg_files(darklydecoration_SRCS darklysettings.kcfgc)
add_library(darklydecoration MODULE
    ${darklydecoration_SRCS}
    ${darklydecoration_config_PART_FORMS_HEADERS})

set_target_properties(darklydecoration PROPERTIES
    OUTPUT_NAME org.kde.darkly
)

target_link_libraries(darklydecoration
    PRIVATE
        darklycommon6
        dbusmenuqt
        Qt6::DBus
        KF6::CoreAddons
        KF6::ConfigGui
        KF6::GuiAddons
        KF6::I18n
        KF6::IconThemes
        KDecoration3::KDecoration
)

install(TARGETS darklydecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/${KDECORATION_PLUGIN_DIR})

add_subdirectory(config)

