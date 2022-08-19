#pragma once

#include "../shared/serialization.hpp"

#define SPECIALIZATION(type, macro, ...) \
__VA_ARGS__ void Deserialize(type& var, const char (&jsonName)[N], rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro(var, jsonName); \
} \
__VA_ARGS__ void DeserializeOptional(std::optional<type>& var, const char (&jsonName)[N], rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_OPTIONAL(var, jsonName); \
} \
__VA_ARGS__ void DeserializeDefault(type& var, const char (&jsonName)[N], const type& defaultValue, rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_DEFAULT(var, jsonName, defaultValue); \
} \
__VA_ARGS__ void Serialize(type& var, const char (&jsonName)[N], rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro(var, jsonName); \
} \
__VA_ARGS__ void SerializeOptional(std::optional<type>& var, const char (&jsonName)[N], rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro##_OPTIONAL(var, jsonName); \
}

#define BASIC_SPECIALIZATION(type) \
SPECIALIZATION(type, VALUE, template<std::size_t N = 0>) \
SPECIALIZATION(std::vector<type>, VECTOR_BASIC, template<std::size_t N = 0>)

namespace rapidjson_macros_auto {

    template<class T, std::size_t N = 0>
    void Deserialize(T& var, const char (&jsonName)[N], rapidjson::Value& jsonValue);
    template<class T, std::size_t N = 0>
    void DeserializeOptional(std::optional<T>& var, const char (&jsonName)[N], rapidjson::Value& jsonValue);
    template<class T, std::size_t N = 0>
    void DeserializeDefault(T& var, const char (&jsonName)[N], const T& defaultValue, rapidjson::Value& jsonValue);
    
    template<class T, std::size_t N = 0>
    void Serialize(T& var, const char (&jsonName)[N], rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);
    template<class T, std::size_t N = 0>
    void SerializeOptional(std::optional<T>& var, const char (&jsonName)[N], rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);

    SPECIALIZATION(
        T,
        CLASS,
        template<JSONClassDerived T, std::size_t N = 0>
    )
    
    SPECIALIZATION(
        std::vector<T>,
        VECTOR,
        template<JSONClassDerived T, std::size_t N = 0>
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
