#pragma once

#include <fc/reflect/reflect.hpp>
#include <fc/variant.hpp>
#include <futureio/chain/block.hpp>

#include "core/ExtType.h"
#include "core/types.h"

namespace futureio {

    enum ConsensusPhase {
        kPhaseInit = 0,
        kPhaseBA0,
        kPhaseBA1,
        kPhaseBAX
    };

    typedef std::vector<ExtType> MsgExtension;

    struct ReqSyncMsg {
        uint32_t seqNum;
        uint32_t startBlockNum;
        uint32_t endBlockNum;
        MsgExtension ext;
    };

    struct ReqBlockNumRangeMsg {
        uint32_t seqNum;
        MsgExtension ext;
    };

    struct RspBlockNumRangeMsg {
        uint32_t seqNum;
        uint32_t firstNum;
        uint32_t lastNum;
        std::string blockHash;
        std::string prevBlockHash;
        MsgExtension ext;
    };

    struct SyncBlockMsg {
        uint32_t seqNum;
        Block block;
        std::string proof;
        MsgExtension ext;
    };

    struct SyncStopMsg {
        uint32_t seqNum;
        MsgExtension ext;
    };

    // propose message
    struct UnsignedProposeMsg {
        Block block;
        MsgExtension ext;
    };

    struct ProposeMsg : public UnsignedProposeMsg {
        std::string signature;
    };

    // echo message
    struct CommonEchoMsg {
        BlockIdType blockId;
        ConsensusPhase phase;
        uint32_t    baxCount;
        AccountName proposer;
        void toStringStream(std::stringstream& ss) const;

        bool fromStringStream(std::stringstream& ss);

        bool operator == (const CommonEchoMsg&) const;

        uint32_t blockNum() const;

        bool valid() const;
    };

    struct UnsignedEchoMsg : public CommonEchoMsg {
        std::string blsSignature;
        AccountName account;
        uint32_t    timestamp;
        MsgExtension ext;
        bool operator == (const UnsignedEchoMsg&) const;
    };

    struct EchoMsg : public UnsignedEchoMsg {
        std::string signature; // hex string
        bool operator == (const EchoMsg&) const;
    };
}

FC_REFLECT( futureio::UnsignedProposeMsg, (block)(ext))
FC_REFLECT_DERIVED( futureio::ProposeMsg, (futureio::UnsignedProposeMsg), (signature))

FC_REFLECT( futureio::CommonEchoMsg, (blockId)(phase)(baxCount)(proposer))
FC_REFLECT_DERIVED( futureio::UnsignedEchoMsg, (futureio::CommonEchoMsg), (blsSignature)(account)(timestamp)(ext))

FC_REFLECT_DERIVED( futureio::EchoMsg, (futureio::UnsignedEchoMsg), (signature))
FC_REFLECT( futureio::ReqSyncMsg, (seqNum)(startBlockNum)(endBlockNum)(ext))
FC_REFLECT( futureio::ReqBlockNumRangeMsg, (seqNum)(ext))
FC_REFLECT( futureio::RspBlockNumRangeMsg, (seqNum)(firstNum)(lastNum)(blockHash)(prevBlockHash)(ext))
FC_REFLECT( futureio::SyncBlockMsg, (seqNum)(block)(proof)(ext))
FC_REFLECT( futureio::SyncStopMsg, (seqNum)(ext))
