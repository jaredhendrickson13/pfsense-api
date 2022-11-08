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
"""Script used to test the /api/v1/system/api endpoint."""
import e2e_test_framework


class APIE2ETestSystemAPI(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/api endpoint."""
    uri = "/api/v1/system/api"

    get_privileges = ["page-all", "page-system-api"]
    put_privileges = ["page-all", "page-system-api"]

    get_tests = [{"name": "Read API configuration"}]
    put_tests = [
        {
            "name": "Update API configuration",
            "req_data": {
                "persist": False,
                "jwt_exp": 86400,
                "keyhash": "sha512",
                "keybytes": 64,
                "allowed_interfaces": ["WAN"],
                "access_list": ["0::/0", "0.0.0.0/0"]
            }
        },
        {
            "name": "Revert API configuration to default",
            "req_data": {
                "persist": True,
                "jwt_exp": 3600,
                "keyhash": "sha256",
                "keybytes": 16,
                "allowed_interfaces": ["any"],
                "access_list": []
            },
        },
        {
            "name": "Test JWT expiration maximum threshold",
            "status": 400,
            "return": 1022,
            "req_data": {
                "jwt_exp": 86401
            }
        },
        {
            "name": "Test JWT expiration minimum threshold",
            "status": 400,
            "return": 1022,
            "req_data": {
                "jwt_exp": 299
            }
        },
        {
            "name": "Test invalid hash algorithm",
            "status": 400,
            "return": 1023,
            "req_data": {
                "keyhash": "INVALID"
            }
        },
        {
            "name": "Test invalid key bytes",
            "status": 400,
            "return": 1024,
            "req_data": {
                "keybytes": "INVALID"
            }
        },
        {
            "name": "Test invalid interface",
            "status": 400,
            "return": 1020,
            "req_data": {
                "allowed_interfaces": ["INVALID"]
            }
        },
        {
            "name": "Test invalid access list",
            "status": 400,
            "return": 1072,
            "req_data": {
                "access_list": ["INVALID"]
            }
        },
        {
            "name": "Test custom_headers type constraint",
            "status": 400,
            "return": 1025,
            "req_data": {"custom_headers": "INVALID"}
        },
        {
            "name": "Test custom_headers key-value type constraints",
            "status": 400,
            "return": 1026,
            "req_data": {"custom_headers": {0: True}}
        },
        {
            "name": "Test hasync_hosts minimum constraint",
            "status": 400,
            "return": 1027,
            "req_data": {"hasync": True, "hasync_hosts": []}
        },
        {
            "name": "Test hasync_hosts IP/FQDN constraint",
            "status": 400,
            "return": 1028,
            "req_data": {"hasync": True, "hasync_hosts": [True]}
        },
        {
            "name": "Test hasync_username required constraint",
            "status": 400,
            "return": 1029,
            "req_data": {"hasync": True, "hasync_hosts": ["127.0.0.1"]}
        },
        {
            "name": "Test hasync_username required",
            "status": 400,
            "return": 1029,
            "req_data": {"hasync": True, "hasync_hosts": ["127.0.0.1"], "hasync_username": "test"}
        },
        {
            "name": "Test authmode options constraint",
            "status": 400,
            "return": 1021,
            "req_data": {"authmode": "INVALID"}
        }
    ]


APIE2ETestSystemAPI()
