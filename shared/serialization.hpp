#pragma once

#include "../shared/types.hpp"

#include <span>
#include <sstream>
#include <tuple>

namespace rapidjson_macros_auto {
    template<class T>
    inline void ForwardToDeserialize(T& var, auto const& jsonName, rapidjson::Value const& jsonValue);
    template<class T>
    inline void ForwardToSerialize(T const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);
}

namespace rapidjson_macros_serialization {

    template<class T, rapidjson_macros_types::callable F>
    requires std::is_constructible_v<std::string, T>
    inline std::tuple<rapidjson::Value const&, bool> GetMember(rapidjson::Value const& jsonObject, T const& search, F const& onNotFound) {
        if(!jsonObject.IsObject()) {
            std::stringstream exc{};
            exc << " was an unexpected type (";
            exc << rapidjson_macros_types::JsonTypeName(jsonObject);
            exc << ") not an object";
            throw JSONException(exc.str());
        }
        auto iter = jsonObject.FindMember(search);
        if(iter != jsonObject.MemberEnd())
            return {iter->value, true};
        onNotFound();
        return {jsonObject, false};
    }

    template<class T, rapidjson_macros_types::callable F>
    requires std::is_constructible_v<std::string, T>
    inline std::tuple<rapidjson::Value const&, bool> GetMember(rapidjson::Value const& jsonObject, std::vector<T> const& search, F const& onNotFound) {
        if(!jsonObject.IsObject()) {
            std::stringstream exc{};
            exc << " was an unexpected type (";
            exc << rapidjson_macros_types::JsonTypeName(jsonObject);
            exc << ") not an object";
            throw JSONException(exc.str());
        }
        for(auto& name : search) {
            auto iter = jsonObject.FindMember(name);
            if(iter != jsonObject.MemberEnd())
                return {iter->value, true};
        }
        onNotFound();
        return {jsonObject, false};
    }

    template<rapidjson_macros_types::callable F>
    inline std::tuple<rapidjson::Value const&, bool> GetMember(rapidjson::Value const& jsonObject, rapidjson_macros_types::SelfValueType const& search, F const& onNotFound) {
        return {jsonObject, true};
    }

    template<class J, class T>
    requires std::is_constructible_v<std::string, T> && std::is_same_v<rapidjson::Value, std::remove_const_t<J>>
    void RemoveMember(J& jsonObject, T const& search) {
        if constexpr(!std::is_const_v<J>)
            jsonObject.RemoveMember(search);
    }

    template<class J, class T>
    requires std::is_constructible_v<std::string, T> && std::is_same_v<rapidjson::Value, std::remove_const_t<J>>
    void RemoveMember(J& jsonObject, std::vector<T> const& search) {
        if constexpr(!std::is_const_v<J>) {
            for(auto& name : search)
                jsonObject.RemoveMember(name);
        }
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetNameString(T const& search) {
        return search;
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetNameString(std::vector<T> const& search) {
        if(search.size() == 0)
            return "()";
        std::stringstream ret;
        ret << "(" << search.front();
        for(auto& name : std::span(search).subspan(1))
            ret << " or " << name;
        ret << ")";
        return ret.str();
    }

    std::string GetNameString(rapidjson_macros_types::SelfValueType const& search) {
        return "{unnamed}";
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetDefaultName(T const& search) {
        return search;
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetDefaultName(std::vector<T> const& search) {
        if(search.size() == 0)
            return "";
        return search.front();
    }

    template<class T, rapidjson_macros_types::callable F>
    bool DeserializeValue(rapidjson::Value const& value, T& variable, F const& onWrongType) {
        if constexpr(JSONClassDerived<rapidjson_macros_types::maybe_optional_t<T>>) {
            if constexpr(rapidjson_macros_types::is_optional<T>) {
                if(!variable.has_value())
                    variable.emplace();
                variable->Deserialize(value);
            } else
                variable.Deserialize(value);
        } else if constexpr(JSONBasicType<rapidjson_macros_types::maybe_optional_t<T>>) {
            if (!rapidjson_macros_types::GetIsType(value, variable)) {
                onWrongType();
                return false;
            }
            variable = rapidjson_macros_types::GetValueType(value, variable);
        } else
            rapidjson_macros_auto::ForwardToDeserialize(variable, rapidjson_macros_types::SelfValueType(), value);
        return true;
    }

    template<class T>
    rapidjson::Value SerializeValue(T const& variable, rapidjson::Document::AllocatorType& allocator) {
        using real_t = std::decay_t<decltype(variable)>; // fixes issues with const for char arrays
        if constexpr(JSONClassDerived<rapidjson_macros_types::maybe_optional_t<real_t>>) {
            if constexpr(rapidjson_macros_types::is_optional<T>)
                return variable->Serialize(allocator);
            else
                return variable.Serialize(allocator);
        } else if constexpr(JSONBasicType<rapidjson_macros_types::maybe_optional_t<real_t>>) {
            if constexpr(rapidjson_macros_types::is_optional<T>)
                return rapidjson_macros_types::CreateJSONValue(variable.value(), allocator);
            else
                return rapidjson_macros_types::CreateJSONValue(variable, allocator);
        } else {
            rapidjson::Value newValue(rapidjson_macros_types::container_t<rapidjson_macros_types::maybe_optional_t<real_t>>);
            rapidjson_macros_auto::ForwardToSerialize(variable, rapidjson_macros_types::SelfValueType(), newValue, allocator);
            return newValue;
        }
    }

    template<class S>
    void DeserializeInternal(S* self, rapidjson::Value const& jsonValue) {
        if constexpr(S::keepExtraFields) {
            self->extraFields = jsonValue;
            for(auto& method : S::deserializers())
                method(self, self->extraFields->document);
        } else {
            for(auto& method : S::deserializers())
                method(self, jsonValue);
        }
    }
}

template<JSONClassDerived T>
static inline void ReadFromFile(std::string_view path, T& toDeserialize) {
    if(!fileexists(path))
        throw JSONException("file not found");
    return ReadFromString(readfile(path), toDeserialize);
}

template<JSONClassDerived T>
static void ReadFromString(std::string_view string, T& toDeserialize) {
    rapidjson::Document document;
    document.Parse(string.data());
    if(document.HasParseError() || !document.IsObject())
        throw JSONException("string could not be parsed as json");

    toDeserialize.Deserialize(document.GetObject());
}

template<JSONClassDerived T>
static inline bool WriteToFile(std::string_view path, const T& toSerialize) {
    return writefile(path, WriteToString(toSerialize));
}

template<JSONClassDerived T>
static std::string WriteToString(const T& toSerialize) {
    rapidjson::Document document;
    document.SetObject();
    toSerialize.Serialize(document.GetAllocator()).Swap(document);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return buffer.GetString();
}
