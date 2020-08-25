#pragma once

#include <map>

#include <core/Message.h>
#include <core/MultiProposeEvidence.h>

namespace futureio {
    class EvilMultiProposeDetector {
    public:
        bool hasMultiPropose(std::map<BlockIdType, ProposeMsg>& proposerMsgMap, const ProposeMsg& propose, MultiProposeEvidence& outEvidence) const;
    };
}