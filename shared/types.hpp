#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

namespace rapidjson_macros_types {
    
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
