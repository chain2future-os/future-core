#ifndef _S_TRANSACTION_H_
#define _S_TRANSACTION_H_

#include <fc/reflect/reflect.hpp>
#include "snark/uint256.h"
#include "snark/note_encryption.h"
#include "snark/serialize.h"
#include "snark/util_str_encodings.h"

typedef int64_t CAmount;
typedef std::array<unsigned char, 64> spend_auth_sig_t;
typedef std::array<unsigned char, 64> binding_sig_t;
static constexpr size_t GROTH_PROOF_SIZE = (
    48 + // π_A
    96 + // π_B
    48); // π_C
typedef std::array<unsigned char, GROTH_PROOF_SIZE> GrothProof;

class SpendDescription
{
public:
    zero::uint256 cv;                    //!< A value commitment to the value of the input note.
    zero::uint256 anchor;                //!< A Merkle root of the Sapling note commitment tree at some block height in the past.
    zero::uint256 nullifier;             //!< The nullifier of the input note.
    zero::uint256 rk;                    //!< The randomized public key for spendAuthSig.
    GrothProof zkproof;            //!< A zero-knowledge proof using the spend circuit.
    spend_auth_sig_t spendAuthSig; //!< A signature authorizing this spend.

    SpendDescription() { }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(cv);
        READWRITE(anchor);
        READWRITE(nullifier);
        READWRITE(rk);
        READWRITE(zkproof);
        READWRITE(spendAuthSig);
    }

    friend bool operator==(const SpendDescription& a, const SpendDescription& b)
    {
        return (
            a.cv == b.cv &&
            a.anchor == b.anchor &&
            a.nullifier == b.nullifier &&
            a.rk == b.rk &&
            a.zkproof == b.zkproof &&
            a.spendAuthSig == b.spendAuthSig
            );
    }

    friend bool operator!=(const SpendDescription& a, const SpendDescription& b)
    {
        return !(a == b);
    }
};

class OutputDescription
{
public:
    zero::uint256 cv;                     //!< A value commitment to the value of the output note.
    zero::uint256 cm;                     //!< The note commitment for the output note.
    zero::uint256 ephemeralKey;           //!< A Jubjub public key.
    zero::uint256 outEphemeralKey;        //!< A Jubjub public key for outCiphertext.
    //libzcash::SaplingEncCiphertext encCiphertext; //!< A ciphertext component for the encrypted output note.
    libzcash::DynamicEncCiphertext encCiphertext; //!< A ciphertext component for the encrypted output note.
    //libzcash::SaplingOutCiphertext outCiphertext; //!< A ciphertext component for the encrypted output note.
    libzcash::DynamicOutCiphertext outCiphertext; //!< A ciphertext component for the encrypted output note.
    GrothProof zkproof;   //!< A zero-knowledge proof using the output circuit.

    OutputDescription() { }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(cv);
        READWRITE(cm);
        READWRITE(ephemeralKey);
        READWRITE(outEphemeralKey);
        READWRITE(encCiphertext);
        READWRITE(outCiphertext);
        READWRITE(zkproof);
    }

    friend bool operator==(const OutputDescription& a, const OutputDescription& b)
    {
        return (
            a.cv == b.cv &&
            a.cm == b.cm &&
            a.ephemeralKey == b.ephemeralKey &&
            a.outEphemeralKey == b.outEphemeralKey &&
            a.encCiphertext == b.encCiphertext &&
            a.outCiphertext == b.outCiphertext &&
            a.zkproof == b.zkproof
            );
    }

    friend bool operator!=(const OutputDescription& a, const OutputDescription& b)
    {
        return !(a == b);
    }
};

class BaseOutPoint
{
public:
    zero::uint256 hash;
    uint32_t n;

