let
  pinnedNixpkgs = import (builtins.fetchTarball {
    name = "pinned-nixpkgs-for-urweb-school";
    url = https://github.com/NixOS/nixpkgs/archive/19.09.tar.gz;
    # Hash obtained using `nix-prefetch-url --unpack <url>`
    sha256 = "0mhqhq21y5vrr1f30qd2bvydv4bbbslvyzclhw0kdxmkgg3z4c92";
  }) {};
in
{ pkgs ? pinnedNixpkgs }: pkgs.callPackage ./default.nix {}
