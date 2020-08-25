#ifndef ZC_NOTE_H_
#define ZC_NOTE_H_

#include "uint256.h"
#include "zcash.h"
#include "address.h"
#include "note_encryption.h"

#include <array>
#include <boost/optional.hpp>

namespace libzcash {

class BaseNote {
protected:
    uint64_t value_ = 0;
public:
    BaseNote() {}
    BaseNote(uint64_t value) : value_(value) {};
    virtual ~BaseNote() {};

    inline uint64_t value() const { return value_; };
};

class SaplingNote : public BaseNote {
public:
    diversifier_t d;
    uint256 pk_d;
    uint256 r;
    uint32_t ts = 0; // timestamp
    std::vector<SaplingPublicKey> source;

    SaplingNote(diversifier_t d, uint256 pk_d, uint64_t value, uint256 r, uint32_t ts = 0)
            : BaseNote(value), d(d), pk_d(pk_d), r(r), ts(ts) {}

    SaplingNote(diversifier_t d, uint256 pk_d, uint64_t value, uint256 r, uint32_t ts, const std::vector<SaplingPublicKey>& source)
            : BaseNote(value), d(d), pk_d(pk_d), r(r), ts(ts), source(source) {}

    SaplingNote() {};

    SaplingNote(const SaplingPaymentAddress &address, uint64_t value);

    virtual ~SaplingNote() {};

    boost::optional<uint256> cm() const;
    boost::optional<uint256> nullifier(const SaplingFullViewingKey &vk, const uint64_t position) const;
};

class BaseNotePlaintext {
protected:
    uint64_t value_ = 0;
    std::array<unsigned char, ZC_MEMO_SIZE> memo_;
public:
    BaseNotePlaintext() {}
    BaseNotePlaintext(const BaseNote& note, std::array<unsigned char, ZC_MEMO_SIZE> memo)
        : value_(note.value()), memo_(memo) {}
    virtual ~BaseNotePlaintext() {}

    inline uint64_t value() const { return value_; }
    inline const std::array<unsigned char, ZC_MEMO_SIZE> & memo() const { return memo_; }
};

//typedef std::pair<SaplingEncCiphertext, SaplingNoteEncryption> SaplingNotePlaintextEncryptionResult;
typedef std::pair<DynamicEncCiphertext, SaplingNoteEncryption> SaplingNotePlaintextEncryptionResult;

class SaplingNotePlaintext : public BaseNotePlaintext {
public:
    diversifier_t d;
    uint256 rcm;
    uint32_t ts; // timestamp
    std::vector<SaplingPublicKey> source;

    SaplingNotePlaintext() {}

    SaplingNotePlaintext(const SaplingNote& note, std::array<unsigned char, ZC_MEMO_SIZE> memo);

    static boost::optional<SaplingNotePlaintext> decrypt(
        const DynamicEncCiphertext &ciphertext,
        const uint256 &ivk,
        const uint256 &epk,
        const uint256 &cmu
    );

    static boost::optional<SaplingNotePlaintext> decrypt(
        const DynamicEncCiphertext &ciphertext,
        const uint256 &epk,
        const uint256 &esk,
        const uint256 &pk_d,
        const uint256 &cmu
    );

    boost::optional<SaplingNote> note(const SaplingIncomingViewingKey& ivk) const;

    virtual ~SaplingNotePlaintext() {}

    boost::optional<SaplingNotePlaintextEncryptionResult> encrypt(const uint256& pk_d) const;
};

class SaplingOutgoingPlaintext
{
public:
    uint256 pk_d;
    uint256 esk;

    SaplingOutgoingPlaintext() {};

    SaplingOutgoingPlaintext(uint256 pk_d, uint256 esk) : pk_d(pk_d), esk(esk) {}

    static boost::optional<SaplingOutgoingPlaintext> decrypt(
        const SaplingOutCiphertext &ciphertext,
        const uint256& ovk,
        const uint256& cv,
        const uint256& cm,
        const uint256& epk
    );

    static boost::optional<SaplingOutgoingPlaintext> decrypt(
        const DynamicEncCiphertext &ciphertext,
        const uint256 &ivk,
        const uint256 &epk
    ); 

    SaplingOutCiphertext encrypt(
        const uint256& ovk,
        const uint256& cv,
        const uint256& cm,
        SaplingNoteEncryption& enc
    ) const;

    boost::optional<DynamicOutCiphertext> encrypt(const uint256& pk_d, SaplingNoteEncryption& enc) const;

};


}

#endif // ZC_NOTE_H_
