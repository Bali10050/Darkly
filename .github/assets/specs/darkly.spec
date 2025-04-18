%global kf6_version 6.3.0
%define qt6_version 6.7.0
%define kf5_version 5.102.0
%define qt5_version 5.15.2
%define dev DeltaCopy
%define style Darkly
%define _style darkly
%define release_tag ${TAG} # this line gets updated automatically by Github Actions

Name:           %{style}
Version:        %{release_tag}
Release:        0
Summary:        A modern style for qt applications
License:        GPL-3.0-or-later
Group:          System/GUI/KDE
URL:            https://github.com/%{dev}/%{style}
Source0:        https://github.com/%{dev}/%{style}/archive/refs/tags/%{version}.tar.gz
BuildRequires:  cmake >= 3.16
BuildRequires:  fdupes
BuildRequires:  gettext
%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version}
BuildRequires:  extra-cmake-modules >= %{kf6_version}
BuildRequires:  kf5-rpm-macros
BuildRequires:  kf6-rpm-macros
%elif 0%{?mageia}
BuildRequires:  kf6-extra-cmake-modules >= %{kf6_version}
BuildRequires:  kf5-macros
BuildRequires:  kf6-macros

%else
#OpenSUSE
BuildRequires:  kf6-extra-cmake-modules >= %{kf6_version}
BuildRequires:  kf5-filesystem
BuildRequires:  kf6-filesystem
%endif
BuildRequires:  pkgconfig

#darkly5 dependencies
BuildRequires:  cmake(KF5Config) >= %{kf5_version}
BuildRequires:  cmake(KF5ConfigWidgets) >= %{kf5_version}
BuildRequires:  cmake(KF5CoreAddons) >= %{kf5_version}
BuildRequires:  cmake(KF5FrameworkIntegration) >= %{kf5_version}
BuildRequires:  cmake(KF5GuiAddons) >= %{kf5_version}
BuildRequires:  cmake(KF5I18n) >= %{kf5_version}
BuildRequires:  cmake(KF5IconThemes) >= %{kf5_version}
BuildRequires:  cmake(KF5Kirigami2) >= %{kf5_version}
BuildRequires:  cmake(KF5WindowSystem) >= %{kf5_version}
BuildRequires:  cmake(Qt5DBus) >= %{qt5_version}
BuildRequires:  cmake(Qt5Quick) >= %{qt5_version}
BuildRequires:  cmake(Qt5Widgets) >= %{qt5_version}

#darkly6 dependencies
BuildRequires:  cmake(KF6ColorScheme) >= %{kf6_version}
BuildRequires:  cmake(KF6Config) >= %{kf6_version}
BuildRequires:  cmake(KF6CoreAddons) >= %{kf6_version}
BuildRequires:  cmake(KF6FrameworkIntegration) >= %{kf6_version}
BuildRequires:  cmake(KF6GuiAddons) >= %{kf6_version}
BuildRequires:  cmake(KF6I18n) >= %{kf6_version}
BuildRequires:  cmake(KF6IconThemes) >= %{kf6_version}
BuildRequires:  cmake(KF6KCMUtils) >= %{kf6_version}
BuildRequires:  cmake(KF6KirigamiPlatform) >= %{kf6_version}
BuildRequires:  cmake(KF6WindowSystem) >= %{kf6_version}
BuildRequires:  cmake(Qt6Core) >= %{qt6_version}
BuildRequires:  cmake(Qt6DBus) >= %{qt6_version}
BuildRequires:  cmake(Qt6Quick) >= %{qt6_version}
BuildRequires:  cmake(Qt6Widgets) >= %{qt6_version}

%description
Darkly is a fork of breeze theme style that aims to be visually modern and minimalistic.

%prep
%autosetup -n %{name}-%{version} -p1

%build
%cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
%cmake_build

%install
%cmake_install


%fdupes %{buildroot}/%{_prefix}

%post   -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%license COPYING
%doc AUTHORS README.md

%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version}

%{_libdir}/lib%{_style}common5.so.*
%{_libdir}/lib%{_style}common6.so.*
%{_datadir}/applications/%{_style}styleconfig.desktop
%{_datadir}/applications/kcm_%{_style}decoration.desktop
%{_datadir}/kservices6/%{_style}decorationconfig.desktop
%dir %{_kf6_qtplugindir}
%dir %{_kf6_qtplugindir}/org.kde.kdecoration3.kcm
%{_kf6_qtplugindir}/org.kde.kdecoration3.kcm/kcm_%{_style}decoration.so
%dir %{_kf6_qtplugindir}/org.kde.kdecoration3/
%{_kf6_qtplugindir}/org.kde.kdecoration3/org.kde.%{_style}.so
%dir %{_kf6_qtplugindir}/kstyle_config
%{_kf6_qtplugindir}/kstyle_config/%{_style}styleconfig.so
%dir %{_kf5_qtplugindir}/styles
%{_kf5_qtplugindir}/styles/%{_style}5.so
%dir %{_kf6_qtplugindir}/styles
%{_kf6_qtplugindir}/styles/%{_style}6.so
%dir %{_datadir}/kstyle
%dir %{_datadir}/kstyle/themes
%{_datadir}/kstyle/themes/%{_style}.themerc
%{_bindir}/%{_style}-settings6
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps
%{_datadir}/icons/hicolor/scalable/apps/%{_style}-settings.svgz
%dir %{_datadir}/color-schemes/
%{_datadir}/color-schemes/%{style}.colors
%{_libdir}/cmake/%{style}/


%else
#OpenSUSE

%{_libdir}/lib%{_style}common5.so.*
%{_libdir}/lib%{_style}common6.so.*
%{_kf6_applicationsdir}/%{_style}styleconfig.desktop
%{_kf6_applicationsdir}/kcm_%{_style}decoration.desktop
%{_datadir}/kservices6/%{_style}decorationconfig.desktop
%dir %{_kf6_plugindir}
%dir %{_kf6_plugindir}/org.kde.kdecoration3.kcm
%{_kf6_plugindir}/org.kde.kdecoration3.kcm/kcm_%{_style}decoration.so
%dir %{_kf6_plugindir}/org.kde.kdecoration3/
%{_kf6_plugindir}/org.kde.kdecoration3/org.kde.%{_style}.so
%dir %{_kf6_plugindir}/kstyle_config
%{_kf6_plugindir}/kstyle_config/%{_style}styleconfig.so
%dir %{_kf5_plugindir}/styles
%{_kf5_plugindir}/styles/%{_style}5.so
%dir %{_kf6_plugindir}/styles
%{_kf6_plugindir}/styles/%{_style}6.so
%dir %{_kf6_sharedir}/kstyle
%dir %{_kf6_sharedir}/kstyle/themes
%{_kf6_sharedir}/kstyle/themes/%{_style}.themerc
%{_kf6_bindir}/%{_style}-settings6
%dir %{_kf6_iconsdir}/hicolor/scalable
%dir %{_kf6_iconsdir}/hicolor/scalable/apps
%{_kf6_iconsdir}/hicolor/scalable/apps/%{_style}-settings.svgz
%dir %{_datadir}/color-schemes/
%{_datadir}/color-schemes/%{style}.colors
%{_kf6_libdir}/cmake/%{style}/

%endif

%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version}

%changelog
%autochangelog

%else
#OpenSUSE

%changelog

%endif
