#pragma once

#ifndef RAPIDJSON_MACROS_GCC_TEST
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#else
#include "fileio.hpp"
#define RAPIDJSON_HAS_STDSTRING 1
#include <cxxabi.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <optional>
#include <vector>
#endif

#include <concepts>
#include <map>

class JSONException : public std::exception {
   private:
    std::string message;

   public:
    explicit JSONException(std::string const& message) : message(message) {}
    char const* what() const noexcept override { return message.c_str(); }
};

template <class T>
concept JSONStruct = requires(T t, rapidjson::Document d) {
    t.Deserialize(d);
    t.Serialize(d.GetAllocator()).IsObject();
};

template <class T>
concept JSONBasicType = requires { rapidjson::internal::TypeHelper<rapidjson::Value, std::decay_t<T>>::Is; };

template <class T>
using StringKeyedMap = std::map<std::string, T>;

namespace rapidjson_macros_types {

    template <class From, class T>
    concept with_constructible = std::is_constructible_v<T, From>;

    template <typename T>
    concept callable = requires(T t) { t(); };

    template <typename T>
    concept is_optional = std::same_as<T, std::optional<typename T::value_type>>;

    template <bool B, class T>
    struct remove_optional_impl {
        using type = T;
    };
    template <class T>
    struct remove_optional_impl<true, T> {
        using type = typename T::value_type;
    };

    template <class T>
    using remove_optional_t = typename remove_optional_impl<is_optional<T>, T>::type;

    template <class T, class U, class... Ts>
    struct uniq_impl {
        static constexpr bool value = !std::is_same_v<T, U> && (!std::is_same_v<T, Ts> && ...) && uniq_impl<U, Ts...>::value;
    };
    template <class T, class U>
    struct uniq_impl<T, U> {
        static constexpr bool value = !std::is_same_v<T, U>;
    };
    template <class T>
    struct uniq_impl<T, bool> {  // dummy type, doesn't matter
        static constexpr bool value = true;
    };

    template <class... Ts>
    concept all_unique = uniq_impl<Ts...>::value;

    template <class To, class U, class... Ts>
    struct first_convertible_impl {
        using type = std::conditional_t<std::is_convertible_v<U, To>, U, typename first_convertible_impl<To, Ts...>::type>;
    };
    template <class To, class U>
    struct first_convertible_impl<To, U> {
        using type = std::conditional_t<std::is_convertible_v<U, To>, U, To>;
    };

    template <class To, class... Ts>
    using first_convertible_t = typename first_convertible_impl<To, Ts...>::type;

    template <class T>
    inline constexpr rapidjson::Type container_t = rapidjson::kObjectType;
    template <class T>
    inline constexpr rapidjson::Type container_t<std::vector<T>> = rapidjson::kArrayType;
    template <class T>
    inline constexpr rapidjson::Type container_t<StringKeyedMap<T>> = rapidjson::kObjectType;

    struct SelfValueType {};

    class CopyableValue {
       public:
        rapidjson::Document document;
        // constructors
        CopyableValue() = default;
        CopyableValue(rapidjson::Value const& val) {
            Clear();
            document.CopyFrom(val, document.GetAllocator());
        }
        CopyableValue(CopyableValue const& copyable) : CopyableValue(copyable.document) {}
        // assignment
        void operator=(rapidjson::Value const& val) {
            Clear();
            document.CopyFrom(val, document.GetAllocator());
        }
        void operator=(CopyableValue const& copyable) {
            Clear();
            document.CopyFrom(copyable.document, document.GetAllocator());
        }
        // comparison
        bool operator==(CopyableValue const&) const { return true; };
        // clear helper
        void Clear() {
            if (document.IsObject())
                document.RemoveAllMembers();
        }
    };

    template <class T>
    struct ConstructorRunner {
        ConstructorRunner() { T(); }
    };

    template <class T>
    inline std::string CppTypeName(T const& var) {
        char* realname = abi::__cxa_demangle(typeid(var).name(), 0, 0, 0);
        std::string s(realname);
        free(realname);
        return s;
    }
    template <class T>
    requires(std::is_convertible_v<T, std::string>)
    inline std::string CppTypeName(T const& var) {
        return "std::string";
    }
    template <class T>
    inline std::string CppTypeName(std::vector<T> const& var) {
        return "std::vector<" + CppTypeName(T()) + ">";
    }
    template <class T>
    inline std::string CppTypeName(StringKeyedMap<T> const& var) {
        return "StringKeyedMap<" + CppTypeName(T()) + ">";
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
                if (jsonValue.IsInt())
                    return "int";
                if (jsonValue.IsUint())
                    return "uint";
                if (jsonValue.IsInt64())
                    return "int64";
                if (jsonValue.IsUint64())
                    return "uint64";
                if (jsonValue.IsFloat())
                    return "float";
                if (jsonValue.IsDouble())
                    return "double";
                return "number";
            default:
                return "unknown";
        }
    }

    template <class T, class R, std::size_t N = 0>
    inline R GetJSONString(T const& string, rapidjson::Document::AllocatorType& allocator);

    template <class T, std::size_t N = 0>
    requires(std::is_convertible_v<T, std::string>)
    inline rapidjson::Value GetJSONString(T const& string, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(string, allocator);
    }
    template <std::size_t N = 0>
    inline rapidjson::Value::StringRefType GetJSONString(char const (&string)[N], rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value::StringRefType(string);
    }
    template <std::size_t N = 0>
    inline SelfValueType GetJSONString(SelfValueType const& string, rapidjson::Document::AllocatorType& allocator) {
        return string;
    }

    template <class T>
    inline rapidjson::Value CreateJSONValue(T& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value);
    }
    template <>
    inline rapidjson::Value CreateJSONValue(std::string const& value, rapidjson::Document::AllocatorType& allocator) {
        return rapidjson::Value(value, allocator);
    }

    template <class T>
    inline T GetValueType(rapidjson::Value const& jsonValue, T const& _) {
        return jsonValue.Get<T>();
    }

    template <class T>
    inline T GetValueType(rapidjson::Value const& jsonValue, std::optional<T> const& _) {
        return jsonValue.Get<T>();
    }

    template <class T>
    inline bool GetIsType(rapidjson::Value const& jsonValue, T const& _) {
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            return jsonValue.IsNumber();
        return jsonValue.Is<T>();
    }

    template <class T>
    inline bool GetIsType(rapidjson::Value const& jsonValue, std::optional<T> const& _) {
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            return jsonValue.IsNumber();
        return jsonValue.Is<T>();
    }

    template <class T>
    inline T NewType(std::vector<T> const& _) {
        return T();
    }

    template <class T>
    inline T NewType(std::optional<std::vector<T>> const& _) {
        return T();
    }

    template <class T>
    inline T NewType(std::map<std::string, T> const& _) {
        return T();
    }

    template <class T>
    inline T NewType(std::optional<std::map<std::string, T>> const& _) {
        return T();
    }
}
