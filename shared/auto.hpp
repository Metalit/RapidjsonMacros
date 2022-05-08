#pragma once

#include "../shared/serialization.hpp"

#define SPECIALIZATION(prefix, type, macro) \
prefix void Deserialize(type& var, const char* const& jsonName, const rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro(var, jsonName); \
} \
prefix void DeserializeOptional(std::optional<type>& var, const char* const& jsonName, const rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_OPTIONAL(var, jsonName); \
} \
prefix void DeserializeDefault(type& var, const char* const& jsonName, const auto& defaultValue, const rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_DEFAULT(var, jsonName, defaultValue); \
} \
prefix void Serialize(type& var, const char* const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro(var, jsonName); \
} \
prefix void SerializeOptional(std::optional<type>& var, const char* const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro##_OPTIONAL(var, jsonName); \
}

#define BASIC_SPECIALIZATION(type) \
SPECIALIZATION(template<>, type, VALUE) \
SPECIALIZATION(template<>, std::vector<type>, VECTOR_BASIC)

namespace rapidjson_macros_auto {

    template<class T>
    void Deserialize(T& var, const char* const& jsonName, const rapidjson::Value& jsonValue);
    template<class T>
    void DeserializeOptional(std::optional<T>& var, const char* const& jsonName, const rapidjson::Value& jsonValue);
    template<class T>
    void DeserializeDefault(T& var, const char* const& jsonName, const auto& defaultValue, const rapidjson::Value& jsonValue);
    
    template<class T>
    void Serialize(T& var, const char* const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);
    template<class T>
    void SerializeOptional(std::optional<T>& var, const char* const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);

    SPECIALIZATION(
        template<JSONClassDerived T>,
        T,
        CLASS
    )
    
    SPECIALIZATION(
        template<JSONClassDerived T>,
        std::vector<T>,
        VECTOR
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
