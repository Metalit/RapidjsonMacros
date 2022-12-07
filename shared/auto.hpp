#pragma once

#include "../shared/serialization.hpp"

#define SPECIALIZATION(type, macro, ...) \
__VA_ARGS__ void Deserialize(type& var, S const& jsonName, rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro(var, jsonName); \
} \
__VA_ARGS__ void DeserializeOptional(std::optional<type>& var, S const& jsonName, rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_OPTIONAL(var, jsonName); \
} \
__VA_ARGS__ void DeserializeDefault(type& var, S const& jsonName, const type& defaultValue, rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_DEFAULT(var, jsonName, defaultValue); \
} \
__VA_ARGS__ void Serialize(const type& var, S const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro(var, jsonName); \
} \
__VA_ARGS__ void SerializeOptional(const std::optional<type>& var, S const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro##_OPTIONAL(var, jsonName); \
}

#define BASIC_SPECIALIZATION(type) \
SPECIALIZATION(type, VALUE, template<class S>) \
SPECIALIZATION(std::vector<type>, VECTOR_BASIC, template<class S>)

namespace rapidjson_macros_auto {

    template<class T, class S>
    void Deserialize(T& var, S const& jsonName, rapidjson::Value& jsonValue);
    template<class T, class S>
    void DeserializeOptional(std::optional<T>& var, S const& jsonName, rapidjson::Value& jsonValue);
    template<class T, class S>
    void DeserializeDefault(T& var, S const& jsonName, const T& defaultValue, rapidjson::Value& jsonValue);

    template<class T, class S>
    void Serialize(const T& var, S const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);
    template<class T, class S>
    void SerializeOptional(const std::optional<T>& var, S const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);

    SPECIALIZATION(
        T,
        CLASS,
        template<JSONClassDerived T, class S>
    )

    SPECIALIZATION(
        std::vector<T>,
        VECTOR,
        template<JSONClassDerived T, class S>
    )

    BASIC_SPECIALIZATION(bool)
    BASIC_SPECIALIZATION(int)
    BASIC_SPECIALIZATION(unsigned)
    BASIC_SPECIALIZATION(int64_t)
    BASIC_SPECIALIZATION(uint64_t)
    BASIC_SPECIALIZATION(double)
    BASIC_SPECIALIZATION(float)
    BASIC_SPECIALIZATION(std::string)
}

#undef SPECIALIZATION
#undef BASIC_SPECIALIZATION
