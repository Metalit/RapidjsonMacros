#pragma once

#include "./auto.hpp"

// declare a struct with serialization and deserialization support using the Read and Write functions
#pragma region DECLARE_JSON_STRUCT(name, base Ts) { members; }
#define DECLARE_JSON_STRUCT(name, ...) \
struct name : rapidjson_macros_types::Parent<name __VA_OPT__(,) __VA_ARGS__>
#pragma endregion

// preserves json data not specified in class fields when reserialized
#pragma region KEEP_EXTRA_FIELDS
#define KEEP_EXTRA_FIELDS static inline constexpr bool keepExtraFields = true
#pragma endregion

// define a function that will be run when deserializing based on its position in the struct members
// parameters:
//     rapidjson::Value& jsonValue: the value the struct is currently being deserialized from
#pragma region DESERIALIZE_FUNCTION(name) { body; }
#define DESERIALIZE_FUNCTION(name) \
class _DeserializeAction_##name { \
    _DeserializeAction_##name() { \
        deserializers().emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            self->name(jsonValue); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_DeserializeAction_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_DeserializeAction_##name> instance; \
}; \
void name(rapidjson::Value& jsonValue)
#pragma endregion

// define a function that will be run when serializing based on its position in the struct members
// parameters:
//     rapidjson::Value& jsonObject: the value the struct is currently being serialized to
//     rapidjson::Document::AllocatorType& allocator: the allocator to use with jsonObject
#pragma region SERIALIZE_FUNCTION(name) { body; }
#define SERIALIZE_FUNCTION(name) \
class _SerializeAction_##name { \
    _SerializeAction_##name() { \
        serializers().emplace_back([](SelfType const* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            self->name(jsonObject, allocator); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_SerializeAction_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_SerializeAction_##name> instance; \
}; \
void name(rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) const
#pragma endregion

// define an automatically serialized / deserialized instance variable with a custom name in the json file
#pragma region NAMED_VALUE(type, name, jsonName)
#define NAMED_VALUE(type, name, jsonName) \
class _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers().emplace_back([](SelfType const* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers().emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::Deserialize(self->name, jsonName, jsonValue); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name> _##name##_JSONValueAdderInstance; \
}; \
type name
#pragma endregion

// define an automatically serialized / deserialized instance variable with a custom name in the json file and a default value
#pragma region NAMED_VALUE_DEFAULT(type, name, default, jsonName)
#define NAMED_VALUE_DEFAULT(type, name, def, jsonName) \
class _JSONValueAdder_##name { \
    _JSONValueAdder_##name() { \
        serializers().emplace_back([](SelfType const* self, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
            rapidjson_macros_auto::Serialize(self->name, jsonName, jsonObject, allocator); \
        }); \
        deserializers().emplace_back([](SelfType* self, rapidjson::Value& jsonValue) { \
            rapidjson_macros_auto::Deserialize(self->name, jsonName, def, jsonValue); \
        }); \
    } \
    friend class rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name>; \
    static inline rapidjson_macros_types::ConstructorRunner<_JSONValueAdder_##name> _##name##_JSONValueAdderInstance; \
    template <class T> \
    static type& _def(T* self = nullptr, T* jsonValue = nullptr) { \
        static type ref; \
        if constexpr (requires (T* self, T* jsonValue) { [](type x) {} (def); }) \
            return ref = def; \
        else \
            return ref = type{}; \
    } \
   public: \
    static type& GetDefault() { return _def<bool>(); } \
}; \
type name = _JSONValueAdder_##name::GetDefault()
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

// can use this instead of a name to have a vector or map serialize and deserialize from the json object itself,
// instead of being a field with a name inside the object
#define SELF_OBJECT_NAME rapidjson_macros_types::SelfValueType()

// a class that can accept multiple types
#pragma region TypeOptions<types...>
template <typename TDefault, typename... Ts>
class TypeOptions {
    static_assert(rapidjson_macros_types::all_unique<TDefault, Ts...>, "All template arguments of TypeOptions must be unique");

   private:
    template <typename T>
    static bool IsType(rapidjson::Value const& jsonValue, T& var) {
        rapidjson::Document tmp;
        tmp.CopyFrom(jsonValue, tmp.GetAllocator());
        try {
            rapidjson_macros_auto::Deserialize(var, rapidjson_macros_types::SelfValueType(), tmp);
            return true;
        } catch (JSONException const& e) {
            return false;
        }
    }
    template <typename T>
    static bool IsType(rapidjson::Value const& jsonValue) {
        T var;
        return IsType<T>(jsonValue, var);
    }
    rapidjson_macros_types::CopyableValue storedValue;
    template <typename C, typename... Cs>
    static bool CheckValueWithTypes(rapidjson::Value const& jsonValue) {
        if constexpr (sizeof...(Cs) > 0)
            return IsType<C>(jsonValue) || CheckValueWithTypes<Cs...>(jsonValue);
        return IsType<C>(jsonValue);
    }

