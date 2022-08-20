#pragma once

#include "../shared/auto.hpp"

// declare a class with serialization and deserialization support using ReadFromFile and WriteToFile
#define DECLARE_JSON_CLASS(namespaze, name, ...) \
namespace namespaze { \
    class name : public JSONClass { \
        using SelfType = name; \
        private: \
            static inline std::vector<void(*)(SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)> serializers; \
            static inline std::vector<void(*)(SelfType* self, rapidjson::Value& jsonValue)> deserializers; \
            rapidjson_macros_types::CopyableValue extraFields; \
            static inline bool keepExtraFields = true; \
        public: \
            rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) { \
                rapidjson::Value jsonObject(rapidjson::kObjectType); \
                if(keepExtraFields) \
                    jsonObject.CopyFrom(extraFields.document, allocator); \
                for(auto& method : serializers) \
                    method(this, jsonObject, allocator); \
                return jsonObject; \
            } \
            void Deserialize(rapidjson::Value& jsonValue) { \
                for(auto& method : deserializers) \
                    method(this, jsonValue); \
                if(keepExtraFields) \
                    extraFields = jsonValue; \
            } \
            bool operator==(const class name&) const = default; \
            __VA_ARGS__ \
    }; \
}

// prevents the class from preserving json data not specified in class fields and serialization
#define DISCARD_EXTRA_FIELDS \
private: \
friend struct _ExtraFieldBoolSetter; \
struct _ExtraFieldBoolSetter { \
    _ExtraFieldBoolSetter() { \
        SelfType::keepExtraFields = false; \
    } \
}; \
static inline _ExtraFieldBoolSetter _ExtraFieldBoolSetterInstance;

// add an action to be run during deserialization (requires an identifier unique to the class)
// will most likely be run in the order of fields in your class definition
#define DESERIALIZE_ACTION(uid, ...) \
private: \
struct _DeserializeAction_##uid { \
    _DeserializeAction_##uid() { \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
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
        serializers.emplace_back([](SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            __VA_ARGS__ \
        }); \
    } \
}; \
static inline _SerializeAction_##uid _##uid##_SerializeActionInstance;

// define an automatically serialized / deserialized instance variable with a custom name in the json file
#define NAMED_VALUE(type, name, jsonName) \
public: \
type name; \
private: \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::Deserialize(self->name, jsonName, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

// define an automatically serialized / deserialized std::optional instance variable with a custom name in the json file
#define NAMED_VALUE_OPTIONAL(type, name, jsonName) \
public: \
std::optional<type> name = std::nullopt; \
private: \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::SerializeOptional(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeOptional(self->name, jsonName, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

// define an automatically serialized / deserialized instance variable with a custom name in the json file and a default value
#define NAMED_VALUE_DEFAULT(type, name, def, jsonName) \
public: \
type name = def; \
private: \
struct _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeDefault(self->name, jsonName, def, jsonValue); \
        }); \
    } \
}; \
static inline _JSONValueAdder_##name _##name##_JSONValueAdderInstance;

// define an automatically serialized / deserialized std::vector with a custom name in the json file
#define NAMED_VECTOR(type, name, jsonName) NAMED_VALUE(std::vector<type>, name, jsonName)

// define an automatically serialized / deserialized std::optional<std::vector> with a custom name in the json file
#define NAMED_VECTOR_OPTIONAL(type, name, jsonName) NAMED_VALUE_OPTIONAL(std::vector<type>, name, jsonName)

// define an automatically serialized / deserialized std::vector with a custom name in the json file and a default value
#define NAMED_VECTOR_DEFAULT(type, name, def, jsonName) NAMED_VALUE_DEFAULT(std::vector<type>, name, def, jsonName)

// versions of the macros above that use the name of the instance variable as the name in the json file
#define VALUE(type, name) NAMED_VALUE(type, name, #name)
#define VALUE_OPTIONAL(type, name) NAMED_VALUE_OPTIONAL(type, name, #name)
#define VALUE_DEFAULT(type, name, def) NAMED_VALUE_DEFAULT(type, name, def, #name)

#define VECTOR(type, name) NAMED_VECTOR(type, name, #name)
#define VECTOR_OPTIONAL(type, name) NAMED_VECTOR_OPTIONAL(type, name, #name)
#define VECTOR_DEFAULT(type, name, def) NAMED_VECTOR_DEFAULT(type, name, def, #name)

// multiple candidate names can be used for deserialization, and the first name will be used for serialization
#define NAME_OPTS(...) std::vector({__VA_ARGS__})
