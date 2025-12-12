#pragma once

#include <unistd.h>

#include <fstream>
#include <span>
#include <sstream>
#include <tuple>

#include "./types.hpp"
#include "rapidjson/include/rapidjson/prettywriter.h"
#include "rapidjson/include/rapidjson/writer.h"

namespace rapidjson_macros_auto {
    template <class T>
    inline void ForwardToDeserialize(T& var, auto const& jsonName, rapidjson::Value& jsonValue);
    template <class T>
    inline void ForwardToSerialize(T const& var, auto const& jsonName, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator);
}

namespace rapidjson_macros_serialization {

    template <class T, rapidjson_macros_types::callable F>
    requires std::is_constructible_v<std::string, T>
    inline std::tuple<rapidjson::Value&, bool> GetMember(rapidjson::Value& jsonObject, T const& search, F const& onNotFound) {
        if (!jsonObject.IsObject()) {
            std::stringstream exc{};
            exc << " was an unexpected type (";
            exc << rapidjson_macros_types::JsonTypeName(jsonObject);
            exc << ") not an object";
            throw JSONException(exc.str());
        }
        auto iter = jsonObject.FindMember(search);
        if (iter != jsonObject.MemberEnd()) {
            return {iter->value, true};
        }
        onNotFound();
        return {jsonObject, false};
    }

    template <class T, rapidjson_macros_types::callable F>
    requires std::is_constructible_v<std::string, T>
    inline std::tuple<rapidjson::Value&, bool> GetMember(rapidjson::Value& jsonObject, std::vector<T> const& search, F const& onNotFound) {
        if (!jsonObject.IsObject()) {
            std::stringstream exc{};
            exc << " was an unexpected type (";
            exc << rapidjson_macros_types::JsonTypeName(jsonObject);
            exc << ") not an object";
            throw JSONException(exc.str());
        }
        for (auto& name : search) {
            auto iter = jsonObject.FindMember(name);
            if (iter != jsonObject.MemberEnd()) {
                return {iter->value, true};
            }
        }
        onNotFound();
        return {jsonObject, false};
    }

    template <rapidjson_macros_types::callable F>
    inline std::tuple<rapidjson::Value&, bool>
    GetMember(rapidjson::Value& jsonObject, rapidjson_macros_types::SelfValueType const& search, F const& onNotFound) {
        return {jsonObject, true};
    }

    template <class J, class T>
    requires std::is_constructible_v<std::string, T> && std::is_same_v<rapidjson::Value, std::remove_const_t<J>>
    void RemoveMember(J& jsonObject, T const& search) {
        if constexpr (!std::is_const_v<J>)
            jsonObject.RemoveMember(search);
    }

    template <class J, class T>
    requires std::is_same_v<rapidjson::Value, std::remove_const_t<J>>
    void RemoveMember(J& jsonObject, std::vector<T> const& search) {
        if constexpr (!std::is_const_v<J>) {
            for (auto& name : search)
                RemoveMember(jsonObject, name);
        }
    }

    template <class J>
    void RemoveMember(J& jsonObject, rapidjson_macros_types::SelfValueType const& search) {}

    template <class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetNameString(T const& search) {
        return std::string(".") + search;
    }

    template <class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetNameString(std::vector<T> const& search) {
        if (search.size() == 0)
            return ".()";
        std::stringstream ret;
        ret << ".(" << search.front();
        for (auto& name : std::span(search).subspan(1))
            ret << " or " << name;
        ret << ")";
        return ret.str();
    }

    inline std::string GetNameString(rapidjson_macros_types::SelfValueType const& search) {
        return "";
    }

    template <class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetDefaultName(T const& search) {
        return search;
    }

    template <class T>
    requires std::is_constructible_v<std::string, T>
    std::string GetDefaultName(std::vector<T> const& search) {
        if (search.size() == 0)
            return "";
        return search.front();
    }

