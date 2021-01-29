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

class APIUnitTestServicesDHCPdStaticMapping(unit_test_framework.APIUnitTest):
    url = "/api/v1/services/dhcpd/static_mapping"
    get_payloads = [
        {
            "interface": "lan"
        }
    ]
    post_payloads = [
        {
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
        }
    ]
    put_payloads = [
        {
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
            "arp_table_static_entry": False
        }
    ]
    delete_payloads = [
        {
            "interface": "lan",
            "id": 0
        }
    ]

APIUnitTestServicesDHCPdStaticMapping()