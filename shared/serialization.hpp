#pragma once

#include "../shared/types.hpp"

#include <span>
#include <sstream>

namespace rapidjson_macros_serialization {

    template<class T>
    requires std::is_constructible_v<std::string, T>
    inline std::optional<std::reference_wrapper<rapidjson::Value>> TryGetMember(rapidjson::Value& jsonObject, T const& search) {
        auto iter = jsonObject.FindMember(search);
        if(iter != jsonObject.MemberEnd())
            return iter->value;
        return std::nullopt;
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    inline std::optional<std::reference_wrapper<rapidjson::Value>> TryGetMember(rapidjson::Value& jsonObject, std::vector<T> const& search) {
        for(auto& name : search) {
            auto iter = jsonObject.FindMember(name);
            if(iter != jsonObject.MemberEnd())
                return iter->value;
        }
        return std::nullopt;
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    void TryRemoveMember(rapidjson::Value& jsonObject, T const& search) {
        jsonObject.RemoveMember(search);
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    void TryRemoveMember(rapidjson::Value& jsonObject, std::vector<T> const& search) {
        for(auto& name : search)
            jsonObject.RemoveMember(name);
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
            return "";
        std::stringstream ret;
        ret << search.front();
        for(auto& name : std::span(search).subspan(1))
            ret << " or " << name;
        return ret.str();
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
}

#define RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(varName, jsonName) \
rapidjson_macros_serialization::GetNameString(jsonName) + " was an unexpected type (" \
+ rapidjson_macros_types::JsonTypeName(value) + "), type expected was: " \
+ rapidjson_macros_types::CppTypeName(varName)

#define DESERIALIZE_VALUE(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(rapidjson_macros_serialization::GetNameString(jsonName) + " not found"); \
rapidjson::Value& value = valueOpt.value(); \
if (!rapidjson_macros_types::GetIsType(value, name)) throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); \
name = rapidjson_macros_types::GetValueType(value, name); \
rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); }

#define DESERIALIZE_VALUE_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && rapidjson_macros_types::GetIsTypeOptional(valueOpt.value(), name)) { \
    rapidjson::Value& value = valueOpt.value(); \
    name = rapidjson_macros_types::GetValueTypeOptional(value, name); \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = std::nullopt; }

#define DESERIALIZE_VALUE_DEFAULT(name, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && rapidjson_macros_types::GetIsType(valueOpt.value(), name)) { \
    rapidjson::Value& value = valueOpt.value(); \
    name = rapidjson_macros_types::GetValueType(value, name); \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

#define DESERIALIZE_CLASS(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(rapidjson_macros_serialization::GetNameString(jsonName) + " not found"); \
rapidjson::Value& value = valueOpt.value(); \
if (!value.IsObject()) throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); \
name.Deserialize(value); \
rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); }

#define DESERIALIZE_CLASS_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsObject()) { \
    if(!name.has_value()) name.emplace(); \
    rapidjson::Value& value = valueOpt.value(); \
    name->Deserialize(value); \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = std::nullopt; }

#define DESERIALIZE_CLASS_DEFAULT(name, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsObject()) { \
    rapidjson::Value& value = valueOpt.value(); \
    name.Deserialize(value); \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

// seems to assume vector is of another json class
#define DESERIALIZE_VECTOR(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(rapidjson_macros_serialization::GetNameString(jsonName) + " not found"); \
name.clear(); \
rapidjson::Value& value = valueOpt.value(); \
if(value.IsArray()) { \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); }

#define DESERIALIZE_VECTOR_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorTypeOptional(name); \
        value.Deserialize(*it); \
        name->push_back(value); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = std::nullopt; }

#define DESERIALIZE_VECTOR_DEFAULT(name, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    name.clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

#define DESERIALIZE_VECTOR_BASIC(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(rapidjson_macros_serialization::GetNameString(jsonName) + " not found"); \
name.clear(); \
rapidjson::Value& value = valueOpt.value(); \
if(value.IsArray()) { \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); }

#define DESERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        name->push_back(rapidjson_macros_types::GetValueTypeVectorOptional(*it, name)); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = std::nullopt; }

#define DESERIALIZE_VECTOR_BASIC_DEFAULT(name, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    name.clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

#define SERIALIZE_VALUE(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name, allocator);

#define SERIALIZE_VALUE_OPTIONAL(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
if(name) jsonObject.AddMember(name##_jsonName, name.value(), allocator);

#define SERIALIZE_CLASS(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name.Serialize(allocator), allocator);

#define SERIALIZE_CLASS_OPTIONAL(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
if(name) jsonObject.AddMember(name##_jsonName, name->Serialize(allocator), allocator);

// assumes vector is of json serializables
#define SERIALIZE_VECTOR(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kArrayType); \
for(auto& jsonClass : name) { \
    name##_value.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kArrayType); \
    for(auto& jsonClass : name.value()) { \
        name##_value.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
    } \
    auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
    jsonObject.AddMember(name##_jsonName, name##_value, allocator);\
}

#define SERIALIZE_VECTOR_BASIC(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kArrayType); \
for(const auto& member : name) { \
    name##_value.GetArray().PushBack(rapidjson_macros_types::CreateJSONValue(member, allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kArrayType); \
    for(const auto& member : name.value()) { \
        name##_value.GetArray().PushBack(rapidjson_macros_types::CreateJSONValue(member, allocator), allocator); \
    } \
    auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
    jsonObject.AddMember(name##_jsonName, name##_value, allocator); \
}

template<JSONClassDerived T>
static void ReadFromFile(std::string_view path, T& toDeserialize) {
    if(!fileexists(path))
        throw JSONException("file not found");
    auto json = readfile(path);

    rapidjson::Document document;
    document.Parse(json);
    if(document.HasParseError() || !document.IsObject())
        throw JSONException("file could not be parsed as json");
    
    toDeserialize.Deserialize(document.GetObject());
}

template<JSONClassDerived T>
static bool WriteToFile(std::string_view path, T& toSerialize) {
    rapidjson::Document document;
    document.SetObject();
    toSerialize.Serialize(document.GetAllocator()).Swap(document);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string s = buffer.GetString();

    return writefile(path, s);
}
