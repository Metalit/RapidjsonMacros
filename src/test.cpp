#include "config.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

static ModInfo modInfo;

RapidjsonMacros::TestClass testClass;
RapidjsonMacros::TestClass testClass2;
RapidjsonMacros::AutoTestClass autoTestClass;
RapidjsonMacros::AutoTestClass autoTestClass2;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

std::string& GetConfigPath() {
    static std::string configPath = Configuration::getConfigFilePath(modInfo);
    return configPath;
}

void setup(ModInfo& info) {
    info.id = ID;
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
        ReadFromFile(GetConfigPath(), autoTestClass);
        ReadFromFile(GetConfigPath(), autoTestClass2);
    } catch(const std::exception& e) {
        getLogger().error("error reading auto test class: %s", e.what());
    }
    CRASH_UNLESS(autoTestClass == autoTestClass2);
	
    getLogger().info("Completed setup!");
}