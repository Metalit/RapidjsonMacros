#pragma once

#include "shared/macros.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"

Logger& getLogger();

DECLARE_JSON_CLASS(RapidjsonMacros, SmallSubclass,
    AUTO_VALUE_DEFAULT(double, DoubleValue, 1.0 / 2.5)
)

DECLARE_JSON_CLASS(RapidjsonMacros, Subclass,
    NAMED_AUTO_VALUE(int, IntValue, "CustomNamedIntValue")
    AUTO_VALUE_DEFAULT(float, FloatValue, 0)
    AUTO_VALUE_OPTIONAL(std::string, StringValue)
    SERIALIZE_ACTION(0,
        getLogger().info("Serializing subclass!");
    )
    DESERIALIZE_ACTION(0,
        getLogger().info("Deserializing subclass!");
    )
    DISCARD_EXTRA_FIELDS
)

DECLARE_JSON_CLASS(RapidjsonMacros, TestClass,
    AUTO_VALUE(bool, BoolValue)
    AUTO_VECTOR_DEFAULT(int, IntVector, std::vector<int>({0, 1, 2, 3}))
    AUTO_VALUE_OPTIONAL(std::vector<float>, FloatVector)
    AUTO_VALUE(RapidjsonMacros::Subclass, Subclass)
    AUTO_VECTOR(RapidjsonMacros::SmallSubclass, SubclassVector)
)

extern RapidjsonMacros::TestClass testClass;
