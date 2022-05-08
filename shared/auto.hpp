#pragma once

#include "rapidjson-macros/shared/serialization.hpp"

#define SPECIALIZATION(prefix, type, macro) \
prefix void Deserialize(type& value, const char*& name, const rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro(value, name); \
} \
prefix void DeserializeOptional(std::optional<type>& value, const char*& name, const rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_OPTIONAL(value, name); \
} \
prefix void DeserializeDefault(type& value, const char*& name, const type& defaultValue, const rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_DEFAULT(value, name, defaultValue); \
} \
prefix void Serialize(type& value, const char*& name, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro(value, name); \
} \
prefix void SerializeOptional(std::optional<type>& value, const char*& name, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    SERIALIZE_##macro##_OPTIONAL(value, name); \
}

#define BASIC_SPECIALIZATION(type) \
SPECIALIZATION(template<>, type, BASIC) \
SPECIALIZATION(template<>, std::vector<type>, VECTOR_BASIC)

namespace rapidjson_macros_auto {

    template<class T>
    void Deserialize(T& value, const char*& name, const rapidjson::Value& jsonValue);
    template<class T>
    void DeserializeOptional(std::optional<T>& value, const char*& name, const rapidjson::Value& jsonValue);
    template<class T>
    void DeserializeDefault(T& value, const char*& name, const T& defaultValue, const rapidjson::Value& jsonValue);
    
    template<class T>
    void Serialize(T& value, const char*& name, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);
    template<class T>
    void SerializeOptional(std::optional<T>& value, const char*& name, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);

    SPECIALIZATION(
        template<JSONClassDerived T>
        T,
        CLASS
    )
    
    SPECIALIZATION(
        template<JSONClassDerived T>
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
