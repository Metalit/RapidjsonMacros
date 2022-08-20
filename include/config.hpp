#pragma once

#include "shared/macros.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"

Logger& getLogger();

DECLARE_JSON_CLASS(RapidjsonMacros, SmallSubclass,
    VALUE_DEFAULT(double, DoubleValue, 1.0 / 2.5)
)

DECLARE_JSON_CLASS(RapidjsonMacros, Subclass,
    NAMED_VALUE(bool, BoolValue, NAME_OPTS("BoolValueName1", "BoolValueName2", "BoolValueName3"))
    NAMED_VALUE(int, IntValue, "CustomNamedIntValue")
    VALUE_DEFAULT(float, FloatValue, 0)
    VALUE_OPTIONAL(std::string, StringValue)
    SERIALIZE_ACTION(0,
        getLogger().info("Serializing subclass!");
    )
    DESERIALIZE_ACTION(0,
        getLogger().info("Deserializing subclass!");
    )
    DISCARD_EXTRA_FIELDS
)

DECLARE_JSON_CLASS(RapidjsonMacros, TestClass,
    VALUE(bool, BoolValue)
    VECTOR_DEFAULT(int, IntVector, std::vector<int>({0, 1, 2, 3}))
    VALUE_OPTIONAL(std::vector<float>, FloatVector)
    VALUE(RapidjsonMacros::Subclass, Subclass)
    VECTOR(RapidjsonMacros::SmallSubclass, SubclassVector)
)

extern RapidjsonMacros::TestClass testClass;
