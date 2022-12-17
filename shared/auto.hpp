#pragma once

#include "../shared/serialization.hpp"

#define TYPE_EXCEPTION_STRING " was an unexpected type (" + JsonTypeName(jsonValue) + "), type expected was: " + CppTypeName(var)
#define THROW_TYPE_EXCEPTION_FALLBACK [&jsonValue, &var]() { throw JSONException(TYPE_EXCEPTION_STRING); }
#define THROW_NOT_FOUND_EXCEPTION_FALLBACK [&jsonName]() { throw JSONException(GetNameString(jsonName) + " was not found"); }

namespace rapidjson_macros_auto {
    using namespace rapidjson_macros_serialization;
    using namespace rapidjson_macros_types;

#pragma region simple
    template<class T>
    void Deserialize(T& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto& value = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        try {
            DeserializeValue(value, var, THROW_TYPE_EXCEPTION_FALLBACK);
        } catch(std::exception const& e) {
            throw JSONException(GetNameString(jsonName) + "." + e.what());
        }
    }
    template<class T>
    void Deserialize(std::optional<T>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto& value = GetMember(jsonValue, jsonName, fallback);
        try {
            DeserializeValue(value, var, fallback);
        } catch(std::exception const& e) {
            throw JSONException(GetNameString(jsonName) + "." + e.what());
        }
    }
    template<class T, with_constructible<T> D = T>
    void Deserialize(T& var, auto const& jsonName, D const& defaultValue, rapidjson::Value const& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto& value = GetMember(jsonValue, jsonName, fallback);
        try {
            DeserializeValue(value, var, fallback);
        } catch(std::exception const& e) {
            throw JSONException(GetNameString(jsonName) + "." + e.what());
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
        auto& value = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        if(!value.IsArray())
            throw JSONException(GetNameString(jsonName) + "." TYPE_EXCEPTION_STRING);
        for(auto it = value.Begin(); it != value.End(); ++it) {
            auto& inst = var.emplace_back(NewType(var));
            try {
                DeserializeValue(*it, inst, THROW_TYPE_EXCEPTION_FALLBACK);
            } catch(std::exception const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]." + e.what());
            }
        }
    }
    template<class T>
    void Deserialize(std::optional<std::vector<T>>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto& value = GetMember(jsonValue, jsonName, fallback);
        if(!value.IsArray())
            return fallback();
        for(auto it = value.Begin(); it != value.End(); ++it) {
            auto& inst = var->emplace_back(NewType(var));
            try {
                DeserializeValue(*it, inst, fallback);
            } catch(std::exception const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]." + e.what());
            }
        }
    }
    template<class T, with_constructible<std::vector<T>> D = T>
    void Deserialize(std::vector<T>& var, auto const& jsonName, D const& defaultValue, rapidjson::Value const& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto& value = GetMember(jsonValue, jsonName, fallback);
        if(!value.IsArray())
            return fallback();
        for(auto it = value.Begin(); it != value.End(); ++it) {
            auto& inst = var.emplace_back(NewType(var));
            try {
                DeserializeValue(*it, inst, fallback);
            } catch(std::exception const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]." + e.what());
            }
        }
    }

    template<class T>
    void Serialize(std::vector<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value newValue(rapidjson::kArrayType);
        for(auto const& element : var) {
            newValue.GetArray().PushBack(SerializeValue(element, allocator), allocator);
        }
        auto name = GetJSONString(GetDefaultName(jsonName), allocator);
        jsonObject.AddMember(name, newValue, allocator);
    }
    template<class T>
    void Serialize(std::optional<std::vector<T>> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        if(!var.has_value()) return;
        rapidjson::Value newValue(rapidjson::kArrayType);
        for(auto const& element : var.value()) {
            newValue.GetArray().PushBack(SerializeValue(element, allocator), allocator);
        }
        auto name = GetJSONString(GetDefaultName(jsonName), allocator);
        jsonObject.AddMember(name, newValue, allocator);
    }
#pragma endregion

#pragma region map
    template<class T>
    void Deserialize(StringKeyedMap<T>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto& value = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        if(!value.IsObject())
            throw JSONException(GetNameString(jsonName) + "." TYPE_EXCEPTION_STRING);
        for(auto& member : value.GetObject()) {
            auto& inst = var[member.name.GetString()] = NewType(var);
            try {
                DeserializeValue(member.value, inst, THROW_TYPE_EXCEPTION_FALLBACK);
            } catch(std::exception const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + member.name.GetString() + "]." + e.what());
            }
        }
    }
    template<class T>
    void Deserialize(std::optional<StringKeyedMap<T>>& var, auto const& jsonName, rapidjson::Value const& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto& value = GetMember(jsonValue, jsonName, fallback);
        if(!value.IsObject())
            return fallback();
        for(auto& member : value.GetObject()) {
            auto& inst = var[member.name.GetString()] = NewType(var);
            try {
                DeserializeValue(member.value, inst, fallback);
            } catch(std::exception const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + member.name.GetString() + "]." + e.what());
            }
        }
    }
    template<class T, with_constructible<StringKeyedMap<T>> D = T>
    void Deserialize(StringKeyedMap<T>& var, auto const& jsonName, D const& defaultValue, rapidjson::Value const& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto& value = GetMember(jsonValue, jsonName, fallback);
        if(!value.IsObject())
            return fallback();
        for(auto& member : value.GetObject()) {
            auto& inst = var[member.name.GetString()] = NewType(var);
            try {
                DeserializeValue(member.value, inst, fallback);
            } catch(std::exception const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + member.name.GetString() + "]." + e.what());
            }
        }
    }

    template<class T>
    void Serialize(StringKeyedMap<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value newValue(rapidjson::kObjectType);
        for(auto const& member : var) {
            auto memberName = GetJSONString(member.first, allocator);
            newValue.AddMember(memberName, SerializeValue(member.second, allocator), allocator);
        }
        auto name = GetJSONString(GetDefaultName(jsonName), allocator);
        jsonObject.AddMember(name, newValue, allocator);
    }
    template<class T>
    void Serialize(std::optional<StringKeyedMap<T>> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        if(!var.has_value()) return;
        rapidjson::Value newValue(rapidjson::kObjectType);
        for(auto const& member : var.value()) {
            auto memberName = GetJSONString(member.first, allocator);
            newValue.AddMember(memberName, SerializeValue(member.second, allocator), allocator);
        }
        auto name = GetJSONString(GetDefaultName(jsonName), allocator);
        jsonObject.AddMember(name, newValue, allocator);
    }
#pragma endregion
}

#undef TYPE_EXCEPTION_STRING
#undef THROW_TYPE_EXCEPTION_FALLBACK
#undef THROW_NOT_FOUND_EXCEPTION_FALLBACK
