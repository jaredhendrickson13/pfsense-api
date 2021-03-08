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

class APIUnitTestFirewallAlias(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/alias"
    get_tests = [
        {"name": "Read all aliases"}
    ]
    post_tests = [
        {
            "name": "Create network alias",
            "payload": {
                "name": "RFC1918",
                "type": "network",
                "descr": "Unit Test",
                "address": ["10.0.0.0/8", "172.16.0.0/12", "192.168.0.0/24"],
                "detail": ["Class A", "Class B", "Class C"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Create port alias",
            "payload": {
                "name": "HTTP_PORTS",
                "type": "port",
                "descr": "Unit Test",
                "address": [80, 443, 8443],
                "detail": ["HTTP", "HTTPS", "HTTPS-ALT"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Create host alias",
            "payload": {
                "name": "DNS_SERVERS",
                "type": "host",
                "descr": "Unit Test",
                "address": ["1.1.1.1", "8.8.8.8", "8.8.4.4"],
                "detail": ["Cloudflare DNS", "Google DNS", "Secondary Google DNS"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        }
    ]
    put_tests = [
        {
            "name": "Update network alias",
            "payload": {
                "id": "RFC1918",
                "name": "UPDATED_RFC1918",
                "type": "network",
                "descr": "Updated Unit Test",
                "address": ["10.0.0.0/32", "172.16.0.0/32", "192.168.0.0/32"],
                "detail": ["New Class A", "New Class B", "New Class C"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Update port alias",
            "payload": {
                "id": "HTTP_PORTS",
                "name": "UPDATED_HTTP_PORTS",
                "type": "port",
                "descr": "Updated Unit Test",
                "address": [8080, 4433, 443],
                "detail": ["HTTP-ALT", "HTTPS-ALT", "HTTPS"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Update host alias",
            "payload": {
                "id": "DNS_SERVERS",
                "name": "UPDATED_DNS_SERVERS",
                "type": "host",
                "descr": "Updated Unit Test",
                "address": ["8.8.8.8"],
                "detail": ["Google DNS"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        }
    ]
    delete_tests = [
        {
            "name": "Delete network alias",
            "payload": {
                "id": "UPDATED_RFC1918"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Delete port alias",
            "payload": {
                "id": "UPDATED_HTTP_PORTS"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Delete host alias",
            "payload": {
                "id": "UPDATED_DNS_SERVERS"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        }
    ]

APIUnitTestFirewallAlias()