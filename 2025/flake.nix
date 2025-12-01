{
  description = "OCaml dev environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        ocamlPkgs = pkgs.ocamlPackages;
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            ocamlPkgs.ocaml
            ocamlPkgs.dune_2
            ocamlPkgs.findlib
            ocamlPkgs.merlin
            ocamlPkgs.ocamlformat
          ];

          shellHook = ''
          echo Hello from the camel!
          '';
        };


        packages.default = ocamlPkgs.ocaml;
      });
}
