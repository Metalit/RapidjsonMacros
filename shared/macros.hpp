#pragma once

#include "rapidjson-macros/shared/serialization.hpp"

class JSONException : public std::exception {
    private:
        const char* error;
        std::string message;
    public:
        explicit JSONException(const std::string& message) : message(message) {
            error = message.c_str();
        }
        const char* what() const noexcept override {
            return error;
        }
};

class JSONClass {
    protected:
        std::vector<std::function<void(rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)>> serializers;
        std::vector<std::function<void(const rapidjson::Value& jsonValue)>> deserializers;
    public:
        virtual void Deserialize(const rapidjson::Value& jsonValue) {
            for(auto& method : deserializers)
                method(jsonValue);
        }
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) {
            rapidjson::Value jsonObject(rapidjson::kObjectType);
            for(auto& method : serializers)
                method(jsonObject, allocator);
            return jsonObject;
        }
};

#define DECLARE_JSON_CLASS(namespaze, name, ...) \
namespace namespaze { \
    class name : public JSONClass { \
        public: \
            void Deserialize(const rapidjson::Value& jsonValue); \
            rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator); \
            __VA_ARGS__ \
    }; \
}

#define DESERIALIZE_METHOD(namespaze, name, ...) \
void namespaze::name::Deserialize(const rapidjson::Value& jsonValue) { \
    __VA_ARGS__ \
    for(auto& method : deserializers) \
        method(jsonValue); \
}

#define SERIALIZE_METHOD(namespaze, name, ...) \
rapidjson::Value namespaze::name::Serialize(rapidjson::Document::AllocatorType& allocator) { \
    rapidjson::Value jsonObject(rapidjson::kObjectType); \
    __VA_ARGS__ \
    for(auto& method : serializers) \
        method(jsonObject, allocator); \
    return jsonObject; \
}

#define AUTO_VALUE(type, name) \
private: \
template<class T> \
void serialize_##name(T& name##_val, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        SERIALIZE_CLASS(name##_val, name); \
    } else { \
        SERIALIZE_VALUE(name##_val, name); \
    } \
} \
template<class T> \
void deserialize_##name(T& name##_val, const rapidjson::Value& jsonValue) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        DESERIALIZE_CLASS(name##_val, name); \
    } else { \
        DESERIALIZE_VALUE(name##_val, name); \
    } \
} \
public: \
type name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        serialize_##name(name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        deserialize_##name(name, jsonValue); \
    }); \
    return type(); \
})();

#define AUTO_VALUE_OPTIONAL(type, name) \
private: \
template<class T> \
void serialize_##name(std::optional<T>& name##_val, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        SERIALIZE_CLASS_OPTIONAL(name##_val, name); \
    } else { \
        SERIALIZE_VALUE_OPTIONAL(name##_val, name); \
    } \
} \
template<class T> \
void deserialize_##name(std::optional<T>& name##_val, const rapidjson::Value& jsonValue) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        DESERIALIZE_CLASS_OPTIONAL(name##_val, name); \
    } else { \
        DESERIALIZE_VALUE_OPTIONAL(name##_val, name); \
    } \
} \
public: \
std::optional<type> name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        serialize_##name(name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        deserialize_##name(name, jsonValue); \
    }); \
    return std::nullopt; \
})();

#define AUTO_VALUE_DEFAULT(type, name, def) \
private: \
template<class T> \
void serialize_##name(T& name##_val, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        SERIALIZE_CLASS(name##_val, name); \
    } else { \
        SERIALIZE_VALUE(name##_val, name); \
    } \
} \
template<class T> \
void deserialize_##name(T& name##_val, const rapidjson::Value& jsonValue) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        DESERIALIZE_CLASS_DEFAULT(name##_val, name, def); \
    } else { \
        DESERIALIZE_VALUE_DEFAULT(name##_val, name, def); \
    } \
} \
public: \
type name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        serialize_##name(name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        deserialize_##name(name, jsonValue); \
    }); \
    return def; \
})();

#define AUTO_VECTOR(type, name) \
private: \
template<class T> \
void serialize_##name(std::vector<T>& name##_val, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        SERIALIZE_VECTOR(name##_val, name); \
    } else { \
        SERIALIZE_VECTOR_BASIC(name##_val, name); \
    } \
} \
template<class T> \
void deserialize_##name(std::vector<T>& name##_val, const rapidjson::Value& jsonValue) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        DESERIALIZE_VECTOR(name##_val, name); \
    } else { \
        DESERIALIZE_VECTOR_BASIC(name##_val, name); \
    } \
} \
public: \
std::vector<type> name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        serialize_##name(name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        deserialize_##name(name, jsonValue); \
    }); \
    return std::vector<type>(); \
})();

#define AUTO_VECTOR_OPTIONAL(type, name) \
private: \
template<class T> \
void serialize_##name(std::optional<std::vector<T>>& name##_val, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        SERIALIZE_VECTOR_OPTIONAL(name##_val, name); \
    } else { \
        SERIALIZE_VECTOR_BASIC_OPTIONAL(name##_val, name); \
    } \
} \
template<class T> \
void deserialize_##name(std::optional<std::vector<T>>& name##_val, const rapidjson::Value& jsonValue) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        DESERIALIZE_VECTOR_OPTIONAL(name##_val, name); \
    } else { \
        DESERIALIZE_VECTOR_BASIC_OPTIONAL(name##_val, name); \
    } \
} \
public: \
std::optional<std::vector<type>> name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        serialize_##name(name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        deserialize_##name(name, jsonValue); \
    }); \
    return std::nullopt; \
})();

#define AUTO_VECTOR_DEFAULT(type, name, def) \
private: \
template<class T> \
void serialize_##name(std::vector<T>& name##_val, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        SERIALIZE_VECTOR(name##_val, name); \
    } else { \
        SERIALIZE_VECTOR_BASIC(name##_val, name); \
    } \
} \
template<class T> \
void deserialize_##name(std::vector<T>& name##_val, const rapidjson::Value& jsonValue) { \
    if constexpr(std::is_base_of<JSONClass, T>::value) { \
        DESERIALIZE_VECTOR_DEFAULT(name##_val, name, def); \
    } else { \
        DESERIALIZE_VECTOR_BASIC_DEFAULT(name##_val, name, def); \
    } \
} \
public: \
std::vector<type> name = ([this]() { \
    serializers.emplace_back([this](rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) { \
        serialize_##name(name, jsonObject, allocator); \
    }); \
    deserializers.emplace_back([this](const rapidjson::Value& jsonValue) { \
        deserialize_##name(name, jsonValue); \
    }); \
    return def; \
})();

// functions, will be included with class definitions
static void ReadFromFile(std::string_view path, JSONClass& toDeserialize) {
    if(!fileexists(path))
        throw JSONException("file not found");
    auto json = readfile(path);

    rapidjson::Document document;
    document.Parse(json);
    if(document.HasParseError() || !document.IsObject())
        throw JSONException("file could not be parsed as json");
    
    toDeserialize.Deserialize(document.GetObject());
}

static bool WriteToFile(std::string_view path, JSONClass& toSerialize) {
    rapidjson::Document document;
    document.SetObject();
    toSerialize.Serialize(document.GetAllocator()).Swap(document);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string s = buffer.GetString();

    return writefile(path, s);
}