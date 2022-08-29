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
"""Script used to test the /api/v1/services/ipsec/phase2 endpoint."""
import e2e_test_framework


class APIE2ETestServicesIPsecPhase2(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/ipsec/phase2 endpoint."""
    uri = "/api/v1/services/ipsec/phase2"
    get_tests = [{"name": "Read IPsec phase2 entries"}]
    post_tests = [
        {
            "name": "Create IPsec phase 1 for testing",
            "uri": "/api/v1/services/ipsec/phase1",
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.3",
                "authentication_method": "pre_shared_key",
                "pre-shared-key": "example",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256},
                        "hash-algorithm": "sha256",
                        "dhgroup": 14
                    }
                ]
            }
        },
        {
            "name": "Create valid IPsec phase 2",
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14
            }
        },
        {
            "name": "Check ikeid required constraint",
            "status": 400,
            "return": 2207
        },
        {
            "name": "Check ikeid exists constraint",
            "status": 400,
            "return": 2208,
            "payload": {"ikeid": "INVALID"}
        },
        {
            "name": "Check mode required constraint",
            "status": 400,
            "return": 2219,
            "payload": {"ikeid": 1}
        },
        {
            "name": "Check mode options constraint",
            "status": 400,
            "return": 2220,
            "payload": {"ikeid": 1, "mode": "INVALID"}
        },
        {
            "name": "Check localid required constraint",
            "status": 400,
            "return": 2223,
            "payload": {"ikeid": 1, "mode": "tunnel"}
        },
        {
            "name": "Check localid type options constraint",
            "status": 400,
            "return": 2225,
            "payload": {"ikeid": 1, "mode": "tunnel", "localid": {"type": "INVALID"}}
        },
        {
            "name": "Check localid address required constraint",
            "status": 400,
            "return": 2226,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address"}
            }
        },
        {
            "name": "Check localid address IP constraint when 'mode' is 'vti'",
            "status": 400,
            "return": 2227,
            "payload": {
                "ikeid": 1,
                "mode": "vti",
                "localid": {"type": "address", "address": "INVALID"}
            }
        },
        {
            "name": "Check localid address IPv4 constraint when 'mode' is 'tunnel'",
            "status": 400,
            "return": 2228,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address", "address": "::1"}
            }
        },
        {
            "name": "Check localid address IPv6 constraint when 'mode' is 'tunnel6'",
            "status": 400,
            "return": 2229,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "address", "address": "127.0.0.1"}
            }
        },
        {
            "name": "Check localid netbits required constraint when 'type' is 'network'",
            "status": 400,
            "return": 2230,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1"}
            }
        },
        {
            "name": "Check localid IPv4 netbits minimum constraint when 'type' is 'network'",
            "status": 400,
            "return": 2231,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": -1}
            }
        },
        {
            "name": "Check localid IPv4 netbits maxmimum constraint when 'type' is 'network'",
            "status": 400,
            "return": 2231,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": 33}
            }
        },
        {
            "name": "Check localid IPv6 netbits minimum constraint when 'type' is 'network'",
            "status": 400,
            "return": 2232,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "network", "address": "::1", "netbits": -1}
            }
        },
        {
            "name": "Check localid IPv6 netbits maxmimum constraint when 'type' is 'network'",
            "status": 400,
            "return": 2232,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "network", "address": "::1", "netbits": 129}
            }
        },
        {
            "name": "Check natlocalid type required constraint",
            "status": 400,
            "return": 2233,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "lan"},
                "natlocalid": {}
            }
        },
        {
            "name": "Check natlocalid type must match localid type constraint",
            "status": 400,
            "return": 2235,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address", "address": "127.0.0.1"},
                "natlocalid": {"type": "network"}
            }
        },
        {
            "name": "Check natlocalid address required constraint",
            "status": 400,
            "return": 2236,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "lan"},
                "natlocalid": {"type": "address"}
            }
        },
        {
            "name": "Check natlocalid address IPv4 constraint when 'mode' is set to 'tunnel'",
            "status": 400,
            "return": 2237,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "lan"},
                "natlocalid": {"type": "address", "address": "::1"}
            }
        },
        {
            "name": "Check natlocalid address IPv6 constraint when 'mode' is set to 'tunnel6'",
            "status": 400,
            "return": 2238,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "lan"},
                "natlocalid": {"type": "address", "address": "127.0.0.1"}
            }
        },
        {
            "name": "Check natlocalid netbits required constraint when 'type' is 'network'",
            "status": 400,
            "return": 2239,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "lan"},
                "natlocalid": {"type": "network", "address": "127.0.0.1"}
            }
        },
        {
            "name": "Check natlocalid netbits matches localid netbits constraint",
            "status": 400,
            "return": 2240,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": 24},
                "natlocalid": {"type": "network", "address": "127.0.0.2", "netbits": 25}
            }
        },
        {
            "name": "Check remoteid required constraint",
            "status": 400,
            "return": 2256,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": 24}
            }
        },
        {
            "name": "Check remoteid type required constraint",
            "status": 400,
            "return": 2241,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address", "address": "127.0.0.1"},
                "remoteid": {}
            }
        },
        {
            "name": "Check remoteid type options constraint",
            "status": 400,
            "return": 2242,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address", "address": "127.0.0.1"},
                "remoteid": {"type": "INVALID"}
            }
        },
        {
            "name": "Check remoteid address required constraint",
            "status": 400,
            "return": 2243,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address", "address": "127.0.0.1"},
                "remoteid": {"type": "address"}
            }
        },
        {
            "name": "Check remoteid address IPv4 constraint when 'mode' is 'tunnel'",
            "status": 400,
            "return": 2244,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "address", "address": "127.0.0.1"},
                "remoteid": {"type": "address", "address": "::1"}
            }
        },
        {
            "name": "Check remoteid address IPv6 constraint when 'mode' is 'tunnel6'",
            "status": 400,
            "return": 2245,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "address", "address": "::1"},
                "remoteid": {"type": "address", "address": "127.0.0.2"}
            }
        },
        {
            "name": "Check remoteid netbits required constraint when 'type' is 'network'",
            "status": 400,
            "return": 2246,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.0.2"}
            }
        },
        {
            "name": "Check remoteid netbits minimum constraint when 'mode' is 'tunnel'",
            "status": 400,
            "return": 2247,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.0.2", "netbits": -1}
            }
        },
        {
            "name": "Check remoteid netbits maximum constraint when 'mode' is 'tunnel'",
            "status": 400,
            "return": 2247,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.0.1", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.0.2", "netbits": 33}
            }
        },
        {
            "name": "Check remoteid netbits minimum constraint when 'mode' is 'tunnel6'",
            "status": 400,
            "return": 2248,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "network", "address": "::1", "netbits": 24},
                "remoteid": {"type": "network", "address": "0::", "netbits": -1}
            }
        },
        {
            "name": "Check remoteid netbits maximum constraint when 'mode' is 'tunnel6'",
            "status": 400,
            "return": 2248,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel6",
                "localid": {"type": "network", "address": "::1", "netbits": 24},
                "remoteid": {"type": "network", "address": "0::", "netbits": 129}
            }
        },
        {
            "name": "Check protocol required constraint",
            "status": 400,
            "return": 2221,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24}
            }
        },
        {
            "name": "Check protocol options constraint",
            "status": 400,
            "return": 2222,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "INVALID"
            }
        },
        {
            "name": "Check encryption-algorithm-option required constraint when 'protocol' is 'esp'",
            "status": 400,
            "return": 2249,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp"
            }
        },
        {
            "name": "Check encryption-algorithm-option type constraint",
            "status": 400,
            "return": 2250,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": False
            }
        },
        {
            "name": "Check encryption-algorithm-option minimum length constraint",
            "status": 400,
            "return": 2250,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": []
            }
        },
        {
            "name": "Check encryption-algorithm-option options constraint",
            "status": 400,
            "return": 2210,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [False]
            }
        },
        {
            "name": "Check hash-algorithm-option required constraint",
            "status": 400,
            "return": 2251,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}]
            }
        },
        {
            "name": "Check hash-algorithm-option type constraint",
            "status": 400,
            "return": 2252,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": False
            }
        },
        {
            "name": "Check hash-algorithm-option minimum length constraint",
            "status": 400,
            "return": 2252,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": []
            }
        },
        {
            "name": "Check hash-algorithm-option options constraint",
            "status": 400,
            "return": 2253,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["INVALID"]
            }
        },
        {
            "name": "Check pfsgroup requirement constraint",
            "status": 400,
            "return": 2254,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"]
            }
        },
        {
            "name": "Check pfsgroup options constraint",
            "status": 400,
            "return": 2255,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": "INVALID"
            }
        },
        {
            "name": "Check rekey_time numeric constraint",
            "status": 400,
            "return": 2193,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "rekey_time": "INVALID"
            }
        },
        {
            "name": "Check rekey_time minimum constraint",
            "status": 400,
            "return": 2193,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "rekey_time": -1
            }
        },
        {
            "name": "Check rand_time numeric constraint",
            "status": 400,
            "return": 2195,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "rand_time": "INVALID"
            }
        },
        {
            "name": "Check rand_time minimum constraint",
            "status": 400,
            "return": 2195,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "rand_time": -1
            }
        },
        {
            "name": "Check lifetime numeric constraint",
            "status": 400,
            "return": 2196,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "lifetime": "INVALID"
            }
        },
        {
            "name": "Check lifetime minimum constraint",
            "status": 400,
            "return": 2196,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "lifetime": -1
            }
        },
        {
            "name": "Check lifetime greater than rekey_time constraint",
            "status": 400,
            "return": 2197,
            "payload": {
                "ikeid": 1,
                "mode": "tunnel",
                "localid": {"type": "network", "address": "127.0.1.0", "netbits": 24},
                "remoteid": {"type": "network", "address": "127.0.2.0", "netbits": 24},
                "protocol": "esp",
                "encryption-algorithm-option": [{"name": "aes", "keylen": "auto"}],
                "hash-algorithm-option": ["hmac_sha256"],
                "pfsgroup": 14,
                "rekey_time": 2,
                "lifetime": 1
            }
        }
    ]
    delete_tests = [
        {
            "name": "Check uniqueid required constraint",
            "status": 400,
            "return": 2257
        },
        {
            "name": "Check uniqid required constraint",
            "status": 400,
            "return": 2258,
            "payload": {
                "uniqid": "INVALID"
            }
        },
        {
            "name": "Delete phase 1 parent",
            "uri": "/api/v1/services/ipsec/phase1",
            "resp_time": 10,
            "payload": {
                "ikeid": 1,
                "apply": True
            }
        }
    ]


APIE2ETestServicesIPsecPhase2()
