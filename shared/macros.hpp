#pragma once

#include "../shared/auto.hpp"

// declare a class with serialization and deserialization support using ReadFromFile and WriteToFile
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
            bool operator==(const class name&) const = default; \
            __VA_ARGS__ \
    }; \
}

// declare a manual deserialize method to add custom code defined in DESERIALIZE_METHOD
// code specified will run after automatic values and actions
#define MANUAL_DESERIALIZE_METHOD void Deserialize(const rapidjson::Value& jsonValue);

// declare a manual serialize method to add custom code defined in SERIALIZE_METHOD
// code specified will run after automatic values and actions
#define MANUAL_SERIALIZE_METHOD rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator);

// use this macro to define a custom deserialize method outside of your class definition
// code specified will run after automatic values and actions
#define DESERIALIZE_METHOD(namespaze, name, ...) \
void namespaze::name::Deserialize(const rapidjson::Value& jsonValue) { \
    _Deserialize(jsonValue); \
    __VA_ARGS__ \
}

// use this macro to define a custom serialize method outside of your class definition
// code specified will run after automatic values and actions
#define SERIALIZE_METHOD(namespaze, name, ...) \
rapidjson::Value namespaze::name::Serialize(rapidjson::Document::AllocatorType& allocator) { \
    rapidjson::Value jsonObject(rapidjson::kObjectType); \
    _Serialize(jsonObject, allocator); \
    __VA_ARGS__ \
    return jsonObject; \
}

// add an action to be run during deserialization (requires an identifier unique to the class)
// will most likely be run in the order of fields in your class definition
#define DESERIALIZE_ACTION(uid, ...) \
private: \
struct _DeserializeAction_##uid { \
    _DeserializeAction_##uid() { \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            __VA_ARGS__ \
        }); \
    } \
}; \
static inline _DeserializeAction_##uid _##uid##_DeserializeActionInstance;

// add an action to be run during serialization (requires an identifier unique to the class)
// will most likely be run in the order of fields in your class definition
#define SERIALIZE_ACTION(uid, ...) \
private: \
struct _SerializeAction_##uid { \
    _SerializeAction_##uid() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            __VA_ARGS__ \
        }); \
    } \
}; \
static inline _SerializeAction_##uid _##uid##_SerializeActionInstance;

// define an automatically serialized / deserialized instance variable with a custom name in the json file
#define NAMED_AUTO_VALUE(type, name, jsonName) \
public: \
type name; \
private: \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(outerClass->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::Deserialize(outerClass->name, jsonName, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

// define an automatically serialized / deserialized std::optional instance variable with a custom name in the json file
#define NAMED_AUTO_VALUE_OPTIONAL(type, name, jsonName) \
public: \
std::optional<type> name = std::nullopt; \
private: \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::SerializeOptional(outerClass->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeOptional(outerClass->name, jsonName, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

// define an automatically serialized / deserialized instance variable with a custom name in the json file and a default value
#define NAMED_AUTO_VALUE_DEFAULT(type, name, def, jsonName) \
public: \
type name = def; \
private: \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](auto* outerClass, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(outerClass->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](auto* outerClass, const rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeDefault(outerClass->name, jsonName, def, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

// define an automatically serialized / deserialized std::vector with a custom name in the json file
#define NAMED_AUTO_VECTOR(type, name, jsonName) NAMED_AUTO_VALUE(std::vector<type>, name, jsonName)

// define an automatically serialized / deserialized std::optional<std::vector> with a custom name in the json file
#define NAMED_AUTO_VECTOR_OPTIONAL(type, name, jsonName) NAMED_AUTO_VALUE_OPTIONAL(std::vector<type>, name, jsonName)

// define an automatically serialized / deserialized std::vector with a custom name in the json file and a default value
#define NAMED_AUTO_VECTOR_DEFAULT(type, name, def, jsonName) NAMED_AUTO_VALUE_DEFAULT(std::vector<type>, name, def, jsonName)

// versions of the macros above that use the name of the instance variable as the name in the json file
#define AUTO_VALUE(type, name) NAMED_AUTO_VALUE(type, name, #name)
#define AUTO_VALUE_OPTIONAL(type, name) NAMED_AUTO_VALUE_OPTIONAL(type, name, #name)
#define AUTO_VALUE_DEFAULT(type, name, def) NAMED_AUTO_VALUE_DEFAULT(type, name, def, #name)

#define AUTO_VECTOR(type, name) NAMED_AUTO_VECTOR(type, name, #name)
#define AUTO_VECTOR_OPTIONAL(type, name) NAMED_AUTO_VECTOR_OPTIONAL(type, name, #name)
#define AUTO_VECTOR_DEFAULT(type, name, def) NAMED_AUTO_VECTOR_DEFAULT(type, name, def, #name)
