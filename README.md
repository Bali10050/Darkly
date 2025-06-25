
# About this fork

This fork attempts to revive lightly with a different approach from boehs, with the assumption that Luwx won't ever resume his project.

This means that this fork allows real changes, and not just maintainence.

![Darkly](https://github.com/user-attachments/assets/0d60725e-c146-4e1e-9889-fe8317d52b22)

## What changed?

- [x] Redesign the tabbar
- [x] Make the buttons sligthly larger
- [x] Change the defaults to fit the normal use-cases better
- [x] Add my own colorscheme to it
- [x] Make installation in all major distros easy
- [x] Rename the project to something like „Darkly”
- [x] Maintain the project

## Installation

## Automatic

### Thanks to @DeltaCopy, you can use one of these install methods:

#### Fedora copr

<https://copr.fedorainfracloud.org/coprs/deltacopy/darkly/>

#### AUR

<https://aur.archlinux.org/packages/darkly>

<https://aur.archlinux.org/packages/darkly-bin>

#### Pre-built packages

<https://github.com/Bali10050/Darkly/releases>

***
#### Solus
Thanks to @Hurican-Solas

```
sudo eopkg install darkly
```


## Manual

### To ensure this application style works on applications still using QT5 both QT5 and QT6 dependencies are required to be installed before building from source.

### Installation script

> [!NOTE]
> A script called `install.sh` is now available which both builds and installs this application style.

`./install.sh` will remove if existing, build and install Darkly using both QT5/QT6 dependencies.

`./install.sh QT5` will build & install using only QT5/KF5 dependencies.

`./install.sh QT6` will build & install using only QT6/KF6 dependencies.

`./install.sh remove` will remove Darkly.

***

### Flatpak

Due to flatpak's [restriction on file system access](https://docs.flatpak.org/en/latest/sandbox-permissions.html) (in short no access to system libraries, /dev, etc), Darkly flatpak bundle is only an extension of KDE runtime.\
If you want to apply Darkly as the application style, you either need:
- Darkly installed on the system via package manager (pacman, rpm, apt-get, etc) or
- Set manually with QT_STYLE_OVERRIDE=Darkly env variable \[`sudo flatpak override --env=QT_QPA_PLATFORMTHEME=kde` (`sudo` only applies for flatpaks in /var/lib/flatpak, otherwise use `--user` without sudo)]

**Important (primarily) for users of immutable distros!**\
Darkly won't show as an option in `System settings > Theme > Application styles` without system installation.

This brings some untentended behaviour (for any application style, not Darkly in particular), for example:
- Changing color scheme partly applies for QWidget apps (not at all for QML-based)
- Changing application style doesn't apply until the app is restarted

Manifests should have the latest versions of KDE Runtime and SDK. Change the version accordingly. (you can find the version of runtime for each app by running `flatpak list --app --columns=app,runtime`)

#### Build with:
```
org.flatpak.Builder flatpak-build --repo=local --force-clean --ccache org.kde.KStyle.Darkly6.json
```

use `org.kde.KStyle.Darkly5.json` for KF5

#### Bundle with:
```
flatpak build-bundle local/ darkly.flatpak runtime/org.kde.KStyle.Darkly/x86_64/<runtime_version>
```

***

#### Void Linux

```
sudo xbps-install -Sy git extra-cmake-modules base-devel qt6-base qt6-base-devel qt6-tools-devel \
      kf6-kcmutils-devel kf6-kconfigwidgets-devel kf6-kdecoration-devel \
      kf6-kirigami-devel kf6-kcoreaddons-devel kf6-kcolorscheme-devel \
      kf6-kconfig-devel kf6-kguiaddons-devel kf6-ki18n-devel kf6-kiconthemes-devel \
      kf6-kwindowsystem-devel kf6-frameworkintegration-devel kf6-karchive-devel \
      kf6-kcodecs-devel kf6-kwidgetsaddons-devel qt6-declarative-devel \
      qt6-svg-devel qt6-wayland-devel kf6-kwidgetsaddons-devel kf6-knotifications-devel
```

```
git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
./install.sh QT6
```

---

#### <u>Arch Linux</u>

```
sudo pacman -S --noconfirm cmake extra-cmake-modules kdecoration qt6-declarative kcoreaddons \
      kcmutils kcolorscheme kconfig kguiaddons kiconthemes kwindowsystem git \
      qt5-declarative qt5-x11extras gcc make kcmutils5 \
      frameworkintegration5 kconfigwidgets5 kiconthemes5 \
      kirigami2 kwindowsystem5
```

```
git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
./install.sh
```

---

#### <u>Fedora</u>

##### Fedora 40/41

```
sudo dnf install -y git cmake extra-cmake-modules "cmake(KDecoration3)" kwin-devel \
      kf6-kcolorscheme-devel kf6-kguiaddons-devel kf6-ki18n-devel kf6-kiconthemes-devel \
      kf6-kirigami-devel kf6-kcmutils-devel kf6-frameworkintegration-devel \
      libepoxy-devel "cmake(Qt5Core)" "cmake(Qt5Gui)" "cmake(Qt5DBus)" "cmake(KF5GuiAddons)" \
      "cmake(KF5WindowSystem)" "cmake(KF5I18n)" "cmake(KF5CoreAddons)" "cmake(KF5ConfigWidgets)" \
      "cmake(Qt5UiTools)" "cmake(KF5GlobalAccel)" "cmake(KF5IconThemes)" "cmake(KF5Init)" \
      "cmake(KF5KIO)" kf5-kpackage-devel kf5-kcmutils-devel qt5-qtquickcontrols2-devel \
      kf5-kirigami2-devel "cmake(KF5FrameworkIntegration)"
```

```
git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
./install.sh
```

---

#### <u>openSUSE Tumbleweed</u>

```
sudo zypper in --no-recommends git ninja cmake kf6-extra-cmake-modules kf6-kconfig-devel \
      kf6-frameworkintegration-devel gmp-ecm-devel kf6-kconfigwidgets-devel \
      kf6-kguiaddons-devel kf6-ki18n-devel kf6-kiconthemes-devel kf6-kwindowsystem-devel \
      kf6-kcolorscheme-devel kf6-kcoreaddons-devel kf6-kcmutils-devel \
      qt6-quick-devel kf6-kirigami-devel qt6-base-devel kdecoration6-devel \
      qt6-tools qt6-widgets-devel gcc-c++ extra-cmake-modules libQt5Gui-devel \
      libQt5DBus-devel libqt5-qttools-devel libqt5-qtx11extras-devel \
      libQt5OpenGL-devel libQt5Network-devel libepoxy-devel kconfig-devel \
      kconfigwidgets-devel kcrash-devel kglobalaccel-devel ki18n-devel kio-devel \
      kservice-devel kinit-devel knotifications-devel kwindowsystem-devel kguiaddons-devel \
      kiconthemes-devel kpackage-devel kwin5-devel xcb-util-devel xcb-util-cursor-devel \
      xcb-util-wm-devel xcb-util-keysyms-devel kcmutils-devel \
      libqt5-qtquick3d-devel kirigami2-devel libKF5I18n5
```

```
git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
./install.sh
```

---

#### <u>KDE neon</u>

```
sudo apt install -y git build-essential cmake kf6-extra-cmake-modules \
      kf6-extra-cmake-modules kf6-frameworkintegration-dev \
      kf6-kcmutils-dev kf6-kcolorscheme-dev kf6-kconfig-dev kf6-kconfigwidgets-dev \
      kf6-kcoreaddons-dev kf6-kguiaddons-dev kf6-ki18n-dev kf6-kiconthemes-dev \
      kf6-kirigami2-dev kf6-kpackage-dev kf6-kservice-dev kf6-kwindowsystem-dev \
      kirigami2-dev kwayland-dev libx11-dev libkdecorations2-dev libkf5config-dev \
      libkf5configwidgets-dev libkf5coreaddons-dev libkf5guiaddons-dev libkf5i18n-dev \
      libkf5iconthemes-dev libkf5kcmutils-dev libkf5package-dev libkf5service-dev \
      libkf5style-dev libkf5wayland-dev libkf5windowsystem-dev libplasma-dev \
      libqt5x11extras5-dev qt6-base-dev qt6-declarative-dev qtbase5-dev \
      qtdeclarative5-dev gettext qt6-svg-dev extra-cmake-modules qt3d5-dev
```

```
git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
./install.sh
```

---

#### <u>Distrobox (Fedora 41)</u>

```
distrobox create --name lightly --image registry.fedoraproject.org/fedora-toolbox:41
distrobox enter lightly
```

```
sudo dnf install -y git cmake extra-cmake-modules "cmake(KDecoration3)" kwin-devel \
    kf6-kcolorscheme-devel kf6-kguiaddons-devel kf6-ki18n-devel kf6-kiconthemes-devel \
    kf6-kirigami-devel kf6-kcmutils-devel \
    libepoxy-devel "cmake(Qt5Core)" "cmake(Qt5Gui)" "cmake(Qt5DBus)" "cmake(KF5GuiAddons)" \
    "cmake(KF5WindowSystem)" "cmake(KF5I18n)" "cmake(KF5CoreAddons)" "cmake(KF5ConfigWidgets)" \
    "cmake(Qt5UiTools)" "cmake(KF5GlobalAccel)" "cmake(KF5IconThemes)" "cmake(KF5Init)" \
    "cmake(KF5KIO)" kf5-kpackage-devel kf5-kcmutils-devel qt5-qtquickcontrols2-devel \
    kf5-kirigami2-devel "cmake(KF5FrameworkIntegration)"

git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_LIBDIR=lib64 \
      -DBUILD_TESTING=OFF ..

cd ./kdecoration/config/
make -j $(nproc)
cd ../../
make -j $(nproc)
make install
```

```
Set environment variable on plasma startup:

echo "export QT_PLUGIN_PATH=$HOME/.local/lib64/plugins:\$QT_PLUGIN_PATH" > $HOME/.config/plasma-workspace/env/localthemes.sh && chmod +x $HOME/.config/plasma-workspace/env/localthemes.sh
```

---

#### <u>Kubuntu (25.04)</u>

```
sudo apt-get install -y -qq cmake build-essential libkf5config-dev libkdecorations3-dev \
      libqt5x11extras5-dev qtdeclarative5-dev extra-cmake-modules \
      libkf5guiaddons-dev libkf5configwidgets-dev libkf5windowsystem-dev kirigami2-dev \
      libkf5coreaddons-dev libkf5iconthemes-dev gettext qt3d5-dev libkf5kcmutils-dev \
      qt6-base-dev libkf6coreaddons-dev libkf6colorscheme-dev \
      libkf6config-dev libkf6guiaddons-dev libkf6i18n-dev libkf6iconthemes-dev \
      libkf6windowsystem-dev libkf6kcmutils-dev libkirigami-dev libkf6style-dev
```

```
git clone --single-branch --depth=1 https://github.com/Bali10050/Darkly.git
cd Darkly
./install.sh
```

***

#### <u>NixOS</u>

To install Darkly on NixOS, add the following line to your nixos flake
```nix
environment.systemPackages = with pkgs; [ darkly-qt5 darkly ];
qt.platformTheme = "qt5ct";
```
For home-manager, add the following line
```nix
qt.style.package = with pkgs; [ darkly-qt5 darkly ];
qt.platformTheme.name = "qtct";
```
To apply the theme, use qt5ct or qt6ct and select Darkly

If you want to compile Darkly from its source, do the following
1. add `inputs.darkly.url = "github:Bali10050/Darkly";` to `flake.nix`

2. use the following package names for home-manager or NixOS
```nix
inputs.darkly.packages.${pkgs.system}.darkly-qt5
inputs.darkly.packages.${pkgs.system}.darkly-qt6
```


---


## Known issues & solutions

### Blurred icon rendering on Wayland with fractional scaling

As referenced in https://github.com/Bali10050/Darkly/issues/14

On Wayland when scaling is set to >100% some icons appear blurred.

The fix for now until QT is updated is to add `QT_SCALE_FACTOR_ROUNDING_POLICY=RoundPreferFloor` into the file `/etc/environment`

Restart to apply the changes.

For further details see: https://bugs.kde.org/show_bug.cgi?id=479891

### Issues with window decorations on older plasma versions
For plasma versions below 6.3, please use [v0.5.16](https://github.com/Bali10050/Darkly/releases/tag/v0.5.16) or the [Darkly\(6.2\)](https://github.com/Bali10050/Darkly/tree/Darkly(6.2)) branch.
