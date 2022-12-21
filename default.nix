{ pkgs ? import <nixpkgs> { }
}:

with pkgs;
stdenv.mkDerivation
rec {
  pname = "ratslap";
  version = "0.4.1";

  src = ./.;

  nativeBuildInputs = [
    pkg-config
    installShellFiles
    git
  ];

  buildInputs = [
    libusb
  ];

  buildPhase = ''
    make \
      -W gitup \
      VDIRTY="" \
      BUILD_DATE="`git show -s --date=format:'%Y-%m-%d %H:%M:%S%z' --format=%cd`" \
      BUILD_MONTH="`git show -s --date=format:'%B' --format=%cd`" \
      BUILD_YEAR="`git show -s --date=format:'%Y' --format=%cd`" \
      manpage.1 \
      ${pname}
    
      # MAJVER="${version}" \
      # APPBRANCH="main" \
      # BINNAME="${pname}" \
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp ratslap $out/bin

    mv manpage.1 ${pname}.1
    installManPage ${pname}.1
  '';
}
