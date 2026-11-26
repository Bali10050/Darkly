#!/bin/bash

styles=(
    "lightly"
    "Lightly"
    "Darkly"
    "darkly"
)

test $(whoami) != "root" && echo "[ERROR]: This script needs sudo access to remove files." && exit 1

for style in ${styles[@]}; do
    rm /usr/lib/lib${style}common5.so*
    rm /usr/lib64/lib${style}common5.so*
    rm /usr/lib/qt5/plugins/styles/${style}5.so*
    rm /usr/lib64/qt5/plugins/styles/${style}5.so*
    rm /usr/share/color-schemes/${style}.colors
    rm /usr/lib/qt6/plugins/styles/${style}6.so*
    rm /usr/lib64/qt6/plugins/styles/${style}6.so*
    rm /usr/share/kstyle/themes/${style}.themerc
    rm /usr/lib/qt6/plugins/kstyle_config/${style}styleconfig.so*
    rm /usr/lib64/qt6/plugins/kstyle_config/${style}styleconfig.so*
    rm /usr/share/applications/${style}styleconfig.desktop
    rm /usr/bin/${style}-settings6
    rm /usr/share/icons/hicolor/scalable/apps/${style}-settings.svgz
    rm /usr/lib/lib${style}common6.so*
    rm /usr/lib64/lib${style}common6.so*
    rm /usr/lib/qt6/plugins/org.kde.kdecoration3/org.kde.${style}.so*
    rm /usr/lib64/qt6/plugins/org.kde.kdecoration3/org.kde.${style}.so*
    rm /usr/share/kservices6/${style}decorationconfig.desktop
    rm /usr/lib/qt6/plugins/org.kde.kdecoration3.kcm/kcm_${style}decoration.so*
    rm /usr/lib64/qt6/plugins/org.kde.kdecoration3.kcm/kcm_${style}decoration.so*
    rm /usr/share/applications/kcm_${style}decoration.desktop
    rm /usr/lib/cmake/${style}/${style}Config.cmake
    rm /usr/lib64/cmake/${style}/${style}Config.cmake
    rm -r /usr/lib/cmake/${style}
    rm -r /usr/lib64/cmake/${style}
done


