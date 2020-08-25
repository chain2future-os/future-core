#include "core/ExtType.h"

namespace futureio {
    bool ExtType::operator == (const ExtType& rhs) const {
        if (this == &rhs) {
            return true;
        }
        return key == rhs.key && value == rhs.value;
    }

    bool ExtType::operator != (const ExtType& rhs) const {
        return !this->operator==(rhs);
    }
}
