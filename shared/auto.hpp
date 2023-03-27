#pragma once

#include "./serialization.hpp"

#define TYPE_EXCEPTION_STRING(json, cpp) " was an unexpected type (" + JsonTypeName(json) + "), type expected was: " + CppTypeName(cpp)
#define THROW_TYPE_EXCEPTION_FALLBACK(json, cpp) [&jsonRef = json, &cppRef = cpp]() { throw JSONException(TYPE_EXCEPTION_STRING(jsonRef, cppRef)); }
#define THROW_NOT_FOUND_EXCEPTION_FALLBACK [&jsonName]() { throw JSONException(GetNameString(jsonName) + " was not found"); }

namespace rapidjson_macros_auto {
    using namespace rapidjson_macros_serialization;
    using namespace rapidjson_macros_types;

#pragma region simple
    template<class T>
    void Deserialize(T& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto&& [value, success] = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        try {
            DeserializeValue(value, var, THROW_TYPE_EXCEPTION_FALLBACK(jsonValue, var));
        } catch(JSONException const& e) {
            throw JSONException(GetNameString(jsonName) + e.what());
        }
    }
    template<class T>
    void Deserialize(std::optional<T>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if(!success)
            return;
        try {
            if(!DeserializeValue(value, var, fallback))
                return;
        } catch(JSONException const& e) {
            throw JSONException(GetNameString(jsonName) + e.what());
        }
    }
    template<class T, with_constructible<T> D = T>
    void Deserialize(T& var, auto const& jsonName, D const& defaultValue, rapidjson::Value const& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if(!success)
            return;
        try {
            if(!DeserializeValue(value, var, fallback))
                return;
        } catch(JSONException const& e) {
            throw JSONException(GetNameString(jsonName) + e.what());
        }
    }

    template<class T>
    void Serialize(T const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        auto name = GetJSONString(GetDefaultName(jsonName), allocator);
        jsonObject.AddMember(name, SerializeValue(var, allocator), allocator);
    }
    template<class T>
    void Serialize(std::optional<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        if(!var.has_value()) return;
        auto name = GetJSONString(GetDefaultName(jsonName), allocator);
        jsonObject.AddMember(name, SerializeValue(var, allocator), allocator);
    }
#pragma endregion

#pragma region vector
    template<class T>
    void Deserialize(std::vector<T>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto&& [value, success] = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        if(!value.IsArray())
            throw JSONException(GetNameString(jsonName) + TYPE_EXCEPTION_STRING(jsonValue, var));
        var.clear();
        for(auto it = value.Begin(); it != value.End(); ++it) {
            auto& inst = var.emplace_back(NewType(var));
            try {
                DeserializeValue(*it, inst, THROW_TYPE_EXCEPTION_FALLBACK(*it, inst));
            } catch(JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]" + e.what());
            }
        }
    }
    template<class T>
    void Deserialize(std::optional<std::vector<T>>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if(!success)
            return;
        if(!value.IsArray())
            return fallback();
        if(!var)
            var.emplace();
        var->clear();
        for(auto it = value.Begin(); it != value.End(); ++it) {
            auto& inst = var->emplace_back(NewType(var));
            try {
                if(!DeserializeValue(*it, inst, fallback))
                    return;
            } catch(JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]" + e.what());
            }
        }
    }
    template<class T, with_constructible<std::vector<T>> D = std::vector<T>>
    void Deserialize(std::vector<T>& var, auto const& jsonName, D const& defaultValue, rapidjson::Value const& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if(!success)
            return;
        if(!value.IsArray())
            return fallback();
        var.clear();
        for(auto it = value.Begin(); it != value.End(); ++it) {
            auto& inst = var.emplace_back(NewType(var));
            try {
                if(!DeserializeValue(*it, inst, fallback))
                    return;
            } catch(JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]" + e.what());
            }
        }
    }

    template<class T>
    void Serialize(std::vector<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kArrayType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for(auto const& element : var) {
            newValue.GetArray().PushBack(SerializeValue(element, allocator), allocator);
        }
        if constexpr(!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
    template<class T>
    void Serialize(std::optional<std::vector<T>> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        if(!var.has_value()) return;
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kArrayType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for(auto const& element : var.value()) {
            newValue.GetArray().PushBack(SerializeValue(element, allocator), allocator);
        }
        if constexpr(!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
#pragma endregion

#pragma region map
    template<class T>
    void Deserialize(StringKeyedMap<T>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto&& [value, success] = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        if(!value.IsObject())
            throw JSONException(GetNameString(jsonName) + TYPE_EXCEPTION_STRING(jsonValue, var));
        var.clear();
        for(auto& member : value.GetObject()) {
            auto& inst = var[member.name.GetString()] = NewType(var);
            try {
                DeserializeValue(member.value, inst, THROW_TYPE_EXCEPTION_FALLBACK(member.value, inst));
            } catch(JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + member.name.GetString() + "]" + e.what());
            }
        }
    }
    template<class T>
    void Deserialize(std::optional<StringKeyedMap<T>>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if(!success)
            return;
        if(!value.IsObject())
            return fallback();
        if(!var)
            var.emplace();
        var->clear();
        for(auto& member : value.GetObject()) {
            auto& inst = var[member.name.GetString()] = NewType(var);
            try {
                if(!DeserializeValue(member.value, inst, fallback))
                    return;
            } catch(JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + member.name.GetString() + "]" + e.what());
            }
        }
    }
    template<class T, with_constructible<StringKeyedMap<T>> D = StringKeyedMap<T>>
    void Deserialize(StringKeyedMap<T>& var, auto const& jsonName, D const& defaultValue, rapidjson::Value const& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if(!success)
            return;
        if(!value.IsObject())
            return fallback();
        var.clear();
        for(auto& member : value.GetObject()) {
            auto& inst = var[member.name.GetString()] = NewType(var);
            try {
                if(!DeserializeValue(member.value, inst, fallback))
                    return;
            } catch(JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + member.name.GetString() + "]" + e.what());
            }
        }
    }

    template<class T>
    void Serialize(StringKeyedMap<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kObjectType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for(auto const& member : var) {
            auto memberName = GetJSONString(member.first, allocator);
            newValue.AddMember(memberName, SerializeValue(member.second, allocator), allocator);
        }
        if constexpr(!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
    template<class T>
    void Serialize(std::optional<StringKeyedMap<T>> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        if(!var.has_value()) return;
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kObjectType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for(auto const& member : var.value()) {
            auto memberName = GetJSONString(member.first, allocator);
            newValue.AddMember(memberName, SerializeValue(member.second, allocator), allocator);
        }
        if constexpr(!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
#pragma endregion
    template<class T>
    inline void ForwardToDeserialize(T& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        Deserialize(var, jsonName, jsonValue);
    }

    template<class T>
    inline void ForwardToSerialize(T const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        Serialize(var, jsonName, jsonObject, allocator);
    }
}

#undef TYPE_EXCEPTION_STRING
#undef THROW_TYPE_EXCEPTION_FALLBACK
#undef THROW_NOT_FOUND_EXCEPTION_FALLBACK
