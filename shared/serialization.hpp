#pragma once

#include "../shared/types.hpp"

#include <span>
#include <sstream>

namespace rapidjson_macros_serialization {

    template<class T>
    requires std::is_constructible_v<std::string, T>
    inline std::optional<std::reference_wrapper<rapidjson::Value>> TryGetMember(rapidjson::Value& jsonObject, T const& search) {
        if(!jsonObject.IsObject()) {
            std::stringstream exc{};
            exc << " was an unexpected type (";
            exc << rapidjson_macros_types::JsonTypeName(jsonObject);
            exc << ") not an object";
            throw JSONException(exc.str());
        }
        auto iter = jsonObject.FindMember(search);
        if(iter != jsonObject.MemberEnd())
            return iter->value;
        return std::nullopt;
    }

    template<class T>
    requires std::is_constructible_v<std::string, T>
    inline std::optional<std::reference_wrapper<rapidjson::Value>> TryGetMember(rapidjson::Value& jsonObject, std::vector<T> const& search) {
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
            return "()";
        std::stringstream ret;
        ret << "(" << search.front();
        for(auto& name : std::span(search).subspan(1))
            ret << " or " << name;
        ret << ")";
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

#define RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName) \
rapidjson_macros_serialization::GetNameString(jsonName) + ". was not found"

#define RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(varName, jsonName, jsonValue, ...) \
rapidjson_macros_serialization::GetNameString(jsonName) __VA_OPT__(+) __VA_ARGS__ + \
". was an unexpected type (" + rapidjson_macros_types::JsonTypeName(jsonValue) + \
"), type expected was: " + rapidjson_macros_types::CppTypeName(varName)

#define RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, prevExc, ...) \
rapidjson_macros_serialization::GetNameString(jsonName) __VA_OPT__(+) __VA_ARGS__ + "." + prevExc.what()

#define DESERIALIZE_VALUE(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName)); \
rapidjson::Value& value = valueOpt.value(); \
if (!rapidjson_macros_types::GetIsType(value, name)) \
throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName, value)); \
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
if (!valueOpt.has_value()) throw JSONException(RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName)); \
rapidjson::Value& value = valueOpt.value(); \
try { name.Deserialize(value); } catch(const std::exception& e) \
    { throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e)); } \
rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); }

#define DESERIALIZE_CLASS_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value()) { \
    if(!name.has_value()) name.emplace(); \
    rapidjson::Value& value = valueOpt.value(); \
    try { name->Deserialize(value); } catch(const std::exception& e) \
        { throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e)); } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = std::nullopt; }

#define DESERIALIZE_CLASS_DEFAULT(name, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value()) { \
    rapidjson::Value& value = valueOpt.value(); \
    try { name.Deserialize(value); } catch(const std::exception& e) \
        { throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e)); } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

// seems to assume vector is of another json class
#define DESERIALIZE_VECTOR(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName)); \
name.clear(); \
rapidjson::Value& value = valueOpt.value(); \
if(value.IsArray()) { \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto inst = rapidjson_macros_types::NewVectorType(name); \
        try { inst.Deserialize(*it); } catch(const std::exception& e) \
            { int idx = it - value.Begin(); std::string ctx("["); ctx += std::to_string(idx) + "]"; \
            throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e, ctx)); } \
        name.push_back(inst); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName, value)); }

#define DESERIALIZE_VECTOR_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto inst = rapidjson_macros_types::NewVectorTypeOptional(name); \
        try { inst.Deserialize(*it); } catch(const std::exception& e) \
            { int idx = it - value.Begin(); std::string ctx("["); ctx += std::to_string(idx) + "]"; \
            throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e, ctx)); } \
        name->push_back(inst); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = std::nullopt; }

#define DESERIALIZE_VECTOR_DEFAULT(name, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    name.clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto inst = rapidjson_macros_types::NewVectorType(name); \
        try { inst.Deserialize(*it); } catch(const std::exception& e) \
            { int idx = it - value.Begin(); std::string ctx("["); ctx += std::to_string(idx) + "]"; \
            throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e, ctx)); } \
        name.push_back(inst); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

