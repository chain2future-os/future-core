/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once
#include <futureio/chain_plugin/chain_plugin.hpp>
#include <futureio/http_plugin/http_plugin.hpp>

#include <appbase/application.hpp>
#include <futureio/chain/controller.hpp>

namespace futureio {
   using futureio::chain::controller;
   using std::unique_ptr;
   using namespace appbase;

   class chain_info_api_plugin : public plugin<chain_info_api_plugin> {
      public:
        APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin))

        chain_info_api_plugin();
        virtual ~chain_info_api_plugin();

        virtual void set_program_options(options_description&, options_description&) override;

        void plugin_initialize(const variables_map&);
        void plugin_startup();
        void plugin_shutdown();

      private:
        unique_ptr<class chain_info_api_plugin_impl> my;
   };

}
