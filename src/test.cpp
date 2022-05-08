#include "config.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

static ModInfo modInfo;

RapidjsonMacros::TestClass testClass;
RapidjsonMacros::AutoTestClass autoTestClass;

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
        WriteToFile(GetConfigPath(), testClass);
    }

    ReadFromFile(GetConfigPath(), testClass);
    ReadFromFile(GetConfigPath(), autoTestClass);
	
    getLogger().info("Completed setup!");
}