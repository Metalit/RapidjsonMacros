#pragma once

#include "rapidjson-macros/shared/types.hpp"

#define DESERIALIZE_VALUE(name, jsonName) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
if (!rapidjson_macros_types::GetIsType(jsonValue[#jsonName], name)) throw JSONException(#jsonName " was not the expected value type"); \
name = rapidjson_macros_types::GetValueType(jsonValue[#jsonName], name);

#define DESERIALIZE_VALUE_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(#jsonName) && rapidjson_macros_types::GetIsTypeOptional(jsonValue[#jsonName], name)) { \
    name = rapidjson_macros_types::GetValueTypeOptional(jsonValue[#jsonName], name); \
} else name = std::nullopt;

#define DESERIALIZE_VALUE_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(#jsonName) && rapidjson_macros_types::GetIsType(jsonValue[#jsonName], name)) { \
    name = rapidjson_macros_types::GetValueType(jsonValue[#jsonName], name); \
} else name = def;

#define DESERIALIZE_CLASS(name, jsonName) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
if (!jsonValue[#jsonName].IsObject()) throw JSONException(#jsonName ", type expected was: JsonObject"); \
name.Deserialize(jsonValue[#jsonName]);

#define DESERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsObject()) { \
    if(!name.has_value()) name.emplace(); \
    name->Deserialize(jsonValue[#jsonName]); \
} else name = std::nullopt;

#define DESERIALIZE_CLASS_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsObject()) { \
    name.Deserialize(jsonValue[#jsonName]); \
} else name = def;

// seems to assume vector is of another json class
#define DESERIALIZE_VECTOR(name, jsonName) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
name.clear(); \
auto& jsonName = jsonValue[#jsonName]; \
if(jsonName.IsArray()) { \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else throw JSONException(#jsonName ", type expected was: JsonArray");

#define DESERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorTypeOptional(name); \
        value.Deserialize(*it); \
        name->push_back(value); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    name.clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        auto value = rapidjson_macros_types::NewVectorType(name); \
        value.Deserialize(*it); \
        name.push_back(value); \
    } \
} else name = def;

#define DESERIALIZE_VECTOR_BASIC(name, jsonName) \
if (!jsonValue.HasMember(#jsonName)) throw JSONException(#jsonName " not found"); \
name.clear(); \
auto& jsonName = jsonValue[#jsonName]; \
if(jsonName.IsArray()) { \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
} else throw JSONException(#jsonName ", type expected was: JsonArray");

#define DESERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    if(!name.has_value()) name.emplace(); \
    else name->clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        name->push_back(rapidjson_macros_types::GetValueTypeVectorOptional(*it, name)); \
    } \
} else name = std::nullopt;

#define DESERIALIZE_VECTOR_BASIC_DEFAULT(name, jsonName, def) \
if(jsonValue.HasMember(#jsonName) && jsonValue[#jsonName].IsArray()) { \
    name.clear(); \
    auto& jsonName = jsonValue[#jsonName]; \
    for (auto it = jsonName.Begin(); it != jsonName.End(); ++it) { \
        name.push_back(rapidjson_macros_types::GetValueTypeVector(*it, name)); \
    } \
} else name = def;

#define SERIALIZE_VALUE(name, jsonName) \
jsonObject.AddMember(#jsonName, name, allocator);

#define SERIALIZE_VALUE_OPTIONAL(name, jsonName) \
if(name) jsonObject.AddMember(#jsonName, name.value(), allocator);

#define SERIALIZE_CLASS(name, jsonName) \
jsonObject.AddMember(#jsonName, name.Serialize(allocator), allocator);

#define SERIALIZE_CLASS_OPTIONAL(name, jsonName) \
if(name) jsonObject.AddMember(#jsonName, name->Serialize(allocator), allocator);

// assumes vector is of json serializables
#define SERIALIZE_VECTOR(name, jsonName) \
rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
for(auto jsonClass : name) { \
    name##_jsonArray.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
} \
jsonObject.AddMember(#jsonName, name##_jsonArray, allocator);

#define SERIALIZE_VECTOR_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
    for(auto jsonClass : name.value()) { \
        name##_jsonArray.GetArray().PushBack(jsonClass.Serialize(allocator), allocator); \
    } \
    jsonObject.AddMember(#jsonName, name##_jsonArray, allocator);\
}

#define SERIALIZE_VECTOR_BASIC(name, jsonName) \
rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
for(auto member : name) { \
    name##_jsonArray.GetArray().PushBack(rapidjson::Value(member, allocator).Move(), allocator); \
} \
jsonObject.AddMember(#jsonName, name##_jsonArray, allocator);

#define SERIALIZE_VECTOR_BASIC_OPTIONAL(name, jsonName) \
if(name) { \
    rapidjson::Value name##_jsonArray(rapidjson::kArrayType); \
    for(auto member : name.value()) { \
        name##_jsonArray.GetArray().PushBack(rapidjson::Value(member, allocator).Move(), allocator); \
    } \
    jsonObject.AddMember(#jsonName, name##_jsonArray, allocator); \
}