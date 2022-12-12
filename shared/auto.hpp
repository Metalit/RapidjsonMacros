#pragma once

#include "../shared/serialization.hpp"

#define SPECIALIZATION(type, macro, ...) \
__VA_ARGS__ void Deserialize(type& var, S const& jsonName, rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro(var, jsonName); \
} \
__VA_ARGS__ void DeserializeOptional(std::optional<type>& var, S const& jsonName, rapidjson::Value& jsonValue) { \
    DESERIALIZE_##macro##_OPTIONAL(var, jsonName); \
} \
__VA_ARGS__ void DeserializeDefault(type& var, S const& jsonName, const rapidjson_macros_types::with_constructible<type> auto& defaultValue, rapidjson::Value& jsonValue) { \
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
SPECIALIZATION(std::vector<type>, VECTOR_BASIC, template<class S>) \
SPECIALIZATION(StringKeyedMap<type>, MAP_BASIC, template<class S>)

namespace rapidjson_macros_auto {

    template<class T, class S>
    void Deserialize(T& var, S const& jsonName, rapidjson::Value& jsonValue);
    template<class T, class S>
    void DeserializeOptional(std::optional<T>& var, S const& jsonName, rapidjson::Value& jsonValue);
    template<class T, class S>
    void DeserializeDefault(T& var, S const& jsonName, const rapidjson_macros_types::with_constructible<T> auto& defaultValue, rapidjson::Value& jsonValue);

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

    SPECIALIZATION(
        StringKeyedMap<T>,
        MAP,
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

#pragma region undefs
#undef SPECIALIZATION
#undef BASIC_SPECIALIZATION

#undef RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING
#undef RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING
#undef RAPIDJSON_MACROS_EXCEPTION_CONTEXT

#undef DESERIALIZE_VALUE
#undef DESERIALIZE_VALUE_OPTIONAL
#undef DESERIALIZE_VALUE_DEFAULT
#undef DESERIALIZE_CLASS
#undef DESERIALIZE_CLASS_OPTIONAL
#undef DESERIALIZE_CLASS_DEFAULT
#undef DESERIALIZE_VECTOR
#undef DESERIALIZE_VECTOR_OPTIONAL
#undef DESERIALIZE_VECTOR_DEFAULT
#undef DESERIALIZE_VECTOR_BASIC
#undef DESERIALIZE_VECTOR_BASIC_OPTIONAL
#undef DESERIALIZE_VECTOR_BASIC_DEFAULT
#undef DESERIALIZE_MAP
#undef DESERIALIZE_MAP_OPTIONAL
#undef DESERIALIZE_MAP_DEFAULT
#undef DESERIALIZE_MAP_BASIC
#undef DESERIALIZE_MAP_BASIC_OPTIONAL
#undef DESERIALIZE_MAP_BASIC_DEFAULT

#undef SERIALIZE_VALUE
#undef SERIALIZE_VALUE_OPTIONAL
#undef SERIALIZE_CLASS
#undef SERIALIZE_CLASS_OPTIONAL
#undef SERIALIZE_VECTOR
#undef SERIALIZE_VECTOR_OPTIONAL
#undef SERIALIZE_VECTOR_BASIC
#undef SERIALIZE_VECTOR_BASIC_OPTIONAL
#undef SERIALIZE_MAP
#undef SERIALIZE_MAP_OPTIONAL
#undef SERIALIZE_MAP_BASIC
#undef SERIALIZE_MAP_BASIC_OPTIONAL
#pragma endregion
