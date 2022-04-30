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

class APIE2ETestServicesDHCPdStaticMapping(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/services/dhcpd/static_mapping"
    get_tests = [
        {
            "name": "Read all DHCPd static mappings on the LAN interface",
            "payload": {
                "interface": "lan"
            }
        }
    ]
    post_tests = [
        {
            "name": "Create DHCPd static mapping on the LAN interface",
            "payload": {
                "interface": "lan",
                "mac": "ac:de:48:00:11:22",
                "ipaddr": "192.168.1.254",
                "cid": "a098b-zpe9s-1vr45",
                "descr": "This is a DHCP static mapping created by pfSense API",
                "hostname": "test-host",
                "domain": "example.com",
                "dnsserver": ["1.1.1.1"],
                "domainsearchlist": ["example.com"],
                "gateway": "192.168.1.1",
                "arp_table_static_entry": True
            },
            "resp_time": 12    # Allow a few seconds to reload the DHCP service
        }
    ]
    put_tests = [
        {
            "name": "Update DHCPd static mapping on the LAN interface",
            "payload": {
                "id": 0,
                "interface": "lan",
                "mac": "ac:de:48:00:11:22",
                "ipaddr": "192.168.1.250",
                "cid": "updated-a098b-zpe9s-1vr45",
                "descr": "This is an updated DHCP static mapping created by pfSense API",
                "hostname": "updated-test-host",
                "domain": "updated.example.com",
                "dnsserver": ["8.8.8.8", "8.8.4.4", "1.1.1.1"],
                "domainsearchlist": ["updated.example.com", "extra.example.com"],
                "gateway": "192.168.1.2",
                "arp_table_static_entry": True
            },
            "resp_time": 5    # Allow a few seconds to reload the DHCP service
        }
    ]
    delete_tests = [
        {
            "name": "Delete DHCPd static mapping on the LAN interface",
            "payload": {
                "interface": "lan",
                "id": 0
            },
            "resp_time": 5    # Allow a few seconds to reload the DHCP service
        }
    ]

APIE2ETestServicesDHCPdStaticMapping()
