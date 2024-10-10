$triplet = "x64-windows"

if ($null -eq $env:VCPKG_ROOT -or (Test-Path $env:VCPKG_ROOT) -eq $false) {
    Write-Error "VCPKG_ROOT not found"
}

$rapidjson = Join-Path $env:VCPKG_ROOT "installed/$triplet/include/rapidjson"

if ((Test-Path $rapidjson) -eq $false) {
    Write-Error "rapidjson not found"
}

g++ -std=c++20 -DRAPIDJSON_MACROS_GCC_TEST -I"$rapidjson" -Iinclude -Ishared ./src/*.cpp -o rapidjsontest.exe
