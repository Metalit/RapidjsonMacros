#include "config.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

static ModInfo modInfo;

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

extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

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
    testClass.Subclass.SBDValue = true;
    testClass.Subclass.SBDValue = "string";
    testClass.Subclass.FloatValue = -1.1;

    while(testClass.SubclassVector.size() < 3)
        testClass.SubclassVector.push_back({});
    testClass.SubclassVector[0].IntMap["one"] = 0;
    testClass.SubclassVector[1].IntMap["two"] = 1;
    testClass.SubclassVector[2].IntMap["three"] = 2;

    WriteToFile(GetConfigPath(), testClass);

    getLogger().info("Completed setup!");
}
