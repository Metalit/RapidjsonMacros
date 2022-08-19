#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

class JSONException : public std::exception {
    private:
        std::string message;
    public:
        explicit JSONException(const std::string& message) : message(message) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
};

class JSONClass {
    public:
        virtual void Deserialize(rapidjson::Value& jsonValue) = 0;
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) = 0;
        bool operator==(const JSONClass&) const = default; \
};

template<class T>
concept JSONClassDerived = std::is_base_of_v<JSONClass, T>;

namespace rapidjson_macros_types {
    
    template<class T>
    inline std::string CppTypeName(T& var) {
        char* realname = abi::__cxa_demangle(typeid(var).name(), 0, 0, 0);
        std::string s(realname);
        free(realname);
        return s;
    }
    inline std::string JsonTypeName(rapidjson::Value& jsonValue) {
        auto type = jsonValue.GetType();
        switch (type) {
        case rapidjson::kNullType:
            return "null";
        case rapidjson::kFalseType:
            return "false";
        case rapidjson::kTrueType:
            return "true";
        case rapidjson::kObjectType:
            return "object";
        case rapidjson::kArrayType:
            return "array";
        case rapidjson::kStringType:
            return "string";
        case rapidjson::kNumberType:
            return "number";
        default:
            return "unknown";
        }
    }

    template<class T, class R, std::size_t N = 0>
    inline R GetJSONString(const T& string, rapidjson::Document::AllocatorType& allocator);

    template<class T, std::size_t N = 0>
    requires(std::is_convertible_v<T, std::string>)
    inline rapidjson::Value GetJSONString(const T& string, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(string, allocator);
    }
    template<std::size_t N = 0>
    inline rapidjson::Value::StringRefType GetJSONString(const char (&string)[N], rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value::StringRefType(string);
    }

    template<class T>
    inline rapidjson::Value CreateJSONValue(T& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value);
    }
    template<>
    inline rapidjson::Value CreateJSONValue(const std::string& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value, allocator);
    }
    template<>
    inline rapidjson::Value CreateJSONValue(std::string& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value, allocator);
    }
    
    template<class T>
    inline T GetValueType(const rapidjson::Value& jsonValue, const T& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    inline bool GetIsType(const rapidjson::Value& jsonValue, const T& _) {
        return jsonValue.Is<T>();
    }

    template<class T>
    inline T GetValueTypeOptional(const rapidjson::Value& jsonValue, const std::optional<T>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    inline bool GetIsTypeOptional(const rapidjson::Value& jsonValue, const std::optional<T>& _) {
        return jsonValue.Is<T>();
    }

    template<class T>
    inline T GetValueTypeVector(const rapidjson::Value& jsonValue, const std::vector<T>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    inline T GetValueTypeVectorOptional(const rapidjson::Value& jsonValue, const std::optional<std::vector<T>>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    inline T NewVectorType(const std::vector<T>& _) {
        return T();
    }

    template<class T>
    inline T NewVectorTypeOptional(const std::optional<std::vector<T>>& _) {
        return T();
    }
}
