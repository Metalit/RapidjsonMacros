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

#include <functional>
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
    T::Deserialize(&t, d);
    T::Serialize(&t, d.GetAllocator()).IsObject();
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

    template <class From, class U, class... Ts>
    struct first_convertible_impl {
        using type = std::conditional_t<std::is_convertible_v<From, U>, U, typename first_convertible_impl<From, Ts...>::type>;
    };
    template <class From, class U>
    struct first_convertible_impl<From, U> {
        using type = std::conditional_t<std::is_convertible_v<From, U>, U, From>;
    };

    template <class From, class... Ts>
    using first_convertible_t = typename first_convertible_impl<From, Ts...>::type;

    template <class T>
    struct container_impl {
        static constexpr rapidjson::Type type = rapidjson::kObjectType;
        static_assert(std::is_same_v<T, bool>, "Invalid member type");
    };
    template <class T>
    struct container_impl<std::vector<T>> {
        static constexpr rapidjson::Type type = rapidjson::kArrayType;
    };
    template <class T>
    struct container_impl<StringKeyedMap<T>> {
        static constexpr rapidjson::Type type = rapidjson::kObjectType;
    };

    template <class T>
    constexpr rapidjson::Type container_t = container_impl<T>::type;

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
        bool operator==(CopyableValue const& rhs) const { return document == rhs.document; };
        // clear helper
        void Clear() { document.SetObject(); }
    };

    template <class T>
    struct ConstructorRunner {
        ConstructorRunner() { T(); }
    };

    template <class T>
    concept HasIalizers = requires(T t) {
        T::serializers().size();
        T::deserializers().size();
    };

    template <class T>
    using SerializersT = std::vector<std::function<void(T const*, rapidjson::Value&, rapidjson::Document::AllocatorType&)>>;
    template <class T>
    using DeserializersT = std::vector<std::function<void(T*, rapidjson::Value&)>>;

    template <class T>
    SerializersT<T> Serializers() {
        return {};
    }
    template <class T>
    DeserializersT<T> Deserializers() {
        return {};
    }

    template <class T, class P>
    SerializersT<T> Serializers() {
        if constexpr (HasIalizers<P>) {
            SerializersT<T> ret;
            for (auto& f : P::serializers()) {
                ret.emplace_back([f](T const* self, auto& p1, auto& p2) { f(static_cast<P::SelfType const*>(self), p1, p2); });
            }
            return ret;
        } else
            return {};
    }
    template <class T, class P>
    DeserializersT<T> Deserializers() {
        if constexpr (HasIalizers<P>) {
            DeserializersT<T> ret;
            for (auto& f : P::deserializers()) {
                ret.emplace_back([f](T* self, auto& p1) { f(static_cast<P::SelfType*>(self), p1); });
            }
            return ret;
        } else
            return {};
    }

    template <class T, class P1, class P2, class... Ps>
    SerializersT<T> Serializers() {
        auto ret = Serializers<T, P2, Ps...>();
        for (auto& f : Serializers<T, P1>())
            ret.emplace_back(f);
        return ret;
    }

    template <class T, class P1, class P2, class... Ps>
    DeserializersT<T> Deserializers() {
        auto ret = Deserializers<T, P2, Ps...>();
        for (auto& f : Deserializers<T, P1>())
            ret.emplace_back(f);
        return ret;
    }

    template <class T, class... Ps>
    struct Parent : Ps... {
        static rapidjson::Value Serialize(T const* self, rapidjson::Document::AllocatorType& allocator) {
            rapidjson::Value jsonObject(rapidjson::kObjectType);
            if (T::keepExtraFields && self->extraFields.has_value())
                jsonObject.CopyFrom(self->extraFields->document, allocator);
            for (auto& method : serializers())
                method(self, jsonObject, allocator);
            return jsonObject;
        }
        static void Deserialize(T* self, rapidjson::Value& jsonValue) {
            for (auto& method : deserializers())
                method(self, jsonValue);
            if (T::keepExtraFields)
                self->extraFields = jsonValue;
        }
        template <class S, class P>
        friend SerializersT<S> Serializers();
        template <class S, class P>
        friend DeserializersT<S> Deserializers();
        static inline constexpr bool keepExtraFields = false;
        std::optional<rapidjson_macros_types::CopyableValue> extraFields = std::nullopt;
        bool operator==(Parent<T, Ps...> const& rhs) const {
            // if only I could do a default operator== outside of the class :(
            rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> allocator;
            return T::Serialize((T*) this, allocator) == T::Serialize((T*) &rhs, allocator);
        };
        using SelfType = T;

       protected:
        static inline SerializersT<T>& serializers() {
            static auto instance = Serializers<T, Ps...>();
            return instance;
        }
        static inline DeserializersT<T>& deserializers() {
            static auto instance = Deserializers<T, Ps...>();
            return instance;
        }
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
    struct EmplaceWrapper {
        T* reference;

        T& ref() { return *reference; }
        EmplaceWrapper(std::vector<T>& vector) { reference = &vector.emplace_back(); }
        void finish() {}
    };

    template <>
    struct EmplaceWrapper<bool> {
        bool* reference;
        bool value = false;
        std::vector<bool>& vec;

        bool& ref() { return *reference; }
        EmplaceWrapper(std::vector<bool>& vector) : vec(vector) { reference = &value; }
        // don't use destructor to avoid adding on exceptions
        void finish() { vec.emplace_back(value); }
    };
}