    BaseOutPoint() { SetNull(); }
    BaseOutPoint(zero::uint256 hashIn, uint32_t nIn) { hash = hashIn; n = nIn; }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(hash);
        READWRITE(n);
    }

    void SetNull() { hash.SetNull(); n = (uint32_t) -1; }
    bool IsNull() const { return (hash.IsNull() && n == (uint32_t) -1); }

    friend bool operator<(const BaseOutPoint& a, const BaseOutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const BaseOutPoint& a, const BaseOutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const BaseOutPoint& a, const BaseOutPoint& b)
    {
        return !(a == b);
    }
};

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint : public BaseOutPoint
{
public:
    COutPoint() : BaseOutPoint() {};
    COutPoint(zero::uint256 hashIn, uint32_t nIn) : BaseOutPoint(hashIn, nIn) {};
    std::string ToString() const
    {
        char buffer[64] = {0};
        snprintf(buffer, sizeof(buffer), "COutPoint(%s, %u)", hash.ToString().substr(0,10).c_str(), n);
        return buffer;
    }
};

struct CTxIn {
    COutPoint prevout;
    uint32_t nSequence;
};

struct CTxOut {
    CAmount nValue;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nValue);
    }
};

struct shielded_transaction {
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    CAmount valueBalance;
    std::vector<SpendDescription> vShieldedSpend;
    std::vector<OutputDescription> vShieldedOutput;
    binding_sig_t bindingSig;
};

struct ShieldedSpendPack {
    std::string cv;
    std::string anchor;
    std::string nullifier;
    std::string rk;
    std::string zkproof;
    std::string spendAuthSig;
    //GrothProof zkproof;
    //spend_auth_sig_t spendAuthSig;

    ShieldedSpendPack() {
    }

    ShieldedSpendPack(const SpendDescription& spend) {
        cv = spend.cv.GetHex();
        anchor = spend.anchor.GetHex();
        nullifier = spend.nullifier.GetHex();
        rk = spend.rk.GetHex();
        zkproof = HexStr(spend.zkproof);
        spendAuthSig = HexStr(spend.spendAuthSig);
    }

    SpendDescription toShieldedSpendDesc() const {
        SpendDescription spend;
        spend.cv.SetHex(cv);
        spend.anchor.SetHex(anchor);
        spend.nullifier.SetHex(nullifier);
        spend.rk.SetHex(rk);

        std::vector<unsigned char> bytes = ParseHex(zkproof);
        for (size_t i = 0; i < bytes.size() && i < spend.zkproof.size(); i++) {
            spend.zkproof[i] = bytes[i];
        }

        bytes = ParseHex(spendAuthSig);
        for (size_t i = 0; i < bytes.size() && i < spend.spendAuthSig.size(); i++) {
            spend.spendAuthSig[i] = bytes[i];
        }
 
        return spend;
    }
};

struct ShieldedOutputPack {
    std::string cv;
    std::string cm;
    std::string ephemeralKey;
    std::string outEphemeralKey;
    std::string encCiphertext;
    std::string outCiphertext;
    std::string zkproof;
    //libzcash::SaplingEncCiphertext encCiphertext;
    //libzcash::SaplingOutCiphertext outCiphertext;
    //GrothProof zkproof;

    ShieldedOutputPack() {
    }

    ShieldedOutputPack(const OutputDescription& output) {
        cv = output.cv.GetHex();
        cm = output.cm.GetHex();
        ephemeralKey = output.ephemeralKey.GetHex();
        outEphemeralKey = output.outEphemeralKey.GetHex();
        encCiphertext = HexStr(output.encCiphertext);
        outCiphertext = HexStr(output.outCiphertext);
        zkproof = HexStr(output.zkproof);
    }

