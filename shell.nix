{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
	nativeBuildInputs = with pkgs.buildPackages; [
	  gcc15
	  clang-tools
	  hping3
	  iproute2
	];
  }
