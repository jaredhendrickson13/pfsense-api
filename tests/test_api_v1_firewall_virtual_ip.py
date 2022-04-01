# Copyright 2022 Jared Hendrickson
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

import e2e_test_framework

class APIE2ETestFirewallVirtualIP(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/firewall/virtual_ip"
    get_tests = [{"name": "Read all virtual IPs"}]
    post_tests = [
        {
            "name": "Create CARP virtual IP",
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.239/32",
                "password": "testpass",
                "descr": "E2E Test",
                "vhid": 10
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Create Proxy ARP virtual IP",
            "payload": {
                "mode": "proxyarp",
                "interface": "wan",
                "subnet": "172.16.77.240/32",
                "descr": "E2E Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Create IP Alias virtual IP",
            "payload": {
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "172.16.77.241/32",
                "descr": "E2E Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check mode requirement",
            "status": 400,
            "return": 4019
        },
        {
            "name": "Check mode validation",
            "status": 400,
            "return": 4023,
            "payload": {
                "mode": "INVALID"
            }
        },
        {
            "name": "Check interface requirement",
            "status": 400,
            "return": 4020,
            "payload": {
                "mode": "ipalias"
            }
        },
        {
            "name": "Check interface validation",
            "status": 400,
            "return": 4024,
            "payload": {
                "mode": "ipalias",
                "interface": "INVALID"
            }
        },
        {
            "name": "Check subnet requirement",
            "status": 400,
            "return": 4021,
            "payload": {
                "mode": "ipalias",
                "interface": "wan"
            }
        },
        {
            "name": "Check subnet validation",
            "status": 400,
            "return": 4025,
            "payload": {
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "INVALID"
            }
        },
        {
            "name": "Check subnet unique constraint",
            "status": 400,
            "return": 4026,
            "payload": {
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "172.16.77.241/32"
            }
        },
        {
            "name": "Check CARP VHID minimum constraint",
            "status": 400,
            "return": 4028,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "vhid": -1
            }
        },
        {
            "name": "Check CARP VHID maximum constraint",
            "status": 400,
            "return": 4028,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "vhid": 4000000
            }
        },
        {
            "name": "Check CARP VHID unique constraint duplicate VHID on same interface",
            "status": 400,
            "return": 4027,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "vhid": 10
            }
        },
        {
            "name": "Check CARP password requirement & VHID unique constraint duplicate VHID on different interface",
            "status": 400,
            "return": 4022,
            "payload": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "192.168.1.252/32",
                "vhid": 10
            }
        },
        {
            "name": "Check CARP advertisement skew minimum constraint",
            "status": 400,
            "return": 4030,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advskew": -1
            }
        },
        {
            "name": "Check CARP advertisement skew maximum constraint",
            "status": 400,
            "return": 4030,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advskew": 4030
            }
        },
        {
            "name": "Check CARP advertisement base minimum constraint",
            "status": 400,
            "return": 4029,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advbase": 0
            }
        },
        {
            "name": "Check CARP advertisement base maximum constraint",
            "status": 400,
            "return": 4029,
            "payload": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advbase": 4030
            }
        },
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
                "descr": "Updated E2E test",
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
                "descr": "Updated E2E test",
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
                "descr": "Updated E2E test",
            },
            "resp_time": 10  # Allow up to ten seconds for vips
        },
        {
            "name": "Check subnet validation",
            "status": 400,
            "return": 4025,
            "payload": {
                "id": 2,
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "INVALID"
            }
        },
        {
            "name": "Check subnet unique constraint",
            "status": 400,
            "return": 4026,
            "payload": {
                "id": 2,
                "mode": "ipalias",
                "interface": "wan",
                "subnet": "172.16.77.230/32",
            }
        },
        {
            "name": "Check CARP VHID minimum constraint",
            "status": 400,
            "return": 4028,
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "vhid": -1
            }
        },
        {
            "name": "Check CARP VHID maximum constraint",
            "status": 400,
            "return": 4028,
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "vhid": 4000000
            }
        },
        {
            "name": "Check CARP advertisement skew minimum constraint",
            "status": 400,
            "return": 4030,
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advskew": -1
            }
        },
        {
            "name": "Check CARP advertisement skew maximum constraint",
            "status": 400,
            "return": 4030,
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advskew": 4030
            }
        },
        {
            "name": "Check CARP advertisement base minimum constraint",
            "status": 400,
            "return": 4029,
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advbase": 0
            }
        },
        {
            "name": "Check CARP advertisement base maximum constraint",
            "status": 400,
            "return": 4029,
            "payload": {
                "id": 0,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.252/32",
                "advbase": 4030
            }
        },
        {
            "name": "Check CARP password requirement when updating from non-CARP virtual IP",
            "status": 400,
            "return": 4022,
            "payload": {
                "id": 2,
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.234/32",
            }
        },
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
        }
    ]

APIE2ETestFirewallVirtualIP()
