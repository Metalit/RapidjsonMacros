# RapidjsonMacros

A library of macros to create c++ structs that can validate and parse almost any json structure.

## How To Use

This is a QPM package, so run `qpm dependency add rapidjson-macros` and `qpm restore`. It's also used in the `config-utils` package.

Documentation can be found in the comments in `macros.hpp`.

## Local Test

Make sure gcc is on path and VCPKG_ROOT is set. Run `vcpkg install rapidjson` and optionally update the path in `build.ps1` if needed. Then run `build.ps1` and `test.ps1`.
