#pragma once

#include <iostream>

#include "macros.hpp"

namespace RapidjsonMacros {
    DECLARE_JSON_STRUCT(TestClass) {
        SERIALIZE_ACTION(0, std::cout << "Serialize action\n";)
        DESERIALIZE_ACTION(0, std::cout << "Deserialize action\n";)
    };
}
