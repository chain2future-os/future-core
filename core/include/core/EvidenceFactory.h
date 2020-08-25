#pragma once
#include <memory>
#include <string>

#include "core/Evidence.h"

namespace futureio {
    class EvidenceFactory {
    public:
        static std::shared_ptr<Evidence> create(const std::string& str);
    };
}
