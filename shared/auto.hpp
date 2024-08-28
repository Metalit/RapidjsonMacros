#pragma once

#include "./serialization.hpp"

#define TYPE_EXCEPTION_STRING(json, cpp) " was an unexpected type (" + JsonTypeName(json) + "), type expected was: " + CppTypeName(cpp)
#define THROW_TYPE_EXCEPTION_FALLBACK(json, cpp) [&jsonRef = json, &cppRef = cpp]() { throw JSONException(TYPE_EXCEPTION_STRING(jsonRef, cppRef)); }
#define THROW_NOT_FOUND_EXCEPTION_FALLBACK [&jsonName]() { throw JSONException(GetNameString(jsonName) + " was not found"); }

namespace rapidjson_macros_auto {
    using namespace rapidjson_macros_serialization;
    using namespace rapidjson_macros_types;

#pragma region simple
    template <class T>
    void Deserialize(T& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        auto&& [value, success] = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        try {
            DeserializeValue(value, var, THROW_TYPE_EXCEPTION_FALLBACK(value, var));
        } catch (JSONException const& e) {
            throw JSONException(GetNameString(jsonName) + e.what());
        }
        RemoveMember(jsonValue, jsonName);
    }
    template <class T>
    void Deserialize(std::optional<T>& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        auto fallback = [&var]() {
            var = std::nullopt;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if (!success)
            return;
        try {
            if (!DeserializeValue(value, var, fallback))
                return;
        } catch (JSONException const& e) {
            return fallback();  // configurable to throw exception?
            // throw JSONException(GetNameString(jsonName) + e.what());
        }
        RemoveMember(jsonValue, jsonName);
    }
    template <class T, with_constructible<T> D = T>
    void Deserialize(T& var, auto const& jsonName, D const& defaultValue, rapidjson::Value& jsonValue) {
        auto fallback = [&var, &defaultValue]() {
            var = defaultValue;
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if (!success)
            return;
        try {
            if (!DeserializeValue(value, var, fallback))
                return;
        } catch (JSONException const& e) {
            return fallback();  // configurable to throw exception?
            // throw JSONException(GetNameString(jsonName) + e.what());
        }
        RemoveMember(jsonValue, jsonName);
    }

    template <class T>
    void Serialize(T const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        auto serialized = SerializeValue(var, allocator);
        if constexpr (!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, serialized, allocator);
        } else
            jsonObject.Swap(serialized);
    }
    template <class T>
    void Serialize(std::optional<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        if (!var.has_value())
            return;
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        auto serialized = SerializeValue(var, allocator);
        if constexpr (!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, serialized, allocator);
        } else
            jsonObject.Swap(serialized);
    }
#pragma endregion

#pragma region vector
    template <class T>
    void Deserialize(std::vector<T>& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        auto&& [value, success] = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        if (!value.IsArray())
            throw JSONException(GetNameString(jsonName) + TYPE_EXCEPTION_STRING(value, var));
        var.clear();
        for (auto it = value.Begin(); it != value.End();) {
            auto helper = EmplaceWrapper<T>(var);
            try {
                DeserializeValue(*it, helper.ref(), THROW_TYPE_EXCEPTION_FALLBACK(*it, helper.ref()));
                it = value.Erase(it);
            } catch (JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]" + e.what());
            }
            helper.finish();
        }
        RemoveMember(jsonValue, jsonName);
    }
    template <class T>
    void Deserialize(std::optional<std::vector<T>>& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        auto fallback = [&var, &jsonValue, &jsonName]() {
            var = std::nullopt;
            RemoveMember(jsonValue, jsonName);
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if (!success)
            return;
        if (!value.IsArray())
            return fallback();
        if (!var)
            var.emplace();
        var->clear();
        for (auto it = value.Begin(); it != value.End();) {
            auto helper = EmplaceWrapper<T>(*var);
            try {
                if (!DeserializeValue(*it, helper.ref(), fallback))
                    return;
                it = value.Erase(it);
            } catch (JSONException const& e) {
                return fallback();  // configurable to throw exception?
                // throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]" + e.what());
            }
            helper.finish();
        }
        RemoveMember(jsonValue, jsonName);
    }
    template <class T, with_constructible<std::vector<T>> D = std::vector<T>>
    void Deserialize(std::vector<T>& var, auto const& jsonName, D const& defaultValue, rapidjson::Value& jsonValue) {
        auto fallback = [&var, &defaultValue, &jsonValue, &jsonName]() {
            var = defaultValue;
            RemoveMember(jsonValue, jsonName);
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if (!success)
            return;
        if (!value.IsArray())
            return fallback();
        var.clear();
        for (auto it = value.Begin(); it != value.End();) {
            auto helper = EmplaceWrapper<T>(var);
            try {
                if (!DeserializeValue(*it, helper.ref(), fallback))
                    return;
                it = value.Erase(it);
            } catch (JSONException const& e) {
                return fallback();  // configurable to throw exception?
                // throw JSONException(GetNameString(jsonName) + "[" + std::to_string(it - value.Begin()) + "]" + e.what());
            }
            helper.finish();
        }
        RemoveMember(jsonValue, jsonName);
    }

    template <class T>
    void Serialize(std::vector<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kArrayType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for (auto const& element : var) {
            newValue.GetArray().PushBack(SerializeValue(element, allocator), allocator);
        }
        if constexpr (!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
    template <class T>
    void Serialize(
        std::optional<std::vector<T>> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator
    ) {
        if (!var.has_value())
            return;
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kArrayType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for (auto const& element : var.value()) {
            newValue.GetArray().PushBack(SerializeValue(element, allocator), allocator);
        }
        if constexpr (!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
#pragma endregion

#pragma region map
    template <class T>
    void Deserialize(StringKeyedMap<T>& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        auto&& [value, success] = GetMember(jsonValue, jsonName, THROW_NOT_FOUND_EXCEPTION_FALLBACK);
        if (!value.IsObject())
            throw JSONException(GetNameString(jsonName) + TYPE_EXCEPTION_STRING(value, var));
        var.clear();
        for (auto it = value.MemberBegin(); it != value.MemberEnd();) {
            auto& inst = var[it->name.GetString()] = T();
            try {
                DeserializeValue(it->value, inst, THROW_TYPE_EXCEPTION_FALLBACK(it->value, inst));
                it = value.RemoveMember(it);
            } catch (JSONException const& e) {
                throw JSONException(GetNameString(jsonName) + "[" + it->name.GetString() + "]" + e.what());
            }
        }
        RemoveMember(jsonValue, jsonName);
    }
    template <class T>
    void Deserialize(std::optional<StringKeyedMap<T>>& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        auto fallback = [&var, &jsonValue, &jsonName]() {
            var = std::nullopt;
            RemoveMember(jsonValue, jsonName);
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if (!success)
            return;
        if (!value.IsObject())
            return fallback();
        if (!var)
            var.emplace();
        var->clear();
        for (auto it = value.MemberBegin(); it != value.MemberEnd();) {
            auto& inst = var[it->name.GetString()] = T();
            try {
                if (!DeserializeValue(it->value, inst, fallback))
                    return;
                it = value.RemoveMember(it);
            } catch (JSONException const& e) {
                return fallback();  // configurable to throw exception?
                // throw JSONException(GetNameString(jsonName) + "[" + it->name.GetString() + "]" + e.what());
            }
        }
        RemoveMember(jsonValue, jsonName);
    }
    template <class T, with_constructible<StringKeyedMap<T>> D = StringKeyedMap<T>>
    void Deserialize(StringKeyedMap<T>& var, auto const& jsonName, D const& defaultValue, rapidjson::Value& jsonValue) {
        auto fallback = [&var, &defaultValue, &jsonValue, &jsonName]() {
            var = defaultValue;
            RemoveMember(jsonValue, jsonName);
        };
        auto&& [value, success] = GetMember(jsonValue, jsonName, fallback);
        if (!success)
            return;
        if (!value.IsObject())
            return fallback();
        var.clear();
        for (auto it = value.MemberBegin(); it != value.MemberEnd();) {
            auto& inst = var[it->name.GetString()] = T();
            try {
                if (!DeserializeValue(it->value, inst, fallback))
                    return;
                it = value.RemoveMember(it);
            } catch (JSONException const& e) {
                return fallback();  // configurable to throw exception?
                // throw JSONException(GetNameString(jsonName) + "[" + it->name.GetString() + "]" + e.what());
            }
        }
        RemoveMember(jsonValue, jsonName);
    }

    template <class T>
    void Serialize(StringKeyedMap<T> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kObjectType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for (auto const& member : var) {
            auto memberName = GetJSONString(member.first, allocator);
            newValue.AddMember(memberName, SerializeValue(member.second, allocator), allocator);
        }
        if constexpr (!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
    template <class T>
    void Serialize(
        std::optional<StringKeyedMap<T>> const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator
    ) {
        if (!var.has_value())
            return;
        constexpr bool addToExisting = std::is_same_v<decltype(jsonName), SelfValueType const&>;
        rapidjson::Value local(rapidjson::kObjectType);
        rapidjson::Value& newValue = addToExisting ? jsonObject : local;
        for (auto const& member : var.value()) {
            auto memberName = GetJSONString(member.first, allocator);
            newValue.AddMember(memberName, SerializeValue(member.second, allocator), allocator);
        }
        if constexpr (!addToExisting) {
            auto name = GetJSONString(GetDefaultName(jsonName), allocator);
            jsonObject.AddMember(name, newValue, allocator);
        }
    }
#pragma endregion
    template <class T>
    inline void ForwardToDeserialize(T& var, auto const& jsonName, rapidjson::Value& jsonValue) {
        Deserialize(var, jsonName, jsonValue);
    }

    template <class T>
    inline void ForwardToSerialize(T const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) {
        Serialize(var, jsonName, jsonObject, allocator);
    }
}

#undef TYPE_EXCEPTION_STRING
#undef THROW_TYPE_EXCEPTION_FALLBACK
#undef THROW_NOT_FOUND_EXCEPTION_FALLBACK
