{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {self, nixpkgs, flake-utils} :
    flake-utils.lib.eachSystem ["x86_64-linux"]
      (system:
        let 
          pkgs = import nixpkgs { system = system; };
          p = import ./default.nix {
            stdenv = pkgs.stdenv;
            curl = pkgs.curl;
            gcc = pkgs.gcc;
            urweb = pkgs.urweb;
            icu = pkgs.icu;
          };
        in
          {
            defaultPackage = p;
            ur-package = p;
          }
      );
}
