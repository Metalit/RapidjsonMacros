#pragma once

#include "../shared/auto.hpp"

#define DECLARE_JSON_CLASS(namespaze, name, ...) \
namespace namespaze { \
    class name : public JSONClass { \
        private: \
            static inline std::vector<void(*)(name* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)> serializers; \
            static inline std::vector<void(*)(name* outerClass, const rapidjson::Value& jsonValue)> deserializers; \
            void _Serialize(rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
                for(auto& method : serializers) \
                    method(this, jsonObject, allocator); \
            } \
            void _Deserialize(const rapidjson::Value& jsonValue) { \
                for(auto& method : deserializers) \
                    method(this, jsonValue); \
            } \
        public: \
            __VA_ARGS__ \
    }; \
}

#define MANUAL_DESERIALIZE_METHOD void Deserialize(const rapidjson::Value& jsonValue);

#define MANUAL_SERIALIZE_METHOD rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator);

#define DESERIALIZE_METHOD(namespaze, name, ...) \
void namespaze::name::Deserialize(const rapidjson::Value& jsonValue) { \
    __VA_ARGS__ \
    _Deserialize(jsonValue); \
}

#define SERIALIZE_METHOD(namespaze, name, ...) \
rapidjson::Value namespaze::name::Serialize(rapidjson::Document::AllocatorType& allocator) { \
    rapidjson::Value jsonObject(rapidjson::kObjectType); \
    __VA_ARGS__ \
    _Serialize(jsonObject, allocator); \
    return jsonObject; \
}

#define AUTO_VALUE(type, name) \
type name; \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(outerClass->name, #name, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::Deserialize(outerClass->name, #name, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

#define AUTO_VALUE_OPTIONAL(type, name) \
std::optional<type> name = std::nullopt; \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::SerializeOptional(outerClass->name, #name, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeOptional(outerClass->name, #name, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

#define AUTO_VALUE_DEFAULT(type, name, def) \
type name = def; \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(outerClass->name, #name, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeDefault(outerClass->name, #name, def, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

#define AUTO_VECTOR(type, name) AUTO_VALUE(std::vector<type>, name)

#define AUTO_VECTOR_OPTIONAL(type, name) AUTO_VALUE_OPTIONAL(std::vector<type>, name)

#define AUTO_VECTOR_DEFAULT(type, name, def) AUTO_VALUE_DEFAULT(std::vector<type>, name, def)
