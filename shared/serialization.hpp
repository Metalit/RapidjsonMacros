#pragma once

#include "../shared/types.hpp"

#define RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(varName, jsonName) \
std::string(jsonName) + " was an unexpected type (" + rapidjson_macros_types::JsonTypeName(value) + "), type expected was: " + rapidjson_macros_types::CppTypeName(varName)

#define DESERIALIZE_VALUE(name, jsonName) { \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
auto& value = jsonValue[jsonName]; \
if (!rapidjson_macros_types::GetIsType(value, name)) throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); \
name = rapidjson_macros_types::GetValueType(value, name); \

#define DESERIALIZE_VALUE_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && rapidjson_macros_types::GetIsTypeOptional(jsonValue[jsonName], name)) { \
    auto& value = jsonValue[jsonName]; \
    name = rapidjson_macros_types::GetValueTypeOptional(value, name); \
} else name = std::nullopt;

#define DESERIALIZE_VALUE_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && rapidjson_macros_types::GetIsType(jsonValue[jsonName], name)) { \
    auto& value = jsonValue[jsonName]; \
    name = rapidjson_macros_types::GetValueType(value, name); \
} else name = def;

#define DESERIALIZE_CLASS(name, jsonName) { \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
auto& value = jsonValue[jsonName]; \
if (!value.IsObject()) throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); \
name.Deserialize(value); \

#define DESERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsObject()) { \
    if(!name.has_value()) name.emplace(); \
    auto& value = jsonValue[jsonName]; \
    name->Deserialize(value); \
} else name = std::nullopt;

#define DESERIALIZE_CLASS_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsObject()) { \
    auto& value = jsonValue[jsonName]; \
    name->Deserialize(value); \
} else name = def;

// seems to assume vector is of another json class
#define DESERIALIZE_VECTOR(name, jsonName) { \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
name.clear(); \
auto& value = jsonValue[jsonName]; \
if(value.IsArray()) { \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
    jsonValue.RemoveMember(value); \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); }

#define DESERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& value = jsonValue[jsonName]; \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorTypeOptional(name); \
        value.Deserialize(*it); \
        name->push_back(value); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    name.clear(); \
    auto& value = jsonValue[jsonName]; \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else name = def;

#define DESERIALIZE_VECTOR_BASIC(name, jsonName) { \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
name.clear(); \
auto& value = jsonValue[jsonName]; \
if(value.IsArray()) { \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
} else throw JSONException(RAPIDJSON_MACROS_TYPE_EXCEPTION_STRING(name, jsonName)); }

#define DESERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& value = jsonValue[jsonName]; \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        name->push_back(rapidjson_macros_types::GetValueTypeVectorOptional(*it, name)); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_BASIC_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    name.clear(); \
    auto& value = jsonValue[jsonName]; \
    for (auto it = value.Begin(); it != value.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
} else name = def;

#define SERIALIZE_VALUE(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
jsonObject.AddMember(name##_jsonName, name, allocator);

#define SERIALIZE_VALUE_OPTIONAL(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
if(name) jsonObject.AddMember(name##_jsonName, name.value(), allocator);

#define SERIALIZE_CLASS(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
jsonObject.AddMember(name##_jsonName, name.Serialize(allocator), allocator);

#define SERIALIZE_CLASS_OPTIONAL(name, jsonName) \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
if(name) jsonObject.AddMember(name##_jsonName, name->Serialize(allocator), allocator);

// assumes vector is of json serializables
#define SERIALIZE_VECTOR(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kArrayType); \
for(auto& jsonClass : name) { \
    name##_value.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kArrayType); \
    for(auto& jsonClass : name.value()) { \
        name##_value.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
    } \
    auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
    jsonObject.AddMember(name##_jsonName, name##_value, allocator);\
}

#define SERIALIZE_VECTOR_BASIC(name, jsonName) \
rapidjson::Value name##_value(rapidjson::kArrayType); \
for(const auto& member : name) { \
    name##_value.GetArray().PushBack(rapidjson_macros_types::CreateJSONValue(member, allocator), allocator); \
} \
auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
jsonObject.AddMember(name##_jsonName, name##_value, allocator);

#define SERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_value(rapidjson::kArrayType); \
    for(const auto& member : name.value()) { \
        name##_value.GetArray().PushBack(rapidjson_macros_types::CreateJSONValue(member, allocator), allocator); \
    } \
    auto name##_jsonName = rapidjson_macros_types::GetJSONString(jsonName, allocator); \
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
