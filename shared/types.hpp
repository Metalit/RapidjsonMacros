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
concept JSONClassDerived = std::is_base_of_v<JSONClass, std::decay_t<T>>;

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
    struct remove_optional_impl { using type = T; };
    template<class T>
    struct remove_optional_impl<true, T> { using type = typename T::value_type; };

    template<class T>
    using remove_optional_t = typename remove_optional_impl<is_optional<T>, T>::type;

    template <class T, class U, class... Ts>
    struct uniq_impl {
        static constexpr bool value = !std::is_same_v<T, U> && (!std::is_same_v<T, Ts> && ...) && uniq_impl<U, Ts...>::value;
    };
    template <class T, class U>
    struct uniq_impl<T, U> {
        static constexpr bool value = !std::is_same_v<T, U>;
    };

    template <class... Ts>
    concept all_unique = uniq_impl<Ts...>::value;

    template<class To, class U, class... Ts>
    struct first_convertible_impl {
        using type = std::conditional_t<std::is_convertible_v<U, To>, U, typename first_convertible_impl<To, Ts...>::type>;
    };
    template<class To, class U>
    struct first_convertible_impl<To, U> {
        using type = std::conditional_t<std::is_convertible_v<U, To>, U, To>;
    };

    template <class To, class... Ts>
    using first_convertible_t = typename first_convertible_impl<To, Ts...>::type;

    template<class T>
    inline constexpr rapidjson::Type container_t = rapidjson::kObjectType;
    template<class T>
    inline constexpr rapidjson::Type container_t<std::vector<T>> = rapidjson::kArrayType;
    template<class T>
    inline constexpr rapidjson::Type container_t<StringKeyedMap<T>> = rapidjson::kObjectType;

    struct SelfValueType {};

    class CopyableValue {
        public:
        rapidjson::Document document;
        // constructors
        CopyableValue() = default;
        CopyableValue(const rapidjson::Value& val) {
            Clear();
            document.CopyFrom(val, document.GetAllocator());
        }
        CopyableValue(const CopyableValue& copyable) : CopyableValue(copyable.document) {}
        // assignment
        void operator=(const rapidjson::Value& val) {
            Clear();
            document.CopyFrom(val, document.GetAllocator());
        }
        void operator=(const CopyableValue& copyable) {
            Clear();
            document.CopyFrom(copyable.document, document.GetAllocator());
        }
        // comparison
        bool operator==(const CopyableValue&) const { return true; };
        // clear helper
        void Clear() { if(document.IsObject()) document.RemoveAllMembers(); }
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
    template<class T>
    requires(std::is_convertible_v<T, std::string>)
    inline std::string CppTypeName(const T& var) {
        return "std::string";
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
    template<std::size_t N = 0>
    inline SelfValueType GetJSONString(const SelfValueType& string, rapidjson::Document::AllocatorType& allocator) {
        return string;
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