#define DESERIALIZE_VECTOR_BASIC(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName)); \
name.clear(); \
rapidjson::Value& value = valueOpt.value(); \
if(value.IsArray()) { \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        if (!rapidjson_macros_types::GetIsTypeVector(*it, name)) \
            { int idx = it - value.Begin(); std::string ctx("["); ctx += std::to_string(idx) + "]"; \
            auto cppTypeVar = rapidjson_macros_types::NewVectorType(name); \
            throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(cppTypeVar, jsonName, *it, ctx)); } \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName, value)); }

#define DESERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        if (!rapidjson_macros_types::GetIsTypeVectorOptional(*it, name)) \
            { int idx = it - value.Begin(); std::string ctx("["); ctx += std::to_string(idx) + "]"; \
            auto cppTypeVar = rapidjson_macros_types::NewVectorTypeOptional(name); \
            throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(cppTypeVar, jsonName, *it, ctx)); } \
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
        if (!rapidjson_macros_types::GetIsTypeVector(*it, name)) \
            { int idx = it - value.Begin(); std::string ctx("["); ctx += std::to_string(idx) + "]"; \
            auto cppTypeVar = rapidjson_macros_types::NewVectorType(name); \
            throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(cppTypeVar, jsonName, *it, ctx)); } \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else name = def; }

#define DESERIALIZE_MAP(varName, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName)); \
varName.clear(); \
rapidjson::Value& value = valueOpt.value(); \
if(value.IsObject()) { \
    for (auto& member : value.GetObject()) { \
        auto inst = rapidjson_macros_types::NewMapType(varName); \
        try { inst.Deserialize(member.value); } catch(const std::exception& e) \
            { std::string ctx("["); ctx += std::string(member.name.GetString()) + "]"; \
            throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e, ctx)); } \
        varName.insert({member.name.GetString(), inst}); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(varName, jsonName, value)); }

#define DESERIALIZE_MAP_OPTIONAL(varName, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsObject()) { \
    if(!varName.has_value()) varName.emplace(); \
    else varName->clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto& member : value.GetObject()) { \
        auto inst = rapidjson_macros_types::NewMapTypeOptional(varName); \
        try { inst.Deserialize(member.value); } catch(const std::exception& e) \
            { std::string ctx("["); ctx += std::string(member.name.GetString()) + "]"; \
            throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e, ctx)); } \
        varName->insert({member.name.GetString(), inst}); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else varName = std::nullopt; }

#define DESERIALIZE_MAP_DEFAULT(varName, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsObject()) { \
    varName.clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto& member : value.GetObject()) { \
        auto inst = rapidjson_macros_types::NewMapType(varName); \
        try { inst.Deserialize(member.value); } catch(const std::exception& e) \
            { std::string ctx("["); ctx += std::string(member.name.GetString()) + "]"; \
            throw JSONException(RAPIDJSON_MACROS_EXCEPTION_CONTEXT(jsonName, e, ctx)); } \
        varName.insert({member.name.GetString(), inst}); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else varName = def; }

#define DESERIALIZE_MAP_BASIC(varName, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if (!valueOpt.has_value()) throw JSONException(RAPIDJSON_MACROS_NOT_FOUND_EXCPETION_STRING(jsonName)); \
varName.clear(); \
rapidjson::Value& value = valueOpt.value(); \
if(value.IsObject()) { \
    for (auto& member : value.GetObject()) { \
        if (!rapidjson_macros_types::GetIsTypeMap(member.value, varName)) \
            { std::string ctx("["); ctx += std::string(member.name.GetString()) + "]"; \
            auto cppTypeVar = rapidjson_macros_types::NewMapType(varName); \
            throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(cppTypeVar, jsonName, member.value, ctx)); } \
        varName.insert({member.name.GetString(), rapidjson_macros_types::GetValueTypeMap(member.value, varName)}); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(varName, jsonName, value)); }

