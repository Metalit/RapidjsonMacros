#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

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
        virtual void _Deserialize(const rapidjson::Value& jsonValue) = 0;
        virtual void _Serialize(rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator) = 0;
    public:
        virtual void Deserialize(const rapidjson::Value& jsonValue) {
            _Deserialize(jsonValue);
        }
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) {
            rapidjson::Value jsonObject(rapidjson::kObjectType);
            _Serialize(jsonObject, allocator);
            return jsonObject;
        }
};

template<class T>
concept JSONClassDerived = std::is_base_of_v<JSONClass, T>;

namespace rapidjson_macros_types {

    template<class T, class R>
    R GetJSONString(const T& string, rapidjson::Document::AllocatorType& allocator);

    template<class T>
    rapidjson::Value& GetJSONString(const T& string, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(string, allocator).Move();
    }
    template<>
    const char* const& GetJSONString(const char* const& string, rapidjson::Document::AllocatorType& allocator) {
        return string;
    }

    template<class T>
    rapidjson::Value& CreateJSONValue(T& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value).Move();
    }
    template<>
    rapidjson::Value& CreateJSONValue(const std::string& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value, allocator).Move();
    }
    template<>
    rapidjson::Value& CreateJSONValue(std::string& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value, allocator).Move();
    }
    
    template<class T>
    T GetValueType(const rapidjson::Value& jsonValue, const T& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    bool GetIsType(const rapidjson::Value& jsonValue, const T& _) {
        return jsonValue.Is<T>();
    }

    template<class T>
    T GetValueTypeOptional(const rapidjson::Value& jsonValue, const std::optional<T>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    bool GetIsTypeOptional(const rapidjson::Value& jsonValue, const std::optional<T>& _) {
        return jsonValue.Is<T>();
    }

    template<class T>
    T GetValueTypeVector(const rapidjson::Value& jsonValue, const std::vector<T>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    T GetValueTypeVectorOptional(const rapidjson::Value& jsonValue, const std::optional<std::vector<T>>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    T NewVectorType(const std::vector<T>& _) {
        return T();
    }

    template<class T>
    T NewVectorTypeOptional(const std::optional<std::vector<T>>& _) {
        return T();
    }
}
