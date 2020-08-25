#pragma once

#include <string>
#include "core/types.h"

namespace futureio {
    struct ExtType {
        uint32_t key;
        std::string value;
        bool operator == (const ExtType& rhs) const;
        bool operator != (const ExtType& rhs) const;
    };
}

FC_REFLECT( futureio::ExtType, (key)(value))
