{
  description = "Configure G300 and G300s Logitech mice";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-21.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      rec {
        name = "ratslap";
        packages.default = (with nixpkgs.legacyPackages.${system};
          stdenv.mkDerivation {
            name = name;
            src = ./.;

            nativeBuildInputs = [
              pkg-config
              installShellFiles
              git
              which
              ctags
              python310Packages.markdown
            ];

            buildInputs = [
              libusb
            ];

            buildPhase = ''
              REV="${self."rev" or "dirty-worktree"}"
              APPVER="${builtins.substring 0 8 (self."shortRev" or "dirty")}-flake"
              TIME="@${toString self.lastModified}"

              make \
                -W gitup \
                VDIRTY="" \
                APPBRANCH="" \
                MAJVER="" \
                APPVER="$APPVER" \
                BUILD_COMMIT="$REV" \
                BINNAME="${name}" \
                BUILD_DATE="`date --date=$TIME +'%Y-%m-%d %H:%M:%S%z'`" \
                BUILD_MONTH="`date --date=$TIME +'%B'`" \
                BUILD_YEAR="`date --date=$TIME +'%Y'`" \
                manpage.1 \
                ${name}
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp ${name} $out/bin

              mv manpage.1 ${name}.1
              installManPage ${name}.1
            '';
          });

        apps.default = flake-utils.lib.mkApp {
          drv = packages.default;
          exePath = "/bin/${name}";
        };
      }
    );
}
