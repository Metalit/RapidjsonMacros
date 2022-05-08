#pragma once

#include "rapidjson-macros/shared/types.hpp"

#define DESERIALIZE_VALUE(name, jsonName) \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
if (!rapidjson_macros_types::GetIsType(jsonValue[jsonName], name)) throw JSONException(std::string(jsonName) + " was not the expected value type"); \
name = rapidjson_macros_types::GetValueType(jsonValue[jsonName], name);

#define DESERIALIZE_VALUE_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && rapidjson_macros_types::GetIsTypeOptional(jsonValue[jsonName], name)) { \
    name = rapidjson_macros_types::GetValueTypeOptional(jsonValue[jsonName], name); \
} else name = std::nullopt;

#define DESERIALIZE_VALUE_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && rapidjson_macros_types::GetIsType(jsonValue[jsonName], name)) { \
    name = rapidjson_macros_types::GetValueType(jsonValue[jsonName], name); \
} else name = def;

#define DESERIALIZE_CLASS(name, jsonName) \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
if (!jsonValue[jsonName ].IsObject()) throw JSONException(std::string(jsonName) + ", type expected was: JsonObject"); \
name.Deserialize(jsonValue[jsonName]);

#define DESERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsObject()) { \
    if(!name.has_value()) name.emplace(); \
    name->Deserialize(jsonValue[jsonName]); \
} else name = std::nullopt;

#define DESERIALIZE_CLASS_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsObject()) { \
    name.Deserialize(jsonValue[jsonName]); \
} else name = def;

// seems to assume vector is of another json class
#define DESERIALIZE_VECTOR(name, jsonName) \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
name.clear(); \
auto& jsonArray = jsonValue[jsonName]; \
if(jsonArray.IsArray()) { \
    for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else throw JSONException(std::string(jsonName) + ", type expected was: JsonArray");

#define DESERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& jsonArray = jsonValue[jsonName]; \
    for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorTypeOptional(name); \
        value.Deserialize(*it); \
        name->push_back(value); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    name.clear(); \
    auto& jsonArray = jsonValue[jsonName]; \
    for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else name = def;

#define DESERIALIZE_VECTOR_BASIC(name, jsonName) \
if (!jsonValue.HasMember(jsonName)) throw JSONException(std::string(jsonName) + " not found"); \
name.clear(); \
auto& jsonArray = jsonValue[jsonName]; \
if(jsonArray.IsArray()) { \
    for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
} else throw JSONException(std::string(jsonName) + ", type expected was: JsonArray");

#define DESERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& jsonArray = jsonValue[jsonName]; \
    for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it) { \
        name->push_back(rapidjson_macros_types::GetValueTypeVectorOptional(*it, name)); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_BASIC_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(jsonName) && jsonValue[jsonName].IsArray()) { \
    name.clear(); \
    auto& jsonArray = jsonValue[jsonName]; \
    for (auto it = jsonArray.Begin(); it != jsonArray.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
} else name = def;

#define SERIALIZE_VALUE(name, jsonName) \
jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name, allocator);

#define SERIALIZE_VALUE_OPTIONAL(name, jsonName) \
if(name) jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name.value(), allocator);

#define SERIALIZE_CLASS(name, jsonName) \
jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name.Serialize(allocator), allocator);

#define SERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(name) jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name->Serialize(allocator), allocator);

// assumes vector is of json serializables
#define SERIALIZE_VECTOR(name, jsonName) \
rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
for(auto& jsonClass : name) { \
    name##_jsonArray.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
} \
jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name##_jsonArray, allocator);

#define SERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
    for(auto& jsonClass : name.value()) { \
        name##_jsonArray.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
    } \
    jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name##_jsonArray, allocator);\
}

#define SERIALIZE_VECTOR_BASIC(name, jsonName) \
rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
for(const auto& member : name) { \
    name##_jsonArray.GetArray().PushBack(rapidjson_macros_types::CreateJSONValue(member, allocator), allocator); \
} \
jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name##_jsonArray, allocator);

#define SERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
    for(const auto& member : name.value()) { \
        name##_jsonArray.GetArray().PushBack(rapidjson_macros_types::CreateJSONValue(member, allocator), allocator); \
    } \
    jsonObject.AddMember(rapidjson_macros_types::GetJSONString(jsonName, allocator), name##_jsonArray, allocator); \
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
