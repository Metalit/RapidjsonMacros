
$rapidjson="C:/Path/vcpkg/packages/rapidjson_x64-mingw-dynamic/include"
$rapidjson_macros="shared/rapidjson-macros"

g++ -std=c++20 -DRAPIDJSON_MACROS_GCC_TEST -I"$rapidjson" -I"$rapidjson_macros" -Iinclude -Ishared ./src/*.cpp -o rapidjsontest.exe
