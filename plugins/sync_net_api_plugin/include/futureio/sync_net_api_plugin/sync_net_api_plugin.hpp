/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once

#include <futureio/sync_net_plugin/sync_net_plugin.hpp>
#include <futureio/http_plugin/http_plugin.hpp>

#include <appbase/application.hpp>

namespace futureio {

using namespace appbase;

class sync_net_api_plugin : public plugin<sync_net_api_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES((sync_net_plugin) (http_plugin))

   sync_net_api_plugin() = default;
   sync_net_api_plugin(const sync_net_api_plugin&) = delete;
   sync_net_api_plugin(sync_net_api_plugin&&) = delete;
   sync_net_api_plugin& operator=(const sync_net_api_plugin&) = delete;
   sync_net_api_plugin& operator=(sync_net_api_plugin&&) = delete;
   virtual ~sync_net_api_plugin() override = default;

   virtual void set_program_options(options_description& cli, options_description& cfg) override {}
   void plugin_initialize(const variables_map& vm);
   void plugin_startup();
   void plugin_shutdown() {}

private:
};

}
