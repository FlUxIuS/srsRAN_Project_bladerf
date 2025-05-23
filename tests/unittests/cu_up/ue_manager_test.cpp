/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "cu_up_test_helpers.h"
#include "lib/cu_up/ue_manager.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

/// Fixture class for UE manager tests
class ue_manager_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create required objects
    gtpu_rx_demux      = std::make_unique<dummy_gtpu_demux_ctrl>();
    gtpu_n3_allocator  = std::make_unique<dummy_gtpu_teid_pool>();
    gtpu_f1u_allocator = std::make_unique<dummy_gtpu_teid_pool>();
    gtpu_tx_notifier   = std::make_unique<dummy_gtpu_network_gateway_adapter>();
    f1u_gw             = std::make_unique<dummy_f1u_gateway>(f1u_bearer);
    e1ap               = std::make_unique<dummy_e1ap>();

    ngu_session_mngr = std::make_unique<dummy_ngu_session_manager>();

    cu_up_exec_mapper = std::make_unique<dummy_cu_up_executor_mapper>(&worker);

    // Create UE cfg
    ue_cfg = {security::sec_as_config{}, activity_notification_level_t::ue, std::chrono::seconds(0), {}, 1000000000};

    // create DUT object
    ue_mng = std::make_unique<ue_manager>(ue_manager_config{n3_config, test_mode_config},
                                          ue_manager_dependencies{*e1ap,
                                                                  timers,
                                                                  *f1u_gw,
                                                                  *ngu_session_mngr,
                                                                  *gtpu_rx_demux,
                                                                  *gtpu_n3_allocator,
                                                                  *gtpu_f1u_allocator,
                                                                  *cu_up_exec_mapper,
                                                                  gtpu_pcap,
                                                                  test_logger});
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<gtpu_demux_ctrl>                            gtpu_rx_demux;
  std::unique_ptr<gtpu_teid_pool>                             gtpu_n3_allocator;
  std::unique_ptr<gtpu_teid_pool>                             gtpu_f1u_allocator;
  std::unique_ptr<gtpu_tunnel_common_tx_upper_layer_notifier> gtpu_tx_notifier;
  std::unique_ptr<e1ap_control_message_handler>               e1ap;
  std::unique_ptr<cu_up_executor_mapper>                      cu_up_exec_mapper;
  dummy_inner_f1u_bearer                                      f1u_bearer;
  null_dlt_pcap                                               gtpu_pcap;
  std::unique_ptr<f1u_cu_up_gateway>                          f1u_gw;
  std::unique_ptr<ngu_session_manager>                        ngu_session_mngr;
  timer_manager                                               timers;
  ue_context_cfg                                              ue_cfg;
  std::unique_ptr<ue_manager_ctrl>                            ue_mng;
  n3_interface_config                                         n3_config;
  cu_up_test_mode_config                                      test_mode_config{};
  srslog::basic_logger&                                       test_logger = srslog::fetch_basic_logger("TEST", false);
  manual_task_worker                                          worker{64};

  async_task<void>                        t;
  std::optional<lazy_task_launcher<void>> t_launcher;
};

/// UE object handling tests (creation/deletion)
TEST_F(ue_manager_test, when_ue_db_not_full_new_ue_can_be_added)
{
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
  ue_context* ue = ue_mng->add_ue(ue_cfg);
  ASSERT_NE(ue, nullptr);
  ASSERT_EQ(ue_mng->get_nof_ues(), 1);
}

TEST_F(ue_manager_test, when_ue_db_is_full_new_ue_cannot_be_added)
{
  // add maximum number of UE objects
  for (uint32_t i = 0; i < MAX_NOF_UES; i++) {
    ue_context* ue = ue_mng->add_ue(ue_cfg);
    ASSERT_NE(ue, nullptr);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), MAX_NOF_UES);

  // try to add one more
  ue_context* ue = ue_mng->add_ue(ue_cfg);
  ASSERT_EQ(ue, nullptr);
}

TEST_F(ue_manager_test, when_ue_are_deleted_ue_db_is_empty)
{
  // add maximum number of UE objects
  for (uint32_t i = 0; i < MAX_NOF_UES; i++) {
    ue_context* ue = ue_mng->add_ue(ue_cfg);
    ASSERT_NE(ue, nullptr);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), MAX_NOF_UES);

  // delete all UE objects
  for (uint32_t i = 0; i < MAX_NOF_UES; i++) {
    t = ue_mng->remove_ue(int_to_ue_index(i));
    t_launcher.emplace(t);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
}
