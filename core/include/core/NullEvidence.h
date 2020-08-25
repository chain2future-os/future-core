#pragma once

#include "core/Evidence.h"

namespace futureio {
    class NullEvidence : public Evidence {
    public:
        virtual bool isNull() const;
    };
}