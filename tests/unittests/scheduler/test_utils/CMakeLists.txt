#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_library(scheduler_test_utils
        config_generators.cpp
        indication_generators.cpp)
target_link_libraries(scheduler_test_utils srslog mac_configuration_helpers sched_config sched_test_doubles)

add_library(scheduler_test_suite
        scheduler_test_suite.cpp
        scheduler_output_test_helpers.cpp
        scheduler_test_simulator.cpp)
target_link_libraries(scheduler_test_suite srsran_sched scheduler_test_utils gtest)

add_library(uci_test_utils ../uci_and_pucch/uci_test_utils.cpp)
target_link_libraries(uci_test_utils srslog mac_configuration_helpers sched_config)
