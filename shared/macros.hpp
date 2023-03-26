#pragma once

#include "./auto.hpp"

// declare a class with serialization and deserialization support using the Read and Write functions
#pragma region DECLARE_JSON_CLASS(name, fields)
#define DECLARE_JSON_CLASS(name, ...) \
class name : public JSONClass { \
    using SelfType = name; \
    friend void rapidjson_macros_serialization::DeserializeInternal<name>(name* self, rapidjson::Value const& jsonValue); \
    public: \
        __VA_ARGS__ \
    private: \
        template<class T> \
        using const_t = std::conditional_t<name::keepExtraFields, T, std::add_const_t<T>>; \
    public: \
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) const { \
            rapidjson::Value jsonObject(rapidjson::kObjectType); \
            if(keepExtraFields && extraFields.has_value()) \
                jsonObject.CopyFrom(extraFields->document, allocator); \
            for(auto& method : serializers()) \
                method(this, jsonObject, allocator); \
            return jsonObject; \
        } \
        void Deserialize(rapidjson::Value const& jsonValue) { \
            rapidjson_macros_serialization::DeserializeInternal(this, jsonValue); \
        } \
        bool operator==(class name const&) const = default; \
    private: \
        static inline std::vector<void(*)(const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)>& serializers() { \
            static std::vector<void(*)(const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)> instance = {}; \
            return instance; \
        } \
        static inline std::vector<void(*)(SelfType* self, SelfType::const_t<rapidjson::Value>& jsonValue)>& deserializers() { \
            static std::vector<void(*)(SelfType* self, SelfType::const_t<rapidjson::Value>& jsonValue)> instance = {}; \
            return instance; \
        } \
        std::optional<rapidjson_macros_types::CopyableValue> extraFields = std::nullopt; \
};
#pragma endregion

// prevents the class from preserving json data not specified in class fields and serialization
#pragma region DISCARD_EXTRA_FIELDS
#define DISCARD_EXTRA_FIELDS static inline constexpr bool keepExtraFields = false;
#pragma endregion

// add an action to be run during deserialization (requires an identifier unique to the class)
// will most likely be run in the order of fields in your class definition
#pragma region DESERIALIZE_ACTION(id, body)
#define DESERIALIZE_ACTION(uid, ...) \
class _DeserializeAction_##uid { \
    _DeserializeAction_##uid() { \
        deserializers().emplace_back([](SelfType* self, SelfType::const_t<rapidjson::Value>& jsonValue) { \
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
        serializers().emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
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
type name = {}; \
class _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers().emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers().emplace_back([](SelfType* self, SelfType::const_t<rapidjson::Value>& jsonValue) { \
            rapidjson_macros_auto::Deserialize(self->name, jsonName, jsonValue); \
            if constexpr(SelfType::keepExtraFields) \
                rapidjson_macros_serialization::RemoveMember(jsonValue, jsonName); \
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
        serializers().emplace_back([](const SelfType* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers().emplace_back([](SelfType* self, SelfType::const_t<rapidjson::Value>& jsonValue) { \
            rapidjson_macros_auto::Deserialize(self->name, jsonName, def, jsonValue); \
            if constexpr(SelfType::keepExtraFields) \
                rapidjson_macros_serialization::RemoveMember(jsonValue, jsonName); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name> _##name##_JSONValueAdderInstance; \
};
#pragma endregion

#define NAMED_VALUE_OPTIONAL(type, name, jsonName) NAMED_VALUE(std::optional<type>, name, jsonName)

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

// declare a class that can accept multiple types
#pragma region TypeOptions<types...>
template<typename... Ts>
class TypeOptions : public JSONClass {
    static_assert(rapidjson_macros_types::all_unique<Ts...>, "All template arguments of TypeOptions must be unique");
    private:
        template<typename T>
        static bool IsType(rapidjson::Value const& jsonValue, T& var) {
            try {
                rapidjson_macros_auto::Deserialize(var, rapidjson_macros_types::SelfValueType(), jsonValue);
                return true;
            } catch (std::exception const& e) {
                return false;
            }
        }
        template<typename T>
        static bool IsType(rapidjson::Value const& jsonValue) {
            T var;
            return IsType<T>(jsonValue, var);
        }
        rapidjson_macros_types::CopyableValue storedValue;
        template<typename C, typename... Cs>
        static bool CheckValueWithTypes(rapidjson::Value const& jsonValue) {
            if constexpr(sizeof...(Cs) > 0)
                return IsType<C>(jsonValue) || CheckValueWithTypes<Cs...>(jsonValue);
            return IsType<C>(jsonValue);
        }
    public:
        void Deserialize(rapidjson::Value const& jsonValue) {
            if(!CheckValueWithTypes<Ts...>(jsonValue)) {
                throw JSONException(" was an unexpected type (" +
                rapidjson_macros_types::JsonTypeName(jsonValue) + "), type expected was: " +
                rapidjson_macros_types::CppTypeName(TypeOptions<Ts...>()));
            } else
                storedValue = jsonValue;
        }
        rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) const {
            rapidjson::Value ret;
            ret.CopyFrom(storedValue.document, allocator);
            return ret;
        }

        template<typename T>
        requires (std::is_convertible_v<Ts, T> || ...)
        bool Is() const {
            return IsType<T>(storedValue.document);
        }
        template<typename T>
        requires (std::is_convertible_v<Ts, T> || ...)
        std::optional<T> GetValue() const {
            T ret;
            if(!IsType<T>(storedValue.document, ret))
                return std::nullopt;
            return ret;
        }
        template<typename T>
        requires (std::is_convertible_v<T, Ts> || ...)
        void SetValue(T&& value) {
            rapidjson_macros_types::first_convertible_t<std::remove_reference_t<T>, Ts...> const& casted = value;
            storedValue = rapidjson_macros_serialization::SerializeValue(casted, storedValue.document.GetAllocator());
        }
        template<typename T>
        requires (std::is_convertible_v<T, Ts> || ...)
        TypeOptions<Ts...>& operator=(T&& other) {
            SetValue(other);
            return *this;
        }
        TypeOptions() = default;
        template<typename T>
        requires (std::is_convertible_v<T, Ts> || ...)
        TypeOptions(T value) {
            SetValue(value);
        }
        TypeOptions(TypeOptions<Ts...> const& value) = default;
};
#pragma endregion
