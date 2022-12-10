#pragma once

#include "shared/macros.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"

Logger& getLogger();

namespace RapidjsonMacros {
    DECLARE_JSON_CLASS(SmallSubclass,
        VALUE_DEFAULT(double, DoubleValue, 1.0 / 2.5)
        VALUE(std::vector<std::string>, StringVector)
        MAP_DEFAULT(int, IntMap, StringKeyedMap<int>({
            {"one", 1},
            {"two", 2},
            {"three", 3}
        }))
    )

    DECLARE_JSON_CLASS(Subclass,
        NAMED_VALUE(bool, BoolValue, NAME_OPTS("BoolValueName1", "BoolValueName2", "BoolValueName3"))
        NAMED_VALUE(int, IntValue, "CustomNamedIntValue")
        VALUE_DEFAULT(float, FloatValue, 0)
        using StringOrBoolOrDouble = TypeOptions<std::string, bool, double>;
        VALUE_DEFAULT(StringOrBoolOrDouble, SBDValue, 25.1)
        private:
        VALUE_OPTIONAL(std::string, StringValue)
        SERIALIZE_ACTION(0,
            getLogger().info("Serializing subclass!");
            bool privateVariablesAccessible = self->StringValue.has_value();
        )
        DESERIALIZE_ACTION(0,
            getLogger().info("Deserializing subclass!");
            bool privateVariablesAccessible = self->StringValue.has_value();
        )
        DISCARD_EXTRA_FIELDS
    )

    DECLARE_JSON_CLASS(TestClass,
        VALUE(bool, BoolValue)
        VECTOR_DEFAULT(int, IntVector, std::vector<int>({0, 1, 2, 3}))
        VALUE_OPTIONAL(std::vector<float>, FloatVector)
        VALUE(Subclass, Subclass)
        VECTOR(SmallSubclass, SubclassVector)
        MAP(std::string, Map)
        using IntOrFloat = TypeOptions<int, float>;
        NAMED_VALUE(IntOrFloat, FlexibleValue, "Give me an int or a float!")
    )
}
