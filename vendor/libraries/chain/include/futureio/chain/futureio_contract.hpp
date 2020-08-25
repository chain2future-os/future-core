/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once

#include <futureio/chain/types.hpp>
#include <futureio/chain/contract_types.hpp>

namespace futureio { namespace chain {

   class apply_context;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{
   void apply_futureio_newaccount(apply_context&);
   void apply_futureio_updateauth(apply_context&);
   void apply_futureio_deleteauth(apply_context&);
   void apply_futureio_linkauth(apply_context&);
   void apply_futureio_unlinkauth(apply_context&);
   void apply_futureio_delaccount(apply_context&);

   /*
   void apply_futureio_postrecovery(apply_context&);
   void apply_futureio_passrecovery(apply_context&);
   void apply_futureio_vetorecovery(apply_context&);
   */

   void apply_futureio_setcode(apply_context&);
   void apply_futureio_setabi(apply_context&);

   void apply_futureio_canceldelay(apply_context&);
   void apply_futureio_addwhiteblack(apply_context&);
   void apply_futureio_rmwhiteblack(apply_context&);
   ///@}  end action handlers

} } /// namespace futureio::chain
