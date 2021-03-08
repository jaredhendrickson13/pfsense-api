# Copyright 2021 Jared Hendrickson
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

import unit_test_framework

class APIUnitTestFirewallVirtualIP(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/virtual_ip"
    get_tests = [{"name": "Read all virtual IPs"}]
    post_tests = [
        {
            "name": "Create CARP virtual IP",
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.239/32",
                "password": "testpass",
                "descr": "Unit Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Create Proxy ARP virtual IP",
            "payload": {
                "mode": "proxyarp",
                "interface": "wan",
                "subnet": "172.16.77.240/32",
                "descr": "Unit Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Create IP Alias virtual IP",
            "payload": {
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "172.16.77.241/32",
                "descr": "Unit Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        }
    ]
    put_tests = [
        {
            "name": "Update CARP virtual IP with static VHID",
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.229/32",
                "password": "newtestpass",
                "vhid": 25,
                "descr": "Updated unit Test",
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Update Proxy ARP virtual IP",
            "payload": {
                "id": 1,
                "mode": "proxyarp",
                "interface": "wan",
                "subnet": "172.16.77.230/32",
                "descr": "Updated unit Test",
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Update IP Alias virtual IP",
            "payload": {
                "id": 2,
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "172.16.77.231/32",
                "descr": "Updated unit Test",
            },
            "resp_time": 10  # Allow up to ten seconds for vips
        }
    ]
    delete_tests = [
        {
            "name": "Delete CARP virtual IP",
            "payload": {"id": 0},
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Delete Proxy ARP virtual IP",
            "payload": {"id": 0},
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Delete IP Alias virtual IP",
            "payload": {"id": 0},
            "resp_time": 10  # Allow up to ten seconds for vips
        },
    ]

APIUnitTestFirewallVirtualIP()