set(darklydecoration_config_SRCS
    ../darklyexceptionlist.cpp
    darklyconfigwidget.cpp
    darklydetectwidget.cpp
    darklyexceptiondialog.cpp
    darklyexceptionlistwidget.cpp
    darklyexceptionmodel.cpp
    darklyitemmodel.cpp
)
ki18n_wrap_ui(darklydecoration_config_SRCS
   ui/darklyconfigurationui.ui
   ui/darklyexceptiondialog.ui
   ui/darklyexceptionlistwidget.ui
)

kcoreaddons_add_plugin(kcm_darklydecoration SOURCES kcm_darklydecoration.cpp ${darklydecoration_config_SRCS} INSTALL_NAMESPACE "${KDECORATION_KCM_PLUGIN_DIR}")
kconfig_add_kcfg_files(kcm_darklydecoration ../darklysettings.kcfgc)
target_include_directories(kcm_darklydecoration PRIVATE ${CMAKE_SOURCE_DIR}/kdecoration ${CMAKE_BINARY_DIR}/kdecoration/)
target_link_libraries(kcm_darklydecoration
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::DBus
    PRIVATE
        KF6::ConfigCore
        KF6::CoreAddons
        KF6::GuiAddons
        KF6::I18n
        KF6::IconThemes
        KF6::KCMUtils
)
kcmutils_generate_desktop_file(kcm_darklydecoration)
