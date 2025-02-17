#pragma once

#include <iostream>

#include "macros.hpp"

namespace RapidjsonMacros {
    struct CtorTestHelper {
        int x;
    };

    DECLARE_JSON_STRUCT(CtorTest) {
        VALUE(int, x);
        CtorTest(CtorTestHelper const& other) {
            x = other.x;
        }
        CtorTest() = default;
    };

    DECLARE_JSON_STRUCT(TestClass) {
       private:
        SERIALIZE_FUNCTION(serializeLog) {
            std::cout << "Serialize function\n";
        }
        DESERIALIZE_FUNCTION(deserializeLog) {
            std::cout << "Deserialize function\n";
        }

       public:
        VALUE(int, testval);
        VALUE_DEFAULT(int, testval_def, 1);
        int def = 2;
        VALUE_DEFAULT(int, testval_self_def, self->def);
        VALUE_DEFAULT(int, testval_json_def, jsonValue["why do this"].GetInt());
        VALUE_DEFAULT(int, testval_multi_def, self->def) = -1;
        VECTOR_DEFAULT(bool, testvec_bool, std::vector({ false, false, true }));
        VECTOR_DEFAULT(int, testvec_int, std::vector({ 0, 1, 2, 3 }));
        VALUE_DEFAULT(CtorTest, testval_ctor, {});
    };
}