#define DESERIALIZE_MAP_BASIC_OPTIONAL(varName, jsonName) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsObject()) { \
    if(!varName.has_value()) varName.emplace(); \
    else varName->clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto& member : value.GetObject()) { \
        if (!rapidjson_macros_types::GetIsTypeMapOptional(member.value, varName)) \
            { std::string ctx("["); ctx += std::string(member.name.GetString()) + "]"; \
            auto cppTypeVar = rapidjson_macros_types::NewMapTypeOptional(varName); \
            throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(cppTypeVar, jsonName, member.value, ctx)); } \
        varName->insert({member.name.GetString(), rapidjson_macros_types::GetValueTypeMapOptional(member.value, varName)}); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else varName = std::nullopt; }

#define DESERIALIZE_MAP_BASIC_DEFAULT(varName, jsonName, def) { \
auto valueOpt = rapidjson_macros_serialization::TryGetMember(jsonValue, jsonName); \
if(valueOpt.has_value() && valueOpt.value().get().IsObject()) { \
    varName.clear(); \
    rapidjson::Value& value = valueOpt.value(); \
    for (auto& member : value.GetObject()) { \
        if (!rapidjson_macros_types::GetIsTypeMap(member.value, varName)) \
            { std::string ctx("["); ctx += std::string(member.name.GetString()) + "]"; \
            auto cppTypeVar = rapidjson_macros_types::NewMapType(varName); \
            throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(cppTypeVar, jsonName, member.value, ctx)); } \
        varName.insert({member.name.GetString(), rapidjson_macros_types::GetValueTypeMap(member.value, varName)}); \
    } \
    rapidjson_macros_serialization::TryRemoveMember(jsonValue, jsonName); \
} else varName = def; }

#define SERIALIZE_VALUE(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, rapidjson_macros_types::CreateJSONValue(name, allocator), allocator);

#define SERIALIZE_VALUE_OPTIONAL(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
if(name) jsonObject.AddMember(name##_jsonName, rapidjson_macros_types::CreateJSONValue(name.value(), allocator), allocator);

#define SERIALIZE_CLASS(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name.Serialize(allocator), allocator);

#define SERIALIZE_CLASS_OPTIONAL(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
if(name) jsonObject.AddMember(name##_jsonName, name->Serialize(allocator), allocator);

// assumes vector is of json serializables
#define SERIALIZE_VECTOR(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kArrayType); \
for(const auto& jsonClass : name) { \
    name##_value.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kArrayType); \
    for(const auto& jsonClass : name.value()) { \
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

#define SERIALIZE_MAP(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kObjectType); \
for(const auto& member : name) { \
    auto name##_value_jsonName = rapidjson_macros_types::GetJSONString(member.first, allocator); \
    name##_value.AddMember(name##_value_jsonName, member.second.Serialize(allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_MAP_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kObjectType); \
    for(const auto& member : name.value()) { \
        auto name##_value_jsonName = rapidjson_macros_types::GetJSONString(member.first, allocator); \
        name##_value.AddMember(name##_value_jsonName, member.second.Serialize(allocator), allocator); \
    } \
    auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
    jsonObject.AddMember(name##_jsonName, name##_value, allocator);\
}

#define SERIALIZE_MAP_BASIC(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kObjectType); \
for(const auto& member : name) { \
    auto name##_value_jsonName = rapidjson_macros_types::GetJSONString(member.first, allocator); \
    name##_value.AddMember(name##_value_jsonName, rapidjson_macros_types::CreateJSONValue(member.second, allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_MAP_BASIC_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kObjectType); \
    for(const auto& member : name.value()) { \
        auto name##_value_jsonName = rapidjson_macros_types::GetJSONString(member.first, allocator); \
        name##_value.AddMember(name##_value_jsonName, rapidjson_macros_types::CreateJSONValue(member.second, allocator), allocator); \
    } \
    auto name##_jsonName = rapidjson_macros_types::GetJSONString(rapidjson_macros_serialization::GetDefaultName(jsonName), allocator); \
    jsonObject.AddMember(name##_jsonName, name##_value, allocator); \
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
