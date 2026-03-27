add_definitions(-DTRANSLATION_DOMAIN="darkly_kwin_deco")

################# configuration #################
configure_file(config-darkly.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-darkly.h )

################# includes #################
include_directories(${CMAKE_SOURCE_DIR}/libdarklycommon)
include_directories(${CMAKE_BINARY_DIR}/libdarklycommon)

################# newt target #################
### plugin classes
set(darklydecoration_SRCS
    darklybutton.cpp
    darklydecoration.cpp
    darklyexceptionlist.cpp
    darklysettingsprovider.cpp
)
### build library
kconfig_add_kcfg_files(darklydecoration_SRCS darklysettings.kcfgc)
add_library(darklydecoration MODULE
    ${darklydecoration_SRCS}
    ${darklydecoration_config_SRCS}
    ${darklydecoration_config_PART_FORMS_HEADERS})

set_target_properties(darklydecoration PROPERTIES
    OUTPUT_NAME org.kde.darkly
)

target_link_libraries(darklydecoration
     PUBLIC
        Qt6::DBus
        KDecoration3::KDecoration
     PRIVATE
        darklycommon6
        KF6::CoreAddons
        KF6::ConfigWidgets
        KF6::GuiAddons
        KF6::I18n
        KF6::IconThemes
        KF6::WindowSystem
        KF6::KCMUtils
)
install(TARGETS darklydecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/${KDECORATION_PLUGIN_DIR})
install(FILES config/darklydecorationconfig.desktop DESTINATION  ${KDE_INSTALL_DATADIR}/kservices6)
add_subdirectory(config)
