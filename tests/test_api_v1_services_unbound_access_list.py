# Copyright 2023 Jared Hendrickson
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
"""Script used to test the /api/v1/services/unbound/access_list endpoint."""
import e2e_test_framework


class APIE2ETestServicesUnboundAccessList(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/unbound/access_list endpoint."""
    uri = "/api/v1/services/unbound/access_list"

    get_privileges = ["page-all", "page-services-dnsresolver-acls"]
    post_privileges = ["page-all", "page-services-dnsresolver-acls"]
    put_privileges = ["page-all", "page-services-dnsresolver-acls"]
    delete_privileges = ["page-all", "page-services-dnsresolver-acls"]

    get_tests = [{"name": "Read Unbound access lists"}]
    post_tests = [
        {
            "name": "Create Unbound allow access list",
            "resp_time": 10,
            "req_data": {
                "aclname": "Test Allow ACL",
                "aclaction": "allow",
                "descr": "Unbound allow ACL E2E test",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": 0,
                        "description": "Unbound access list row E2E test"
                    }
                ],
                "apply": True
            }
        },
        {
            "name": "Create Unbound deny access list",
            "req_data": {
                "aclname": "Test Deny ACL",
                "aclaction": "deny",
                "descr": "Unbound deny ACL E2E test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row E2E test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound refuse access list",
            "req_data": {
                "aclname": "Test Refuse ACL",
                "aclaction": "refuse",
                "descr": "Unbound refuse ACL E2E test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row E2E test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound allow snoop access list",
            "req_data": {
                "aclname": "Test Allow Snoop ACL",
                "aclaction": "allow snoop",
                "descr": "Unbound allow snoop ACL E2E test",
                "row": [
                    {
                        "acl_network": "0::",
                        "mask": 128,
                        "description": "Unbound access list row E2E test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound refuse non local access list",
            "req_data": {
                "aclname": "Test Refuse Nonlocal ACL",
                "aclaction": "refuse nonlocal",
                "descr": "Unbound refuse nonlocal ACL E2E test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row E2E test"
                    }
                ]
            }
        },
        {
            "name": "Create Unbound deny nonlocal access list",
            "req_data": {
                "aclname": "Test Deny Nonlocal ACL",
                "aclaction": "deny nonlocal",
                "descr": "Unbound deny nonlocal ACL E2E test",
                "row": [
                    {
                        "acl_network": "8.8.8.8",
                        "mask": 32,
                        "description": "Unbound access list row E2E test"
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
            "req_data": {
                "aclaction": "invalid choice"
            }
        },
        {
            "name": "Check access list row requirement",
            "status": 400,
            "return": 2073,
            "req_data": {
                "aclaction": "allow"
            }
        },
        {
            "name": "Check access list row minimum constraint",
            "status": 400,
            "return": 2073,
            "req_data": {
                "aclaction": "allow",
                "row": []
            }
        },
        {
            "name": "Check access list row network requirement",
            "status": 400,
            "return": 2068,
            "req_data": {
                "aclaction": "allow",
                "row": [{}]
            }
        },
        {
            "name": "Check access list row network IP validation",
            "status": 400,
            "return": 2069,
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
                "id": 0,
                "aclname": "Updated Test Deny ACL",
                "aclaction": "deny",
                "descr": "Updated unbound deny ACL E2E test",
                "row": [
                    {
                        "acl_network": "1.1.1.1",
                        "mask": 32,
                        "description": "Updated unbound access list row E2E test"
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
            "req_data": {
                "id": "invalid"
            }
        },
        {
            "name": "Check access list action choice constraint",
            "status": 400,
            "return": 2067,
            "req_data": {
                "id": 0,
                "aclaction": "invalid choice"
            }
        },
        {
            "name": "Check access list row minimum constraint",
            "status": 400,
            "return": 2073,
            "req_data": {
                "id": 0,
                "aclaction": "allow",
                "row": []
            }
        },
        {
            "name": "Check access list row network requirement",
            "status": 400,
            "return": 2068,
            "req_data": {
                "id": 0,
                "aclaction": "allow",
                "row": [{}]
            }
        },
        {
            "name": "Check access list row network IP validation",
            "status": 400,
            "return": 2069,
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
                "id": "invalid"
            }
        },
        {
            "name": "Delete Unbound access list",
            "req_data": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "req_data": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "req_data": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "req_data": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "req_data": {
                "id": 0
            }
        },
        {
            "name": "Delete Unbound access list",
            "resp_time": 8,
            "req_data": {
                "id": 0,
                "apply": True
            }
        }
    ]


APIE2ETestServicesUnboundAccessList()
