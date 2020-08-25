#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include "uint256.h"
#include "uint252.h"
#include "zcash.h"
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace libzcash {
class InvalidEncoding {
public:
    friend bool operator==(const InvalidEncoding &a, const InvalidEncoding &b) { return true; }
    friend bool operator<(const InvalidEncoding &a, const InvalidEncoding &b) { return true; }
};

const size_t SerializedSaplingPaymentAddressSize = 43;
const size_t SerializedSaplingFullViewingKeySize = 96;
const size_t SerializedSaplingExpandedSpendingKeySize = 96;
const size_t SerializedSaplingSpendingKeySize = 32;

typedef std::array<unsigned char, ZC_DIVERSIFIER_SIZE> diversifier_t;
typedef std::array<unsigned char, ZC_PUBLIC_KEY_SIZE> SaplingPublicKey;

class ReceivingKey : public zero::uint256 {
public:
    ReceivingKey() { }
    ReceivingKey(zero::uint256 sk_enc) : zero::uint256(sk_enc) { }

    zero::uint256 pk_enc() const;
};

//! Sapling functions. 
class SaplingPaymentAddress {
public:
    diversifier_t d;
    zero::uint256 pk_d;

    SaplingPaymentAddress() : d(), pk_d() { }
    SaplingPaymentAddress(diversifier_t d, zero::uint256 pk_d) : d(d), pk_d(pk_d) { }

    //! Get the 256-bit SHA256d hash of this payment address.
    zero::uint256 GetHash() const;

    friend inline bool operator==(const SaplingPaymentAddress& a, const SaplingPaymentAddress& b) {
        return a.d == b.d && a.pk_d == b.pk_d;
    }
    friend inline bool operator<(const SaplingPaymentAddress& a, const SaplingPaymentAddress& b) {
        return (a.d < b.d ||
                (a.d == b.d && a.pk_d < b.pk_d));
    }
};

class SaplingIncomingViewingKey : public zero::uint256 {
public:
    SaplingIncomingViewingKey() : zero::uint256() { }
    SaplingIncomingViewingKey(zero::uint256 ivk) : zero::uint256(ivk) { }
    
    // Can pass in diversifier for Sapling addr
    boost::optional<SaplingPaymentAddress> address(diversifier_t d) const;
};

class SaplingFullViewingKey {
public: 
    zero::uint256 ak;
    zero::uint256 nk;
    zero::uint256 ovk; 
    
    SaplingFullViewingKey() : ak(), nk(), ovk() { }
    SaplingFullViewingKey(zero::uint256 ak, zero::uint256 nk, zero::uint256 ovk) : ak(ak), nk(nk), ovk(ovk) { }

    //! Get the fingerprint of this full viewing key (as defined in ZIP 32).
    zero::uint256 GetFingerprint() const;

    SaplingIncomingViewingKey in_viewing_key() const;
    bool is_valid() const;

    friend inline bool operator==(const SaplingFullViewingKey& a, const SaplingFullViewingKey& b) {
        return a.ak == b.ak && a.nk == b.nk && a.ovk == b.ovk;
    }
    friend inline bool operator<(const SaplingFullViewingKey& a, const SaplingFullViewingKey& b) {
        return (a.ak < b.ak || 
            (a.ak == b.ak && a.nk < b.nk) || 
            (a.ak == b.ak && a.nk == b.nk && a.ovk < b.ovk));
    }
};


class SaplingExpandedSpendingKey {
public: 
    zero::uint256 ask;
    zero::uint256 nsk;
    zero::uint256 ovk;
    
    SaplingExpandedSpendingKey() : ask(), nsk(), ovk() { }
    SaplingExpandedSpendingKey(zero::uint256 ask, zero::uint256 nsk, zero::uint256 ovk) : ask(ask), nsk(nsk), ovk(ovk) { }
    
    SaplingFullViewingKey full_viewing_key() const;
    
    friend inline bool operator==(const SaplingExpandedSpendingKey& a, const SaplingExpandedSpendingKey& b) {
        return a.ask == b.ask && a.nsk == b.nsk && a.ovk == b.ovk;
    }
    friend inline bool operator<(const SaplingExpandedSpendingKey& a, const SaplingExpandedSpendingKey& b) {
        return (a.ask < b.ask || 
            (a.ask == b.ask && a.nsk < b.nsk) || 
            (a.ask == b.ask && a.nsk == b.nsk && a.ovk < b.ovk));
    }
};

class SaplingSpendingKey : public zero::uint256 {
public:
    SaplingSpendingKey() : zero::uint256() { }
    SaplingSpendingKey(zero::uint256 sk) : zero::uint256(sk) { }

    static SaplingSpendingKey random();

    SaplingExpandedSpendingKey expanded_spending_key() const;
    SaplingFullViewingKey full_viewing_key() const;
    
    // Can derive Sapling addr from default diversifier 
    SaplingPaymentAddress default_address() const;

    boost::optional<SaplingPublicKey> default_public_key() const;
};

typedef boost::variant<InvalidEncoding, /*SproutPaymentAddressi*,*/ SaplingPaymentAddress> PaymentAddress;

}

/** Check whether a PaymentAddress is not an InvalidEncoding. */
bool IsValidPaymentAddress(const libzcash::PaymentAddress& zaddr);

#endif // _ADDRESS_H_:w
