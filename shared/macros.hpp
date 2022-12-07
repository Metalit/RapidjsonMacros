#pragma once

#include "../shared/options.hpp"
#include "../shared/auto.hpp"

// declare a class with serialization and deserialization support using the Read and Write functions
#pragma region DECLARE_JSON_CLASS(name, fields)
#define DECLARE_JSON_CLASS(name, ...) \
class name : public JSONClass { \
    using SelfType = name; \
    private: \
        static inline std::vector<void(*)(const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)> serializers; \
        static inline std::vector<void(*)(SelfType* self, rapidjson::Value& jsonValue)> deserializers; \
        std::optional<rapidjson_macros_types::CopyableValue> extraFields = std::nullopt; \
        static inline bool keepExtraFields = true; \
    public: \
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) const { \
            rapidjson::Value jsonObject(rapidjson::kObjectType); \
            if(keepExtraFields && extraFields.has_value()) \
                jsonObject.CopyFrom(extraFields->document, allocator); \
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
        name() = default; \
        __VA_ARGS__ \
};
#pragma endregion

// prevents the class from preserving json data not specified in class fields and serialization
#pragma region DISCARD_EXTRA_FIELDS
#define DISCARD_EXTRA_FIELDS \
class _ExtraFieldBoolSetter { \
    _ExtraFieldBoolSetter() { \
        SelfType::keepExtraFields = false; \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_ExtraFieldBoolSetter>; \
    static inline rapidjson_macros_types::ConstructorRunner<_ExtraFieldBoolSetter> _ExtraFieldBoolSetterInstance; \
};
#pragma endregion

// add an action to be run during deserialization (requires an identifier unique to the class)
// will most likely be run in the order of fields in your class definition
#pragma region DESERIALIZE_ACTION(id, body)
#define DESERIALIZE_ACTION(uid, ...) \
class _DeserializeAction_##uid { \
    _DeserializeAction_##uid() { \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            __VA_ARGS__ \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_DeserializeAction_##uid>; \
    static inline rapidjson_macros_types::ConstructorRunner<_DeserializeAction_##uid> _##uid##_DeserializeActionInstance; \
};
#pragma endregion

// add an action to be run during serialization (requires an identifier unique to the class)
// will most likely be run in the order of fields in your class definition
#pragma region SERIALIZE_ACTION(id, body)
#define SERIALIZE_ACTION(uid, ...) \
class _SerializeAction_##uid { \
    _SerializeAction_##uid() { \
        serializers.emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            __VA_ARGS__ \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_SerializeAction_##uid>; \
    static inline rapidjson_macros_types::ConstructorRunner<_SerializeAction_##uid> _##uid##_SerializeActionInstance; \
};
#pragma endregion

// define an automatically serialized / deserialized instance variable with a custom name in the json file
#pragma region NAMED_VALUE(type, name, jsonName)
#define NAMED_VALUE(type, name, jsonName) \
type name; \
class _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::Deserialize(self->name, jsonName, jsonValue); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name> _##name##_JSONValueAdderInstance; \
};
#pragma endregion

// define an automatically serialized / deserialized std::optional instance variable with a custom name in the json file
#pragma region NAMED_VALUE_OPTIONAL(type, name, jsonName)
#define NAMED_VALUE_OPTIONAL(type, name, jsonName) \
std::optional<type> name = std::nullopt; \
class _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::SerializeOptional(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeOptional(self->name, jsonName, jsonValue); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name> _##name##_JSONValueAdderInstance; \
};
#pragma endregion

// define an automatically serialized / deserialized instance variable with a custom name in the json file and a default value
#pragma region NAMED_VALUE_DEFAULT(type, name, default, jsonName)
#define NAMED_VALUE_DEFAULT(type, name, def, jsonName) \
type name = def; \
class _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers.emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers.emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::DeserializeDefault(self->name, jsonName, def, jsonValue); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name> _##name##_JSONValueAdderInstance; \
};
#pragma endregion

// define an automatically serialized / deserialized std::vector with a custom name in the json file
#define NAMED_VECTOR(type, name, jsonName) NAMED_VALUE(std::vector<type>, name, jsonName)
// define an automatically serialized / deserialized std::optional<std::vector> with a custom name in the json file
#define NAMED_VECTOR_OPTIONAL(type, name, jsonName) NAMED_VALUE_OPTIONAL(std::vector<type>, name, jsonName)
// define an automatically serialized / deserialized std::vector with a custom name in the json file and a default value
#define NAMED_VECTOR_DEFAULT(type, name, def, jsonName) NAMED_VALUE_DEFAULT(std::vector<type>, name, def, jsonName)

// define an automatically serialized / deserialized string keyed std::map with a custom name in the json file
#define NAMED_MAP(type, name, jsonName) NAMED_VALUE(StringKeyedMap<type>, name, jsonName)
// define an automatically serialized / deserialized string keyed std::optional<std::map> with a custom name in the json file
#define NAMED_MAP_OPTIONAL(type, name, jsonName) NAMED_VALUE_OPTIONAL(StringKeyedMap<type>, name, jsonName)
// define an automatically serialized / deserialized string keyed std::map with a custom name in the json file and a default value
#define NAMED_MAP_DEFAULT(type, name, def, jsonName) NAMED_VALUE_DEFAULT(StringKeyedMap<type>, name, def, jsonName)

// versions of the macros above that use the name of the instance variable as the name in the json file
#define VALUE(type, name) NAMED_VALUE(type, name, #name)
#define VALUE_OPTIONAL(type, name) NAMED_VALUE_OPTIONAL(type, name, #name)
#define VALUE_DEFAULT(type, name, def) NAMED_VALUE_DEFAULT(type, name, def, #name)

#define VECTOR(type, name) NAMED_VECTOR(type, name, #name)
#define VECTOR_OPTIONAL(type, name) NAMED_VECTOR_OPTIONAL(type, name, #name)
#define VECTOR_DEFAULT(type, name, def) NAMED_VECTOR_DEFAULT(type, name, def, #name)

#define MAP(type, name) NAMED_MAP(type, name, #name)
#define MAP_OPTIONAL(type, name) NAMED_MAP_OPTIONAL(type, name, #name)
#define MAP_DEFAULT(type, name, def) NAMED_MAP_DEFAULT(type, name, def, #name)

// multiple candidate names can be used for deserialization, and the first name will be used for serialization
#define NAME_OPTS(...) std::vector({__VA_ARGS__})

// declare a class that can accept multiple non-class types, priority determined by order
#pragma region TYPE_OPTS_CLASS(name, types)
#define TYPE_OPTS_CLASS(name, ...) RAPIDJSON_MACROS_GET_TOC_MACRO(name, __VA_ARGS__, \
RAPIDJSON_MACROS_TOC_8, \
RAPIDJSON_MACROS_TOC_7, \
RAPIDJSON_MACROS_TOC_6, \
RAPIDJSON_MACROS_TOC_5, \
RAPIDJSON_MACROS_TOC_4, \
RAPIDJSON_MACROS_TOC_3, \
RAPIDJSON_MACROS_TOC_2 \
)(name, __VA_ARGS__)
#pragma endregion
