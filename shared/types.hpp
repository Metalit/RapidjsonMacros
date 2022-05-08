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

template<class T>
concept JSONClassDerived = std::is_base_of_v<JSONClass, T>;

namespace rapidjson_macros_types {

    template<class T, class R>
    R GetJSONString(T&& string, rapidjson::Document::AllocatorType& allocator allocator) {
        if constexpr(std::is_same<const char*, T>::value) {
            return string;
        }
        return rapidjson::Value(string, allocator).Move();
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
