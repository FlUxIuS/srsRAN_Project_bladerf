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

include_directories(../../..)

add_library(rrc_test_doubles rrc_test_messages.cpp rrc_test_message_validators.cpp rrc_packed_test_messages.cpp)
set_target_properties(rrc_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(rrc_test_doubles rrc_nr_asn1 srsran_rrc srsran_support srslog)
