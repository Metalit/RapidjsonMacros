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

    if(!fileexists(CONFIG_PATH)) {
        try {
            WriteToFile(CONFIG_PATH, testClass);
        } catch(const std::exception& e) {
            getLogger().error("error writing: %s", e.what());
        }
    }

    try {
        ReadFromFile(CONFIG_PATH, testClass);
    } catch(const std::exception& e) {
        getLogger().error("error reading test class 1: %s", e.what());
    }
    try {
        ReadFromFile(CONFIG_PATH, autoTestClass);
    } catch(const std::exception& e) {
        getLogger().error("error reading test class 2: %s", e.what());
    }
	
    getLogger().info("Completed setup!");
}