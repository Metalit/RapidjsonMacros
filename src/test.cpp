#include "test.hpp"

#pragma region all_unique
static_assert(rapidjson_macros_types::all_unique<int, float, std::string, bool>);
static_assert(!rapidjson_macros_types::all_unique<int, int>);
static_assert(!rapidjson_macros_types::all_unique<std::string, std::string>);
static_assert(!rapidjson_macros_types::all_unique<int, bool, int>);
static_assert(!rapidjson_macros_types::all_unique<int, int, bool>);
#pragma endregion

#pragma region first_convertible_t
static_assert(std::is_convertible_v<int, float>);
static_assert(std::is_convertible_v<int, double>);
static_assert(!std::is_convertible_v<int, std::string>);
static_assert(std::is_same_v<rapidjson_macros_types::first_convertible_t<int, float, double>, float>);
static_assert(std::is_same_v<rapidjson_macros_types::first_convertible_t<int, double, float>, double>);
static_assert(std::is_same_v<rapidjson_macros_types::first_convertible_t<int, std::string>, int>);
static_assert(std::is_same_v<rapidjson_macros_types::first_convertible_t<int, std::string, float>, float>);
#pragma endregion

#pragma region ConstructorRunner
static bool ConstructorRunner_run = false;
struct ConstructorRunner_test {
    ConstructorRunner_test() { ConstructorRunner_run = true; }
};
static rapidjson_macros_types::ConstructorRunner<ConstructorRunner_test> ConstructorRunner_runner;
#pragma endregion

int main(int argc, char** args) {
    assert(ConstructorRunner_run);

    RapidjsonMacros::TestClass testcls;
    testcls.testval = 1;
    RapidjsonMacros::TestClass testcls2;
    testcls2.testval = 2;
    assert(testcls != testcls2);
    testcls2.testval = 1;
    assert(testcls == testcls2);

    assert(testcls.testval_multi_def = -1);

    std::cout << WriteToString(RapidjsonMacros::TestClass()) << "\n";

    ReadFromString("{\"testval\":0,\"testval_def\":0,\"testval_self_def\":0,\"why do this\":4}", testcls);
    assert(testcls.testval == 0);
    assert(testcls.testval_def == 0);
    assert(testcls.testval_self_def == 0);
    assert(testcls.testval_json_def == 4);
    assert(testcls.testval_multi_def == 2);
    ReadFromString("{\"testval\":0,\"testval_def\":0,\"why do this\":4}", testcls);
    assert(testcls.testval == 0);
    assert(testcls.testval_def == 0);
    assert(testcls.testval_self_def == 2);
    assert(testcls.testval_json_def == 4);
    assert(testcls.testval_multi_def == 2);
    testcls.def = 3;
    ReadFromString("{\"testval\":0,\"why do this\":4, \"testvec_bool\": [true, false, true]}", testcls);
    assert(testcls.testval == 0);
    assert(testcls.testval_def == 1);
    assert(testcls.testval_self_def == 3);
    assert(testcls.testval_json_def == 4);
    assert(testcls.testvec_bool.size() == 3);
    assert(testcls.testvec_bool[0] == true);
    assert(testcls.testvec_bool[1] == false);
    assert(testcls.testvec_bool[2] == true);

    std::cout << "Completed test!\n";
    return 0;
}
