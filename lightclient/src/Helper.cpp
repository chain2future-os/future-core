#include <lightclient/Helper.h>

namespace futureio {
    bool Helper::isGenesis(const BlockHeader& blockHeader) {
        return std::string(blockHeader.proposer) == std::string("genesis");
    }
}