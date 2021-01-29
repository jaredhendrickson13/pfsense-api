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
    get_payloads = [{}]
    post_payloads = [
        {
            "name": "RFC1918",
            "type": "network",
            "descr": "Unit Test",
            "address": ["10.0.0.0/8", "172.16.0.0/12", "192.168.0.0/24"],
            "detail": ["Class A", "Class B", "Class C"]
        },
        {
            "name": "HTTP_PORTS",
            "type": "port",
            "descr": "Unit Test",
            "address": [80, 443, 8443],
            "detail": ["HTTP", "HTTPS", "HTTPS-ALT"]
        },
        {
            "name": "DNS_SERVERS",
            "type": "host",
            "descr": "Unit Test",
            "address": ["1.1.1.1", "8.8.8.8", "8.8.4.4"],
            "detail": ["Cloudflare DNS", "Google DNS", "Secondary Google DNS"]
        }

    ]
    put_payloads = [
        {
            "id": "RFC1918",
            "name": "UPDATED_RFC1918",
            "type": "network",
            "descr": "Updated Unit Test",
            "address": ["10.0.0.0/32", "172.16.0.0/32", "192.168.0.0/32"],
            "detail": ["New Class A", "New Class B", "New Class C"]
        },
        {
            "id": "HTTP_PORTS",
            "name": "UPDATED_HTTP_PORTS",
            "type": "port",
            "descr": "Updated Unit Test",
            "address": [8080, 4433, 443],
            "detail": ["HTTP-ALT", "HTTPS-ALT", "HTTPS"]
        },
        {
            "id": "DNS_SERVERS",
            "name": "UPDATED_DNS_SERVERS",
            "type": "host",
            "descr": "Updated Unit Test",
            "address": ["8.8.8.8"],
            "detail": ["Google DNS"]
        }
    ]
    delete_payloads = [
        {"id": "UPDATED_RFC1918"},
        {"id": "UPDATED_HTTP_PORTS"},
        {"id": "UPDATED_DNS_SERVERS"}
    ]

APIUnitTestFirewallAlias()