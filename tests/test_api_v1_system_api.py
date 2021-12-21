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

import unit_test_framework


class APIUnitTestSystemAPI(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/api"
    get_tests = [{"name": "Read API configuration"}]
    put_tests = [
        {
            "name": "Update API configuration",
            "payload": {
                "persist": False,
                "jwt_exp": 86400,
                "keyhash": "sha512",
                "keybytes": 64,
                "allowed_interfaces": ["WAN"]
            },
        },
        {
            "name": "Revert API configuration to default",
            "payload": {
                "persist": True,
                "jwt_exp": 3600,
                "keyhash": "sha256",
                "keybytes": 16,
                "allowed_interfaces": ["any"]
            },
        },
        {
            "name": "Test JWT expiration maximum threshold",
            "status": 400,
            "return": 1022,
            "payload": {
                "jwt_exp": 86401
            }
        },
        {
            "name": "Test JWT expiration minimum threshold",
            "status": 400,
            "return": 1022,
            "payload": {
                "jwt_exp": 299
            }
        },
        {
            "name": "Test invalid hash algorithm",
            "status": 400,
            "return": 1023,
            "payload": {
                "keyhash": "INVALID"
            }
        },
        {
            "name": "Test invalid key bytes",
            "status": 400,
            "return": 1024,
            "payload": {
                "keybytes": "INVALID"
            }
        },
        {
            "name": "Test invalid interface",
            "status": 400,
            "return": 1020,
            "payload": {
                "allowed_interfaces": ["INVALID"]
            }
        },

    ]


APIUnitTestSystemAPI()
