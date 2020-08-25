#pragma once

#include <core/types.h>

namespace futureio {
    class Helper {
    public:
        static bool isGenesis(const BlockHeader& blockHeader);
    };
}