   public:
    static void Deserialize(TypeOptions<TDefault, Ts...>* self, rapidjson::Value& jsonValue) {
        if (!CheckValueWithTypes<TDefault, Ts...>(jsonValue)) {
            throw JSONException(
                " was an unexpected type (" + rapidjson_macros_types::JsonTypeName(jsonValue) +
                "), type expected was: " + rapidjson_macros_types::CppTypeName(*self)
            );
        } else
            self->storedValue = jsonValue;
    }
    static rapidjson::Value Serialize(TypeOptions<TDefault, Ts...> const* self, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value ret;
        ret.CopyFrom(self->storedValue.document, allocator);
        return ret;
    }

    template <typename T>
    requires(std::is_convertible_v<TDefault, T> || (std::is_convertible_v<Ts, T> || ...))
    bool Is() const {
        return IsType<T>(storedValue.document);
    }
    template <typename T>
    requires(std::is_convertible_v<TDefault, T> || (std::is_convertible_v<Ts, T> || ...))
    std::optional<T> GetValue() const {
        T ret;
        if (!IsType<T>(storedValue.document, ret))
            return std::nullopt;
        return ret;
    }
    template <typename T>
    requires(std::is_convertible_v<T, TDefault> || (std::is_convertible_v<T, Ts> || ...))
    void SetValue(T&& value) {
        rapidjson_macros_types::first_convertible_t<std::remove_reference_t<T>, TDefault, Ts...> const& casted = value;
        storedValue = rapidjson_macros_serialization::SerializeValue(casted, storedValue.document.GetAllocator());
    }
    template <typename T>
    requires(std::is_convertible_v<T, TDefault> || (std::is_convertible_v<T, Ts> || ...))
    TypeOptions<TDefault, Ts...>& operator=(T&& other) {
        SetValue(other);
        return *this;
    }
    TypeOptions() { SetValue(TDefault()); };
    template <typename T>
    requires(std::is_convertible_v<T, TDefault> || (std::is_convertible_v<T, Ts> || ...))
    TypeOptions(T value) {
        SetValue(value);
    }
    TypeOptions(TypeOptions<TDefault, Ts...> const&) = default;
    bool operator==(TypeOptions<TDefault, Ts...> const&) const = default;
};
#pragma endregion

// allows the storing of unparsed json in a value, with utility methods to parse and set with other JSONClasses
#pragma region UnparsedJSON
class UnparsedJSON {
   public:
    static void Deserialize(UnparsedJSON* self, rapidjson::Value& jsonValue) { self->storedValue = jsonValue; }
    static rapidjson::Value Serialize(UnparsedJSON const* self, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value ret;
        ret.CopyFrom(self->storedValue.document, allocator);
        return ret;
    }
    template <JSONStruct T>
    T Parse() const {
        T ret;
        rapidjson::Document tmp;
        tmp.CopyFrom(storedValue.document, tmp.GetAllocator());
        try {
            T::Deserialize(&ret, tmp);
        } catch (JSONException const& e) {
            auto str = "UnparsedJSON<" + rapidjson_macros_types::CppTypeName(ret) + ">";
            throw JSONException(str + e.what());
        }
        return ret;
    }
    template <JSONStruct T>
    void Set(T const& value) {
        T::Serialize(&value, storedValue.document.GetAllocator()).Swap(storedValue.document);
    }
    template <JSONStruct T>
    UnparsedJSON& operator=(T&& other) {
        Set(other);
        return *this;
    }
    template <JSONStruct T>
    UnparsedJSON(T const& value) {
        Set(value);
    }
    UnparsedJSON() = default;
    UnparsedJSON(UnparsedJSON const&) = default;
    bool operator==(UnparsedJSON const&) const = default;

    rapidjson::Value GetValue() {
        rapidjson::Value ret;
        ret.CopyFrom(storedValue.document, storedValue.document.GetAllocator());
        return ret;
    }
    static UnparsedJSON FromValue(rapidjson::Value const& jsonValue) {
        UnparsedJSON ret;
        ret.storedValue = jsonValue;
        return ret;
    }

   private:
    rapidjson_macros_types::CopyableValue storedValue;
};
#pragma endregion
