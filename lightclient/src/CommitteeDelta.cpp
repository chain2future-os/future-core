#include <lightclient/CommitteeDelta.h>

#include <lightclient/CommitteeInfo.h>

namespace futureio {

    CommitteeDelta::CommitteeDelta(const std::list<CommitteeInfo>& add, const std::list<CommitteeInfo>& removed)
            : m_add(add), m_removed(removed) {}
}