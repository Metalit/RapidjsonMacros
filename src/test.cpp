#include "config.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

static modloader::ModInfo modInfo = {MOD_ID, VERSION, 0};

RapidjsonMacros::TestClass testClass;
RapidjsonMacros::TestClass testClass2;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, {false, true});
    return *logger;
}

std::string& GetConfigPath() {
    static std::string configPath = Configuration::getConfigFilePath(modInfo);
    return configPath;
}

extern "C" void setup(CModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    info.version_long = 0;

    modInfo.assign(info);

    if(!fileexists(GetConfigPath())) {
        if(!WriteToFile(GetConfigPath(), testClass))
            getLogger().error("error writing to config");
    }

    try {
        ReadFromFile(GetConfigPath(), testClass);
        ReadFromFile(GetConfigPath(), testClass2);
        CRASH_UNLESS(testClass == testClass2);
    } catch(const std::exception& e) {
        getLogger().error("error reading test class: %s", e.what());
    }

    testClass.BoolValue = false;
    testClass.FlexibleValue = 0.5;
    testClass.FlexibleValue = 256;
    testClass.Map["string1"] = "string1 value";
    if(testClass.FloatVector && testClass.FloatVector->size() > 0)
        testClass.FloatVector->back() = 0.1;
    testClass.IntVector.emplace_back(9);

    testClass.Subclass.SBDValue = 3.14;
    testClass.Subclass.SBDValue.SetValue(true);
    testClass.Subclass.SBDValue = "string";
    testClass.Subclass.FloatValue = -1.1;

    while(testClass.SubclassVector.size() < 3)
        testClass.SubclassVector.push_back({});
    testClass.SubclassVector[0].IntMap["one"] = 0;
    testClass.SubclassVector[1].IntMap["two"] = 1;
    testClass.SubclassVector[2].IntMap["three"] = 2;

    testClass.Vector2D.push_back({(int) testClass.Vector2D.size(), (int) testClass.Vector2D.size() + 1});

    bool isFloat = testClass.SuperFlexibleValue.Is<float>();
    if(isFloat) {
        auto value = RapidjsonMacros::SmallerSubclass();
        value.Int = 1;
        testClass.SuperFlexibleValue.SetValue(value);
    } else
        testClass.SuperFlexibleValue = 0;

    WriteToFile(GetConfigPath(), testClass);

    getLogger().info("Completed test!");
}
