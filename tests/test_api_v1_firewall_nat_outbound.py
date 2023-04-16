# Copyright 2023 Jared Hendrickson
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Script used to test the /api/v1/firewall/nat/outbound/mapping endpoint."""
import e2e_test_framework


class APIE2ETestFirewallNATOutbound(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/nat/outbound endpoint."""
    uri = "/api/v1/firewall/nat/outbound"

    get_privileges = ["page-all", "page-firewall-nat-outbound"]
    put_privileges = ["page-all", "page-firewall-nat-outbound"]

    get_tests = [
        {"name": "Read entire outbound NAT configuration"}
    ]
    put_tests = [
        {"name": "Set outbound NAT mode to hybrid", "req_data": {"mode": "hybrid"}},
        {"name": "Set outbound NAT mode to advanced", "req_data": {"mode": "advanced"}},
        {"name": "Set outbound NAT mode to disabled", "req_data": {"mode": "disabled"}},
        {"name": "Set outbound NAT mode to automatic", "req_data": {"mode": "automatic"}}
    ]


APIE2ETestFirewallNATOutbound()
