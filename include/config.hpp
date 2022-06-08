#pragma once

#include "shared/macros.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"

Logger& getLogger();

DECLARE_JSON_CLASS(RapidjsonMacros, SmallSubclass,
    AUTO_VALUE_DEFAULT(double, DoubleValue, 1.0 / 2.5)
)

DECLARE_JSON_CLASS(RapidjsonMacros, Subclass,
    MANUAL_DESERIALIZE_METHOD
    MANUAL_SERIALIZE_METHOD
    int IntValue;
    float FloatValue = 0;
    std::optional<std::string> StringValue;
)
SERIALIZE_METHOD(RapidjsonMacros, Subclass,
    SERIALIZE_VALUE(IntValue, "CustomNamedIntValue")
    SERIALIZE_VALUE(FloatValue, "FloatValue")
    SERIALIZE_VALUE_OPTIONAL(StringValue, "StringValue")
    getLogger().info("Serializing subclass!");
)
DESERIALIZE_METHOD(RapidjsonMacros, Subclass,
    DESERIALIZE_VALUE(IntValue, "CustomNamedIntValue")
    DESERIALIZE_VALUE_DEFAULT(FloatValue, "FloatValue", 0)
    DESERIALIZE_VALUE_OPTIONAL(StringValue, "StringValue")
    getLogger().info("Deserializing subclass!");
)

DECLARE_JSON_CLASS(RapidjsonMacros, AutoSubclass,
    NAMED_AUTO_VALUE(int, IntValue, "CustomNamedIntValue")
    AUTO_VALUE_DEFAULT(float, FloatValue, 0)
    AUTO_VALUE_OPTIONAL(std::string, StringValue)
    SERIALIZE_ACTION(0,
        getLogger().info("Serializing auto subclass!");
    )
    DESERIALIZE_ACTION(0,
        getLogger().info("Deserializing auto subclass!");
    )
)

DECLARE_JSON_CLASS(RapidjsonMacros, TestClass,
    MANUAL_DESERIALIZE_METHOD
    MANUAL_SERIALIZE_METHOD
    bool BoolValue;
    std::vector<int> IntVector = std::vector<int>({0, 1, 2, 3});
    std::optional<std::vector<float>> FloatVector;
    RapidjsonMacros::Subclass Subclass;
    RapidjsonMacros::AutoSubclass AutoSubclass;
    std::vector<RapidjsonMacros::SmallSubclass> SubclassVector;
)
SERIALIZE_METHOD(RapidjsonMacros, TestClass,
    SERIALIZE_VALUE(BoolValue, "BoolValue")
    SERIALIZE_VECTOR_BASIC(IntVector, "IntVector")
    SERIALIZE_VECTOR_BASIC_OPTIONAL(FloatVector, "FloatVector")
    SERIALIZE_CLASS(Subclass, "Subclass")
    SERIALIZE_CLASS(AutoSubclass, "AutoSubclass")
    SERIALIZE_VECTOR(SubclassVector, "SubclassVector")
)
DESERIALIZE_METHOD(RapidjsonMacros, TestClass,
    DESERIALIZE_VALUE(BoolValue, "BoolValue")
    DESERIALIZE_VECTOR_BASIC_DEFAULT(IntVector, "IntVector", std::vector<int>({0, 1, 2, 3}))
    DESERIALIZE_VECTOR_BASIC_OPTIONAL(FloatVector, "FloatVector")
    DESERIALIZE_CLASS(Subclass, "Subclass")
    DESERIALIZE_CLASS(AutoSubclass, "AutoSubclass")
    DESERIALIZE_VECTOR(SubclassVector, "SubclassVector")
)

DECLARE_JSON_CLASS(RapidjsonMacros, AutoTestClass,
    AUTO_VALUE(bool, BoolValue)
    AUTO_VECTOR_DEFAULT(int, IntVector, std::vector<int>({0, 1, 2, 3}))
    AUTO_VALUE_OPTIONAL(std::vector<float>, FloatVector)
    AUTO_VALUE(RapidjsonMacros::Subclass, Subclass)
    AUTO_VALUE(RapidjsonMacros::AutoSubclass, AutoSubclass)
    AUTO_VECTOR(RapidjsonMacros::SmallSubclass, SubclassVector)
)

extern RapidjsonMacros::TestClass testClass;
extern RapidjsonMacros::AutoTestClass autoTestClass;
