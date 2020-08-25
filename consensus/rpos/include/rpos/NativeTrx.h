#pragma once

#include <futureio/chain/action.hpp>
#include <core/Evidence.h>
#include <core/types.h>

namespace futureio {
    using Action = futureio::chain::action;
    using ActionName = futureio::chain::action_name;
    using PermissionLevel = futureio::chain::permission_level;

    struct EmptyBlockReason {
        uint32_t blockNum;
        int phase;
        uint32_t baxCount;
        int currentPhase;
        uint32_t currentBaxCount;
        int proposeCount;
        bool isBa0Empty;
    };

    struct MaxBaxCountStatistics {
        uint32_t blockNum;
        int phase;
        uint32_t baxCount;
        int currentPhase;
        uint32_t currentBaxCount;
    };

    class NativeTrx {
    public:
        static const std::string kDesc;

        static const std::string kEvidence;

        static void sendEvilTrx(const AccountName& reporter, const chain::private_key_type& sk, const AccountName& evil, const Evidence& evidence);

        static void reportEvil(const EvilDesc& desc, const Evidence& evidence);

        static void reportEmptyBlockReason(const std::string& chain_name, uint32_t blockNum, const EmptyBlockReason& reason);

        static void reportMaxBaxCountStatistics(const std::string& chain_name, uint32_t blockNum, const MaxBaxCountStatistics& statistics);
    private:
        static SignedTransaction buildTrx(const Action& action, const BlockIdType& referId, const ChainIdType& chainId,
                const chain::private_key_type& sk, const fc::time_point_sec& expiration, fc::unsigned_int usageWords);
    };
}

FC_REFLECT( futureio::EmptyBlockReason, (blockNum)(phase)(baxCount)(currentPhase)(currentBaxCount)(proposeCount)(isBa0Empty))
FC_REFLECT( futureio::MaxBaxCountStatistics, (blockNum)(phase)(baxCount)(currentPhase)(currentBaxCount))
