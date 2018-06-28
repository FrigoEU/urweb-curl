let
  nixpkgs = import <nixpkgs> {};
in 
  nixpkgs.stdenv.mkDerivation {
    name = "urweb-curl";
    src = ./.;
    system = builtins.currentSystem;
    buildInputs = [nixpkgs.gcc nixpkgs.curl];
    buildPhase = nixpkgs.writeScript "builder.sh" ''
    ${nixpkgs.gcc}/bin/gcc -c -I${nixpkgs.urweb}/include/urweb -I${nixpkgs.curl.dev}/include -Isrc/c -o src/c/curl.o -Wimplicit -Wall -Werror -Wno-deprecated-declarations src/c/curl.c
    '';
    installPhase = nixpkgs.writeScript "installer.sh" ''
      mkdir $out
      cp -r . $out
    '';
  }
