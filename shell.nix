{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  hardeningDisable = [ "fortify" ];

  packages = with pkgs; [
    # Keep the Nix-aware clangd wrapper ahead of clang's unwrapped binary.
    clang-tools
    clang
    gnumake
    lldb
  ];
}
