#include <futureio/chain/abi_def.hpp>
#include <fc/utility.hpp>

namespace futureio { namespace chain {

vector<type_def> common_type_defs() {
   vector<type_def> types;

   types.push_back( type_def{"account_name", "name"} );
   types.push_back( type_def{"permission_name", "name"} );
   types.push_back( type_def{"action_name", "name_ex"} );
   types.push_back( type_def{"table_name", "name"} );
   types.push_back( type_def{"transaction_id_type", "checksum256"} );
   types.push_back( type_def{"block_id_type", "checksum256"} );
   types.push_back( type_def{"weight_type", "uint16"} );

   return types;
}

abi_def futureio_contract_abi(const abi_def& futureio_system_abi)
{
   abi_def future_abi(futureio_system_abi);

   if( future_abi.version.size() == 0 ) {
      future_abi.version = "futureio::abi/1.0";
   }

   fc::move_append(future_abi.types, common_type_defs());

   // transaction
   future_abi.structs.emplace_back( struct_def {
      "permission_level", "", {
         {"actor", "account_name"},
         {"permission", "permission_name"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "action", "", {
         {"account", "account_name"},
         {"name", "action_name"},
         {"authorization", "permission_level[]"},
         {"data", "bytes"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "extension", "", {
         {"type", "uint16"},
         {"data", "bytes"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "transaction_header", "", {
         {"expiration", "time_point_sec"},
         {"ref_block_num", "uint16"},
         {"ref_block_prefix", "uint32"},
         {"max_net_usage_words", "varuint32"},
         {"max_cpu_usage_ms", "uint8"},
         {"delay_sec", "varuint32"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "transaction", "transaction_header", {
         {"context_free_actions", "action[]"},
         {"actions", "action[]"},
         {"transaction_extensions", "extension[]"}
      }
   });

   // block_header

   future_abi.structs.emplace_back( struct_def {
      "producer_key", "", {
         {"producer_name", "account_name"},
         {"block_signing_key", "public_key"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "producer_schedule", "", {
         {"version", "uint32"},
         {"producers", "producer_key[]"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "block_header", "", {
         {"timestamp", "uint32"},
         {"proposer", "account_name"},
         {"proposerProof", "string"},
         {"version", "uint32"},
         {"previous", "block_id_type"},
         {"transaction_mroot", "checksum256"},
         {"action_mroot", "checksum256"},
         {"committee_mroot", "checksum256"},
         {"header_extensions", "extension[]"}
      }
   });

   // authority
   future_abi.structs.emplace_back( struct_def {
      "key_weight", "", {
         {"key", "public_key"},
         {"weight", "weight_type"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "permission_level_weight", "", {
         {"permission", "permission_level"},
         {"weight", "weight_type"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "wait_weight", "", {
         {"wait_sec", "uint32"},
         {"weight", "weight_type"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "authority", "", {
         {"threshold", "uint32"},
         {"keys", "key_weight[]"},
         {"accounts", "permission_level_weight[]"},
         {"waits", "wait_weight[]"}
      }
   });

   // TODO add any ricardian_clauses
   //
   // ACTION PAYLOADS

   future_abi.structs.emplace_back( struct_def {
      "newaccount", "", {
         {"creator", "account_name"},
         {"name", "account_name"},
         {"owner", "authority"},
         {"active", "authority"},
         {"updateable", "bool"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "setcode", "", {
         {"account", "account_name"},
         {"vmtype", "uint8"},
         {"vmversion", "uint8"},
         {"code", "bytes"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "setabi", "", {
         {"account", "account_name"},
         {"abi",     "bytes"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "updateauth", "", {
         {"account", "account_name"},
         {"permission", "permission_name"},
         {"parent", "permission_name"},
         {"auth", "authority"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "deleteauth", "", {
         {"account", "account_name"},
         {"permission", "permission_name"},
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "linkauth", "", {
         {"account", "account_name"},
         {"code", "account_name"},
         {"type", "action_name"},
         {"requirement", "permission_name"},
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "unlinkauth", "", {
         {"account", "account_name"},
         {"code", "account_name"},
         {"type", "action_name"},
      }
   });

   future_abi.structs.emplace_back( struct_def {
      "canceldelay", "", {
         {"canceling_auth", "permission_level"},
         {"trx_id", "transaction_id_type"},
      }
   });

   future_abi.structs.emplace_back( struct_def {
         "onerror", "", {
            {"sender_id", "uint128"},
            {"sent_trx",  "bytes"}
      }
   });

   future_abi.structs.emplace_back( struct_def {
         "onblock", "", {
            {"header", "block_header"}
      }
   });

   // TODO add ricardian contracts
   future_abi.actions.push_back( action_def{action_name("newaccount"), "newaccount",""} );
   future_abi.actions.push_back( action_def{action_name("setcode"), "setcode",""} );
   future_abi.actions.push_back( action_def{action_name("setabi"), "setabi",""} );
   future_abi.actions.push_back( action_def{action_name("updateauth"), "updateauth",""} );
   future_abi.actions.push_back( action_def{action_name("deleteauth"), "deleteauth",""} );
   future_abi.actions.push_back( action_def{action_name("linkauth"), "linkauth",""} );
   future_abi.actions.push_back( action_def{action_name("unlinkauth"), "unlinkauth",""} );
   future_abi.actions.push_back( action_def{action_name("canceldelay"), "canceldelay",""} );
   future_abi.actions.push_back( action_def{action_name("onerror"), "onerror",""} );
   future_abi.actions.push_back( action_def{action_name("onblock"), "onblock",""} );

   return future_abi;
}

} } /// futureio::chain