    inline std::string GetDefaultName(rapidjson_macros_types::SelfValueType const& search) {
        return "";
    }

    template <class T, rapidjson_macros_types::callable F>
    bool DeserializeValue(rapidjson::Value& value, T& variable, F const& onWrongType) {
        if constexpr (JSONStruct<rapidjson_macros_types::remove_optional_t<T>>) {
            if constexpr (rapidjson_macros_types::is_optional<T>) {
                if (!variable.has_value())
                    variable.emplace();
                rapidjson_macros_types::remove_optional_t<T>::Deserialize(&*variable, value);
            } else
                rapidjson_macros_types::remove_optional_t<T>::Deserialize(&variable, value);
        } else if constexpr (JSONBasicType<rapidjson_macros_types::remove_optional_t<T>>) {
            if (!rapidjson_macros_types::GetIsType(value, variable)) {
                onWrongType();
                return false;
            }
            variable = rapidjson_macros_types::GetValueType(value, variable);
        } else
            rapidjson_macros_auto::ForwardToDeserialize(variable, rapidjson_macros_types::SelfValueType(), value);
        return true;
    }

    template <class T>
    rapidjson::Value SerializeValue(T const& variable, rapidjson::Document::AllocatorType& allocator) {
        using real_t = std::decay_t<decltype(variable)>;  // fixes issues with const for char arrays
        if constexpr (JSONStruct<rapidjson_macros_types::remove_optional_t<real_t>>) {
            if constexpr (rapidjson_macros_types::is_optional<T>)
                return rapidjson_macros_types::remove_optional_t<real_t>::Serialize(&*variable, allocator);
            else
                return real_t::Serialize(&variable, allocator);
        } else if constexpr (JSONBasicType<rapidjson_macros_types::remove_optional_t<real_t>>) {
            if constexpr (rapidjson_macros_types::is_optional<T>)
                return rapidjson_macros_types::CreateJSONValue(variable.value(), allocator);
            else
                return rapidjson_macros_types::CreateJSONValue(variable, allocator);
        } else {
            rapidjson::Value newValue(rapidjson_macros_types::container_t<rapidjson_macros_types::remove_optional_t<real_t>>);
            rapidjson_macros_auto::ForwardToSerialize(variable, rapidjson_macros_types::SelfValueType(), newValue, allocator);
            return newValue;
        }
    }
}

template <JSONStruct T>
inline void ReadFromString(std::string_view string, T& toDeserialize) {
    rapidjson::Document document;
    document.Parse(string.data());
    if (document.HasParseError())
        throw JSONException("string could not be parsed as json");

    T::Deserialize(&toDeserialize, document);
}

template <JSONStruct T>
inline T ReadFromString(std::string_view string) {
    T ret;
    ReadFromString(string, ret);
    return ret;
}

template <JSONStruct T>
inline void ReadFromFile(std::string_view path, T& toDeserialize) {
    if (access(path.data(), W_OK | R_OK) == -1)
        throw JSONException("file not found");
    std::ifstream file(path.data());
    if (!file.is_open())
        throw JSONException("failed to open file");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return ReadFromString(buffer.str(), toDeserialize);
}

template <JSONStruct T>
inline T ReadFromFile(std::string_view path) {
    T ret;
    ReadFromFile(path, ret);
    return ret;
}

template <JSONStruct T>
inline std::string WriteToString(T const& toSerialize, bool pretty = false) {
    rapidjson::Document document;
    T::Serialize(&toSerialize, document.GetAllocator()).Swap(document);

    rapidjson::StringBuffer buffer;
    if (pretty) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
    } else {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
    }
    return buffer.GetString();
}

template <JSONStruct T>
inline bool WriteToFile(std::string_view path, T const& toSerialize, bool pretty = false) {
    std::ofstream file(path.data());
    if (!file.is_open())
        return false;
    file << WriteToString(toSerialize, pretty);
    return true;
}
