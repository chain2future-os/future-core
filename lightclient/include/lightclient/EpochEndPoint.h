#pragma once

#include <core/Message.h>

namespace futureio {
    class EpochEndPoint {
    public:
        static bool isEpochEndPoint(const BlockHeader& blockHeader);

        EpochEndPoint(const BlockHeader& blockHeader);

        std::string nextCommitteeMroot() const;

    private:
        BlockHeader m_blockHeader;

        std::string m_nextCommitteeMroot;
    };
}