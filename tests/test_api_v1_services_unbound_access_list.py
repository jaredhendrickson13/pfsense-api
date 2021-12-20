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


class APIUnitTestServicesUnboundAccessList(unit_test_framework.APIUnitTest):
    uri = "/api/v1/services/unbound/access_list"
    get_tests = [{"name": "Read Unbound access lists"}]
    post_tests = [
        {
            "name": "Create Unbound allow access list",
            "resp_time": 10,
            "payload": {
                "aclname": "Test Allow ACL",
                "aclaction": "allow",
                "descr": "Unbound allow ACL unit test",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": 0,
                        "description": "Unbound access list row unit test"
                    }
                ],
                "apply": True
            }
        },
        {
            "name": "Create Unbound deny access list",
            "payload": {
                "aclname": "Test Deny ACL",
                "aclaction": "deny",
                "descr": "Unbound deny ACL unit test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row unit test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound refuse access list",
            "payload": {
                "aclname": "Test Refuse ACL",
                "aclaction": "refuse",
                "descr": "Unbound refuse ACL unit test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row unit test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound allow snoop access list",
            "payload": {
                "aclname": "Test Allow Snoop ACL",
                "aclaction": "allow snoop",
                "descr": "Unbound allow snoop ACL unit test",
                "row": [
                    {
                        "acl_network": "0::",
                        "mask": 128,
                        "description": "Unbound access list row unit test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound refuse non local access list",
            "payload": {
                "aclname": "Test Refuse Nonlocal ACL",
                "aclaction": "refuse nonlocal",
                "descr": "Unbound refuse nonlocal ACL unit test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row unit test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound deny nonlocal access list",
            "payload": {
                "aclname": "Test Deny Nonlocal ACL",
                "aclaction": "deny nonlocal",
                "descr": "Unbound deny nonlocal ACL unit test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row unit test"
                    }
                ]
            }
        },
        {
            "name": "Check access list action requirement",
            "status": 400,
            "return": 2066
        },
        {
            "name": "Check access list action choice constraint",
            "status": 400,
            "return": 2067,
            "payload": {
                "aclaction": "invalid choice"
            }
        },
        {
            "name": "Check access list row requirement",
            "status": 400,
            "return": 2073,
            "payload": {
                "aclaction": "allow"
            }
        },
        {
            "name": "Check access list row minimum constraint",
            "status": 400,
            "return": 2073,
            "payload": {
                "aclaction": "allow",
                "row": []
            }
        },
        {
            "name": "Check access list row network requirement",
            "status": 400,
            "return": 2068,
            "payload": {
                "aclaction": "allow",
                "row": [{}]
            }
        },
        {
            "name": "Check access list row network IP validation",
            "status": 400,
            "return": 2069,
            "payload": {
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "invalid ip"
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask requirement",
            "status": 400,
            "return": 2071,
            "payload": {
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0"
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv4 minimum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": -1
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv6 minimum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0::",
                        "mask": -1
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv4 maximum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": 33
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv6 maximum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0::",
                        "mask": 129
                    }
                ]
            }
        }
    ]
    put_tests = [
        {
            "name": "Update Unbound access list",
            "payload": {
                "id": 0,
                "aclname": "Updated Test Deny ACL",
                "aclaction": "deny",
                "descr": "Updated unbound deny ACL unit test",
                "row": [
                    {
                        "acl_network": "1.1.1.1",
                        "mask": 32,
                        "description": "Updated unbound access list row unit test"
                    }
                ]
            }
        },
        {
            "name": "Check access list ID requirement",
            "status": 400,
            "return": 2072
        },
        {
            "name": "Check access list ID exists constraint",
            "status": 400,
            "return": 2074,
            "payload": {
                "id": "invalid"
            }
        },
        {
            "name": "Check access list action choice constraint",
            "status": 400,
            "return": 2067,
            "payload": {
                "id": 0,
                "aclaction": "invalid choice"
            }
        },
        {
            "name": "Check access list row minimum constraint",
            "status": 400,
            "return": 2073,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": []
            }
        },
        {
            "name": "Check access list row network requirement",
            "status": 400,
            "return": 2068,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [{}]
            }
        },
        {
            "name": "Check access list row network IP validation",
            "status": 400,
            "return": 2069,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "invalid ip"
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask requirement",
            "status": 400,
            "return": 2071,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0"
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv4 minimum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": -1
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv6 minimum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0::",
                        "mask": -1
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv4 maximum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": 33
                    }
                ]
            }
        },
        {
            "name": "Check access list row network mask IPv6 maximum constraint",
            "status": 400,
            "return": 2070,
            "payload": {
                "id": 0,
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0::",
                        "mask": 129
                    }
                ]
            }
        },
    ]
    delete_tests = [
        {
            "name": "Check access list ID requirement",
            "status": 400,
            "return": 2072
        },
        {
            "name": "Check access list ID exists constraint",
            "status": 400,
            "return": 2074,
            "payload": {
                "id": "invalid"
            }
        },
        {
            "name": "Delete Unbound access list",
            "payload": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "payload": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "payload": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "payload": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "payload": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "resp_time": 8,
            "payload": {
                "id": 0,
                "apply": True
            }
        }
    ]


APIUnitTestServicesUnboundAccessList()
