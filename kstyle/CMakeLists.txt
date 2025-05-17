################# includes #################
include_directories(
  animations
  debug
)

include_directories(${CMAKE_SOURCE_DIR}/libdarklycommon)
include_directories(${CMAKE_BINARY_DIR}/libdarklycommon)

################# configuration #################
configure_file(config-darkly.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-darkly.h)

########### next target ###############
set(darkly_PART_SRCS
    animations/darklyanimation.cpp
    animations/darklyanimations.cpp
    animations/darklyanimationdata.cpp
    animations/darklybaseengine.cpp
    animations/darklybusyindicatordata.cpp
    animations/darklybusyindicatorengine.cpp
    animations/darklydialdata.cpp
    animations/darklydialengine.cpp
    animations/darklyenabledata.cpp
    animations/darklygenericdata.cpp
    animations/darklyheaderviewdata.cpp
    animations/darklyheaderviewengine.cpp
    animations/darklyscrollbardata.cpp
    animations/darklyscrollbarengine.cpp
    animations/darklyspinboxengine.cpp
    animations/darklyspinboxdata.cpp
    animations/darklystackedwidgetdata.cpp
    animations/darklystackedwidgetengine.cpp
    animations/darklytabbarengine.cpp
    animations/darklytabbardata.cpp
    animations/darklytoolboxengine.cpp
    animations/darklytransitiondata.cpp
    animations/darklytransitionwidget.cpp
    animations/darklywidgetstateengine.cpp
    animations/darklywidgetstatedata.cpp
    debug/darklywidgetexplorer.cpp
    darklyaddeventfilter.cpp
    darklyblurhelper.cpp
    darklyframeshadow.cpp
    darklyhelper.cpp
    darklymdiwindowshadow.cpp
    darklymnemonics.cpp
    darklypropertynames.cpp
    darklyshadowhelper.cpp
    darklysplitterproxy.cpp
    darklystyle.cpp
    darklystyleplugin.cpp
    darklytileset.cpp
    darklywindowmanager.cpp
    darklytoolsareamanager.cpp
)

kconfig_add_kcfg_files(darkly_PART_SRCS ../kdecoration/darklysettings.kcfgc)
kconfig_add_kcfg_files(darkly_PART_SRCS darklystyleconfigdata.kcfgc)

add_library(darkly${QT_MAJOR_VERSION} MODULE ${darkly_PART_SRCS})

target_link_libraries(darkly${QT_MAJOR_VERSION}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::DBus
)

if( DARKLY_HAVE_QTQUICK )
    target_link_libraries(darkly${QT_MAJOR_VERSION}
        Qt${QT_MAJOR_VERSION}::Quick
        KF${QT_MAJOR_VERSION}::CoreAddons
    )
endif()

target_link_libraries(darkly${QT_MAJOR_VERSION}
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::ConfigGui
    KF${QT_MAJOR_VERSION}::GuiAddons
    KF${QT_MAJOR_VERSION}::IconThemes
    KF${QT_MAJOR_VERSION}::WindowSystem
)

if(QT_MAJOR_VERSION STREQUAL "5")
    target_link_libraries(darkly5 KF5::ConfigWidgets)
    if (DARKLY_HAVE_QTQUICK)
        target_link_libraries(darkly5 KF5::Kirigami2)
    endif()
else()
    target_link_libraries(darkly6 KF6::ColorScheme)
    if (DARKLY_HAVE_QTQUICK)
        target_link_libraries(darkly6 KF6::KirigamiPlatform)
    endif()
endif()


target_link_libraries(darkly${QT_MAJOR_VERSION} darklycommon${QT_MAJOR_VERSION})

if(KF${QT_MAJOR_VERSION}FrameworkIntegration_FOUND)
    target_link_libraries(darkly${QT_MAJOR_VERSION}  KF${QT_MAJOR_VERSION}::Style)
endif()


if (WIN32)
    # As stated in https://docs.microsoft.com/en-us/cpp/c-runtime-library/math-constants M_PI only gets defined
    # when if _USE_MATH_DEFINES is defined
    target_compile_definitions(darkly PRIVATE _USE_MATH_DEFINES _BSD_SOURCE)
endif()


########### install files ###############
if (FOR_FLATPAK)
    set(LIB_TARGET ${QT_PLUGINS_DIR}/styles/)
else()
    set(LIB_TARGET ${KDE_INSTALL_QTPLUGINDIR}/styles/)
endif()
install(TARGETS darkly${QT_MAJOR_VERSION} DESTINATION ${LIB_TARGET})

if (NOT FOR_FLATPAK)
    install(FILES darkly.themerc  DESTINATION  ${KDE_INSTALL_DATADIR}/kstyle/themes)
endif()

########### subdirectories ###############

if (QT_MAJOR_VERSION EQUAL "6" AND TARGET "KF6::KCMUtils" AND NOT FOR_FLATPAK)
    add_subdirectory(config)
endif()
