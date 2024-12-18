#Copyright 2019-2025 Jared Hendrickson
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


class APIE2ETestFirewallNATOutboundMapping(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/nat/outbound/mapping endpoint."""
    uri = "/api/v1/firewall/nat/outbound/mapping"

    get_privileges = ["page-all", "page-firewall-nat-outbound"]
    post_privileges = ["page-all", "page-firewall-nat-outbound-edit"]
    put_privileges = ["page-all", "page-firewall-nat-outbound-edit"]
    delete_privileges = ["page-all", "page-firewall-nat-outbound-edit"]

    get_tests = [
        {"name": "Read all outbound NAT mappings"}
    ]
    post_tests = [
        {
            "name": "Create port-based outbound NAT mapping",
            "req_data": {
                "interface": "WAN",
                "protocol": "tcp",
                "src": "any",
                "srcport": 434,
                "dst": "1.1.1.1",
                "dstport": 443,
                "target": "192.168.1.123/24",
                "natport": 443,
                "poolopts": "round-robin",
                "descr": "E2E Test",
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Create non-port-based outbound NAT mapping",
            "req_data": {
                "interface": "WAN",
                "protocol": "any",
                "src": "127.0.0.1/24",
                "dst": "1.1.1.1/32",
                "target": "192.168.1.123/24",
                "poolopts": "round-robin",
                "descr": "E2E Test 2",
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Check interface required constraint",
            "status": 400,
            "return": 4086
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 4087,
            "req_data": {"interface": "INVALID"}
        },
        {
            "name": "Check protocol required constraint",
            "status": 400,
            "return": 4088,
            "req_data": {"interface": "wan"}
        },
        {
            "name": "Check protocol options constraint",
            "status": 400,
            "return": 4089,
            "req_data": {"interface": "wan", "protocol": "INVALID"}
        },
        {
            "name": "Check poolopts options constraint",
            "status": 400,
            "return": 4090,
            "req_data": {"interface": "wan", "protocol": "tcp", "poolopts": "INVALID"}
        },
        {
            "name": "Check source hash key begins with 0x constraint",
            "status": 400,
            "return": 4091,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "INVALID"
            }
        },
        {
            "name": "Check source hash key hex value constraint",
            "status": 400,
            "return": 4092,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0xINVALID"
            }
        },
        {
            "name": "Check source hash key maximum length constraint",
            "status": 400,
            "return": 4093,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf853555"
            }
        },
        {
            "name": "Check target required constraint",
            "status": 400,
            "return": 4094,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535"
            }
        },
        {
            "name": "Check target is IP, subnet or alias constraint",
            "status": 400,
            "return": 4095,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "INVALID"
            }
        },
        {
            "name": "Create alias to use in target test",
            "method": "POST",
            "uri": "/api/v1/firewall/alias",
            "req_data": {
                "name": "TEST_NAT_TARGET",
                "type": "host",
                "address": ["127.0.0.1", "127.0.0.2"],
                "apply": True
            }
        },
        {
            "name": "Check allow alias target only with round-robin poolopt constraint",
            "status": 400,
            "return": 4096,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET"
            }
        },
        {
            "name": "Check natport is port or range constraint",
            "status": 400,
            "return": 4097,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "INVALID"
            }
        },
        {
            "name": "Check src required constraint",
            "status": 400,
            "return": 4098,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": 62000
            }
        },
        {
            "name": "Check src is IP, subnet or any constraint",
            "status": 400,
            "return": 4099,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "INVALID"
            }
        },
        {
            "name": "Check dst required constraint",
            "status": 400,
            "return": 4100,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": 62000,
                "src": "any"
            }
        },
        {
            "name": "Check dst is IP, subnet or any constraint",
            "status": 400,
            "return": 4101,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "127.0.0.1",
                "dst": "INVALID"
            }
        },
        {
            "name": "Check srcport is port or port range constraint",
            "status": 400,
            "return": 4102,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "127.0.0.1",
                "dst": "127.0.0.2",
                "srcport": "INVALID"
            }
        },
        {
            "name": "Check dstport is port or port range constraint",
            "status": 400,
            "return": 4103,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "127.0.0.1",
                "dst": "127.0.0.2",
                "srcport": "any",
                "dstport": "INVALID"
            }
        },
    ]
    put_tests = [
        {
            "name": "Update port-based outbound NAT mapping to non-port-based",
            "req_data": {
                "id": 0,
                "interface": "WAN",
                "protocol": "any",
                "src": "any",
                "dst": "1.1.1.1",
                "target": "192.168.1.123/24",
                "poolopts": "round-robin",
                "descr": "Updated E2E Test",
                "nonat": True,
                "disabled": True,
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Update non-port-based outbound NAT mapping to port-based",
            "req_data": {
                "id": 1,
                "interface": "WAN",
                "protocol": "udp",
                "src": "any",
                "srcport": "433",
                "dst": "1.1.1.1",
                "dstport": "443",
                "target": "192.168.1.123/24",
                "staticnatport": True,
                "poolopts": "round-robin",
                "descr": "Updated E2E Test",
                "nonat": False,
                "disabled": True,
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 4104
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 4105,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 4087,
            "req_data": {"id": 0, "interface": "INVALID"}
        },
        {
            "name": "Check protocol options constraint",
            "status": 400,
            "return": 4089,
            "req_data": {"id": 0, "interface": "wan", "protocol": "INVALID"}
        },
        {
            "name": "Check poolopts options constraint",
            "status": 400,
            "return": 4090,
            "req_data": {"id": 0, "interface": "wan", "protocol": "tcp", "poolopts": "INVALID"}
        },
        {
            "name": "Check source hash key begins with 0x constraint",
            "status": 400,
            "return": 4091,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "INVALID"
            }
        },
        {
            "name": "Check source hash key hex value constraint",
            "status": 400,
            "return": 4092,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0xINVALID"
            }
        },
        {
            "name": "Check source hash key maximum length constraint",
            "status": 400,
            "return": 4093,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf853555"
            }
        },
        {
            "name": "Check target is IP, subnet or alias constraint",
            "status": 400,
            "return": 4095,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "INVALID"
            }
        },
        {
            "name": "Check allow alias target only with round-robin poolopt constraint",
            "status": 400,
            "return": 4096,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "source-hash",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET"
            }
        },
        {
            "name": "Check natport is port or range constraint",
            "status": 400,
            "return": 4097,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "INVALID"
            }
        },
        {
            "name": "Check src is IP, subnet or any constraint",
            "status": 400,
            "return": 4099,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "INVALID"
            }
        },
        {
            "name": "Check dst is IP, subnet or any constraint",
            "status": 400,
            "return": 4101,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "127.0.0.1",
                "dst": "INVALID"
            }
        },
        {
            "name": "Check srcport is port or port range constraint",
            "status": 400,
            "return": 4102,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "127.0.0.1",
                "dst": "127.0.0.2",
                "srcport": "INVALID"
            }
        },
        {
            "name": "Check dstport is port or port range constraint",
            "status": 400,
            "return": 4103,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "poolopts": "round-robin",
                "source_hash_key": "0x3d7f08a393ec707e50df636346bf8535",
                "target": "TEST_NAT_TARGET",
                "natport": "10000:10005",
                "src": "127.0.0.1",
                "dst": "127.0.0.2",
                "srcport": "any",
                "dstport": "INVALID"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 4104
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 4105,
            "req_data": {"id": "INVALID"}
        },
        {"name": "Delete non-port-based outbound NAT mapping", "req_data": {"id": 0}},
        {"name": "Delete port-based outbound NAT mapping", "req_data": {"id": 0}},
        {
            "name": "Delete alias used for testing",
            "method": "DELETE",
            "uri": "/api/v1/firewall/alias",
            "req_data": {"id": "TEST_NAT_TARGET"}
        }
    ]


APIE2ETestFirewallNATOutboundMapping()
