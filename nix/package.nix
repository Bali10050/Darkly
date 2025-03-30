{
  lib,
  stdenv,
  cmake,
  kdePackages,
  qtPackages ? kdePackages,
}:
let
  inherit (builtins) baseNameOf;
  inherit (lib.sources) cleanSourceWith cleanSource;
  inherit (lib.strings) hasSuffix;
  qtMajorVersion = lib.versions.major qtPackages.qtbase.version;
in
stdenv.mkDerivation (finalAttrs: {
  pname = "darkly-qt${qtMajorVersion}";
  version = lib.removeSuffix "\n" (builtins.readFile ../VERSION);

  src = cleanSourceWith {
    filter =
      name: _type:
      let
        baseName = baseNameOf (toString name);
      in
      !(hasSuffix ".nix" baseName);
    src = cleanSource ../.;
  };

  buildInputs =
    with qtPackages;
    [
      qtbase
      kcmutils
      kcoreaddons
      kiconthemes
      kwindowsystem
    ]
    ++ lib.optionals (qtMajorVersion == "5") [
      kirigami2
    ]
    ++ lib.optionals (qtMajorVersion == "6") [
      kcolorscheme
      kdecoration
      kirigami
    ];

  nativeBuildInputs = [
    cmake
    qtPackages.wrapQtAppsHook
    qtPackages.extra-cmake-modules
  ];

  cmakeFlags = map (v: lib.cmakeBool "BUILD_QT${v}" (v == qtMajorVersion)) [
    "5"
    "6"
  ];

  outputs = [
    "out"
    "dev"
  ];

  meta = with lib; {
    description = "Fork of the Darkly breeze theme style that aims to be visually modern and minimalistic";
    mainProgram = "darkly-settings6";
    homepage = "https://github.com/Bali10050/Darkly";
    license = licenses.gpl2Plus;
    platforms = platforms.all;
  };
})
