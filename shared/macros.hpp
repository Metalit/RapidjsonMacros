#pragma once

#include "../shared/auto.hpp"

#define DECLARE_JSON_CLASS(namespaze, name, ...) \
namespace namespaze { \
    class name : public JSONClass { \
        public: \
            __VA_ARGS__ \
    }; \
}

#define MANUAL_DESERIALIZE_METHOD void Deserialize(const rapidjson::Value& jsonValue);

#define MANUAL_SERIALIZE_METHOD rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator);

#define DESERIALIZE_METHOD(namespaze, name, ...) \
void namespaze::name::Deserialize(const rapidjson::Value& jsonValue) { \
    __VA_ARGS__ \
    for(auto& method : deserializers) \
        method(jsonValue); \
}

#define SERIALIZE_METHOD(namespaze, name, ...) \
rapidjson::Value namespaze::name::Serialize(rapidjson::Document::AllocatorType& allocator) { \
    rapidjson::Value jsonObject(rapidjson::kObjectType); \
    __VA_ARGS__ \
    for(auto& method : serializers) \
        method(jsonObject, allocator); \
    return jsonObject; \
}

#define AUTO_VALUE(type, name) \
type name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        rapidjson_macros_auto::Serialize(name, #name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        rapidjson_macros_auto::Deserialize(name, #name, jsonValue); \
    }); \
    return type(); \
})();

#define AUTO_VALUE_OPTIONAL(type, name) \
std::optional<type> name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        rapidjson_macros_auto::SerializeOptional(name, #name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        rapidjson_macros_auto::DeserializeOptional(name, #name, jsonValue); \
    }); \
    return std::nullopt; \
})();

#define AUTO_VALUE_DEFAULT(type, name, def) \
type name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        rapidjson_macros_auto::Serialize(name, #name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        rapidjson_macros_auto::DeserializeDefault(name, #name, def, jsonValue); \
    }); \
    return def; \
})();

#define AUTO_VECTOR(type, name) AUTO_VALUE(std::vector<type>, name)

#define AUTO_VECTOR_OPTIONAL(type, name) AUTO_VALUE_OPTIONAL(std::vector<type>, name)

#define AUTO_VECTOR_DEFAULT(type, name, def) AUTO_VALUE_DEFAULT(std::vector<type>, name, def)
