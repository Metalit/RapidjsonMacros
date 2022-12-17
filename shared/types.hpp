#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include <map>
#include <concepts>

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
        static inline constexpr bool keepExtraFields = true;
        virtual void Deserialize(rapidjson::Value const& jsonValue) = 0;
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) const = 0;
        bool operator==(const JSONClass&) const = default; \
};

template<class T>
concept JSONClassDerived = std::is_base_of_v<JSONClass, T>;

template<class T>
concept JSONBasicType = requires(T t) { rapidjson::internal::TypeHelper<rapidjson::Value, std::decay_t<T>>::Is; };

template<class T>
using StringKeyedMap = std::map<std::string, T>;

namespace rapidjson_macros_types {

    template<class From, class T>
    concept with_constructible = std::is_constructible_v<T, From>;

    template<typename T>
    concept callable = requires(T t) { t(); };

    template<typename T>
    concept is_optional = std::same_as<T, std::optional<typename T::value_type>>;

    template<bool B, class T>
    struct maybe_optional_impl { using type = T; };
    template<class T>
    struct maybe_optional_impl<true, T> { using type = typename T::value_type; };

    template<class T>
    using maybe_optional_t = typename maybe_optional_impl<is_optional<T>, T>::type;

    class CopyableValue {
        public:
        rapidjson::Document document;
        // constructors
        CopyableValue() = default;
        CopyableValue(const rapidjson::Value& val) {
            document.CopyFrom(val, document.GetAllocator());
        }
        CopyableValue(const CopyableValue& copyable) : CopyableValue(copyable.document) {}
        // assignment
        void operator=(const rapidjson::Value& val) {
            document.CopyFrom(val, document.GetAllocator());
        }
        void operator=(const CopyableValue& copyable) {
            document.CopyFrom(copyable.document, document.GetAllocator());
        }
        // comparison
        bool operator==(const CopyableValue&) const { return true; };
    };

    template<class T>
    struct ConstructorRunner {
        ConstructorRunner() { T(); }
    };

    template<class T>
    inline std::string CppTypeName(const T& var) {
        char* realname = abi::__cxa_demangle(typeid(var).name(), 0, 0, 0);
        std::string s(realname);
        free(realname);
        return s;
    }
    inline std::string JsonTypeName(rapidjson::Value const& jsonValue) {
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
            if(jsonValue.IsInt()) return "int";
            if(jsonValue.IsUint()) return "uint";
            if(jsonValue.IsInt64()) return "int64";
            if(jsonValue.IsUint64()) return "uint64";
            if(jsonValue.IsFloat()) return "float";
            if(jsonValue.IsDouble()) return "double";
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

    template<class T>
    inline T GetValueType(rapidjson::Value const& jsonValue, const T& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    inline T GetValueType(rapidjson::Value const& jsonValue, const std::optional<T>& _) {
        return jsonValue.Get<T>();
    }

    template<class T>
    inline bool GetIsType(rapidjson::Value const& jsonValue, const T& _) {
        return jsonValue.Is<T>();
    }

    template<class T>
    inline bool GetIsType(rapidjson::Value const& jsonValue, const std::optional<T>& _) {
        return jsonValue.Is<T>();
    }

    template<class T>
    inline T NewType(const std::vector<T>& _) {
        return T();
    }

    template<class T>
    inline T NewType(const std::optional<std::vector<T>>& _) {
        return T();
    }

    template<class T>
    inline T NewType(const std::map<std::string, T>& _) {
        return T();
    }

    template<class T>
    inline T NewType(const std::optional<std::map<std::string, T>>& _) {
        return T();
    }
}
