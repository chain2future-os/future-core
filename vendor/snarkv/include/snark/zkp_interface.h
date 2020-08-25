#pragma once
#include "snark/stransaction.h"

namespace libsnark {

    // Groth16 verify proof algorithm
    // vk - verification key
    // primary_input - primary input
    // proof - proof
    bool verify_zero_knowledge_proof(char* vk, char* primary_input, char* proof);

    bool init_zksnark_params();

    // check shielded transaction
    bool check_shielded_transaction(const shielded_transaction& tx);
}// end of namespace
