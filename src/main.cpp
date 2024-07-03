#include "test.hpp"

#define OBJECT_PATH "test_objects/test.json"

int main(int argc, char** args) {
    RapidjsonMacros::TestClass testClass;

    if (!fileexists(OBJECT_PATH))
        WriteToFile(OBJECT_PATH, testClass);

    ReadFromFile(OBJECT_PATH, testClass);

    std::cout << "Completed test!\n";

    return 0;
}