    OutputDescription toShieldedOutputDesc() const {
        OutputDescription output;
        output.cv.SetHex(cv);
        output.cm.SetHex(cm);
        output.ephemeralKey.SetHex(ephemeralKey);
        output.outEphemeralKey.SetHex(outEphemeralKey);

        std::vector<unsigned char> bytes = ParseHex(encCiphertext);
        output.encCiphertext.resize(bytes.size());
        for (size_t i = 0; i < bytes.size(); i++) {
            output.encCiphertext[i] = bytes[i];
        }

        bytes = ParseHex(outCiphertext);
        output.outCiphertext.resize(bytes.size());
        for (size_t i = 0; i < bytes.size(); i++) {
            output.outCiphertext[i] = bytes[i];
        }

        bytes = ParseHex(zkproof);
        for (size_t i = 0; i < bytes.size() && output.zkproof.size(); i++) {
            output.zkproof[i] = bytes[i];
        }

        return output;
    }
};

struct OutPointPack {
    std::string hash;
    uint32_t n;

    OutPointPack() {
    }

    OutPointPack(const COutPoint& point) {
        hash = point.hash.GetHex();
        n = point.n;
    }

    COutPoint toOutPoint() const {
        COutPoint point;
        point.hash.SetHex(hash);
        point.n = n;
        return point;
    }
};

struct TxInPack {
    OutPointPack prevout;
    uint32_t nSequence;

    TxInPack() {
    }

    TxInPack(const CTxIn& in) : prevout(in.prevout) {
        nSequence = in.nSequence;
    }

    CTxIn toTxIn() const {
        CTxIn in;
        in.prevout = prevout.toOutPoint();
        in.nSequence = nSequence;
        return in;
    }
};

struct ShieldedTrxPack {
    std::vector<TxInPack> vin;
    std::vector<CAmount> vout;
    CAmount valueBalance;
    std::vector<ShieldedSpendPack> vShieldedSpend;
    std::vector<ShieldedOutputPack> vShieldedOutput;
    //binding_sig_t bindingSig;
    std::string bindingSig;

    ShieldedTrxPack() {
    }

    ShieldedTrxPack(const shielded_transaction& trx) {
        for (auto& in : trx.vin) {
            vin.emplace_back(in);
        }

        for (auto& out : trx.vout) {
            vout.emplace_back(out.nValue);
        }

        valueBalance = trx.valueBalance;

        for (auto& spend : trx.vShieldedSpend) {
            vShieldedSpend.emplace_back(spend);
        }

        for (auto& output: trx.vShieldedOutput) {
            vShieldedOutput.emplace_back(output);
        }

        bindingSig = HexStr(trx.bindingSig);
    }

    shielded_transaction toShieldedTrx() const {
        shielded_transaction trx;
        for (auto& in : vin) {
            trx.vin.push_back(in.toTxIn());
        }

        for (auto& out : vout) {
            CTxOut txOut;
            txOut.nValue = out;
            trx.vout.push_back(txOut);
        }

        trx.valueBalance = valueBalance;

        for (auto& spend : vShieldedSpend) {
            trx.vShieldedSpend.push_back(spend.toShieldedSpendDesc());
        }

        for (auto& output : vShieldedOutput) {
            trx.vShieldedOutput.push_back(output.toShieldedOutputDesc());
        }

        std::vector<unsigned char> bytes = ParseHex(bindingSig);
        for (size_t i = 0; i < bytes.size() && i < trx.bindingSig.size(); i++) {
            trx.bindingSig[i] = bytes[i];
        }

        return trx;
    }
};

FC_REFLECT( OutPointPack, (hash)(n))
FC_REFLECT( TxInPack, (prevout)(nSequence))
FC_REFLECT( ShieldedSpendPack, (cv)(anchor)(nullifier)(rk)(zkproof)(spendAuthSig))
FC_REFLECT( ShieldedOutputPack, (cv)(cm)(ephemeralKey)(outEphemeralKey)(encCiphertext)(outCiphertext)(zkproof))
FC_REFLECT( ShieldedTrxPack, (vin)(vout)(valueBalance)(vShieldedSpend)(vShieldedOutput)(bindingSig))

#endif
