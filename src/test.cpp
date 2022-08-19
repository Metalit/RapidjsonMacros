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
        try {
            WriteToFile(GetConfigPath(), testClass);
        } catch(const std::exception& e) {
            getLogger().error("error writing: %s", e.what());
        }
    }

    try {
        ReadFromFile(GetConfigPath(), testClass);
        ReadFromFile(GetConfigPath(), testClass2);
    } catch(const std::exception& e) {
        getLogger().error("error reading test class: %s", e.what());
    }
    CRASH_UNLESS(testClass == testClass2);
    try {
        WriteToFile(GetConfigPath(), testClass);
        WriteToFile(GetConfigPath(), testClass2);
    } catch(const std::exception& e) {
        getLogger().error("error writing test class: %s", e.what());
    }
	
    getLogger().info("Completed setup!");
}
