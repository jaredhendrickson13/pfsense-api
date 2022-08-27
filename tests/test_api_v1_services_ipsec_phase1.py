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
"""Script used to test the /api/v1/services/ipsec/phase1 endpoint."""
import e2e_test_framework


class APIE2ETestServicesIPsecPhase1(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/ipsec/phase1 endpoint."""
    uri = "/api/v1/services/ipsec/phase1"
    get_tests = [{"name": "Read IPsec phase1 entries"}]
    post_tests = [
        {
            "name": "Create RSA internal CA for IPsec testing",
            "uri": "/api/v1/system/ca",
            "payload": {
                "method": "internal",
                "descr": "IPSEC_CA_RSA",
                "trust": True,
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "ipsec-ca-e2e-test.example.com",
                "dn_country": "US",
                "dn_city": "Salt Lake City",
                "dn_state": "Utah",
                "dn_organization": "Test Company",
                "dn_organizationalunit": "IT"
            },
        },
        {
            "name": "Create internal certificate for IPsec testing",
            "needs_caref": True,
            "uri": "/api/v1/system/certificate",
            "payload": {
                "method": "internal",
                "descr": "IPSEC_CERT_RSA",
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "internal-cert-e2e-test.example.com",
                "dn_country": "US",
                "dn_city": "Salt Lake City",
                "dn_state": "Utah",
                "dn_organization": "Test Company",
                "dn_organizationalunit": "IT",
                "type": "server"
            }
        },
        {
            "name": "Check creating valid IPsec phase 1 with PSK auth",
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
            "name": "Check creating valid IPsec phase 1 with cert auth",
            "needs_certref": True,
            "needs_caref": True,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.2",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256},
                        "hash-algorithm": "sha256",
                        "dhgroup": 14
                    }
                ],
                "apply": True
            }
        },
        {
            "name": "Check iketype required constraint",
            "status": 400,
            "return": 2159
        },
        {
            "name": "Check iketype options constraint",
            "status": 400,
            "return": 2160,
            "payload": {"iketype": "INVALID"}
        },
        {
            "name": "Check protocol required constraint",
            "status": 400,
            "return": 2161,
            "payload": {
                "iketype": "ikev2"
            }
        },
        {
            "name": "Check protocol options constraint",
            "status": 400,
            "return": 2162,
            "payload": {
                "iketype": "ikev2",
                "protocol": "INVALID"
            }
        },
        {
            "name": "Check interface required constraint",
            "status": 400,
            "return": 2163,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet"
            }
        },
        {
            "name": "Check interface options constraint",
            "status": 400,
            "return": 2164,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet6",
                "interface": "INVALID"
            }
        },
        {
            "name": "Check remote-gateway required constraint",
            "status": 400,
            "return": 2165,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet6",
                "interface": "wan"
            }
        },
        {
            "name": "Check remote-gateway IP or hostname constraint",
            "status": 400,
            "return": 2166,
            "payload": {
                "iketype": "ikev2",
                "protocol": "both",
                "interface": "wan",
                "remote-gateway": "INVALID"
            }
        },
        {
            "name": "Check remote-gateway IPv4 constraint when protocol is 'inet'",
            "status": 400,
            "return": 2167,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "::1"
            }
        },
        {
            "name": "Check remote-gateway IPv6 constraint when protocol is 'inet6'",
            "status": 400,
            "return": 2168,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet6",
                "interface": "wan",
                "remote-gateway": "127.0.0.1"
            }
        },
        {
            "name": "Check remote-gateway domain only when protocol is 'both' constraint (inet)",
            "status": 400,
            "return": 2169,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "example.com"
            }
        },
        {
            "name": "Check remote-gateway domain only when protocol is 'both' constraint (inet6)",
            "status": 400,
            "return": 2169,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet6",
                "interface": "wan",
                "remote-gateway": "example.com"
            }
        },
        {
            "name": "Check remote gateway unique constraint",
            "status": 400,
            "return": 2170,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.2"
            }
        },
        {
            "name": "Check authentication_method required constraint",
            "status": 400,
            "return": 2171,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1"
            }
        },
        {
            "name": "Check authentication_method options constraint",
            "status": 400,
            "return": 2172,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "INVALID"
            }
        },
        {
            "name": "Check myid_type required constraint",
            "status": 400,
            "return": 2173,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key"
            }
        },
        {
            "name": "Check myid_type options constraint",
            "status": 400,
            "return": 2174,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "INVALID"
            }
        },
        {
            "name": "Check myid_data required constraint when myid_type is not 'myaddress'",
            "status": 400,
            "return": 2175,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "address"
            }
        },
        {
            "name": "Check myid_data IP address constraint when myid_type is 'address'",
            "status": 400,
            "return": 2176,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "address",
                "myid_data": "INVALID"
            }
        },
        {
            "name": "Check myid_data FQDN constraint when myid_type is 'fqdn'",
            "status": 400,
            "return": 2177,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "fqdn",
                "myid_data": "!INVALID!"
            }
        },
        {
            "name": "Check myid_data email constraint when myid_type is 'user_fqdn'",
            "status": 400,
            "return": 2178,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "user_fqdn",
                "myid_data": "INVALID"
            }
        },
        {
            "name": "Check myid_data FQDN constraint when myid_type is 'dyn_dns'",
            "status": 400,
            "return": 2179,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "dyn_dns",
                "myid_data": "!INVALID!"
            }
        },
        {
            "name": "Check peerid_type required constraint",
            "status": 400,
            "return": 2180,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress"
            }
        },
        {
            "name": "Check peerid_type options constraint",
            "status": 400,
            "return": 2181,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "INVALID"

            }
        },
        {
            "name": "Check myid_data required constraint when myid_type is not 'myaddress'",
            "status": 400,
            "return": 2182,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "address"

            }
        },
        {
            "name": "Check peerid_data IP address constraint when peerid_type is 'address'",
            "status": 400,
            "return": 2183,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "address",
                "peerid_data": "INVALID"
            }
        },
        {
            "name": "Check peerid_data FQDN constraint when peerid_type is 'fqdn'",
            "status": 400,
            "return": 2184,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "fqdn",
                "peerid_data": "!INVALID!"
            }
        },
        {
            "name": "Check peerid_data email constraint when peerid_type is 'user_fqdn'",
            "status": 400,
            "return": 2185,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "user_fqdn",
                "peerid_data": "INVALID"
            }
        },
        {
            "name": "Check pre-shared-key required constraint when authentication_method is 'pre_shared_key'",
            "status": 400,
            "return": 2187,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress"
            }
        },
        {
            "name": "Check certificate required constraint when authentication_method is 'cert'",
            "status": 400,
            "return": 2188,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress"
            }
        },
        {
            "name": "Check certificate exists constraint when authentication_method is 'cert'",
            "status": 400,
            "return": 2189,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "certref": "INVALID"
            }
        },
        {
            "name": "Check CA required constraint when authentication_method is 'cert'",
            "status": 400,
            "return": 2190,
            "needs_certref": True,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "certref": "NEED VALID CERT HERE"
            }
        },
        {
            "name": "Check CA exists constraint when authentication_method is 'cert'",
            "status": 400,
            "return": 2191,
            "needs_certref": True,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "certref": "NEED VALID CERT HERE",
                "caref": "INVALID"
            }
        },
        {
            "name": "Check negotiation mode options constraint when iketype is 'ikev1' or 'auto'",
            "status": 400,
            "return": 2192,
            "payload": {
                "iketype": "ikev1",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "mode": "INVALID"
            }
        },
        {
            "name": "Check negotiation mode options constraint when iketype is 'ikev1' or 'auto'",
            "status": 400,
            "return": 2192,
            "payload": {
                "iketype": "auto",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "mode": "INVALID"
            }
        },
        {
            "name": "Check re-key time numeric constraint",
            "status": 400,
            "return": 2193,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "rekey_time": "INVALID"
            }
        },
        {
            "name": "Check re-key time minimum constraint",
            "status": 400,
            "return": 2193,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "rekey_time": -1
            }
        },
        {
            "name": "Check re-auth time numeric constraint",
            "status": 400,
            "return": 2194,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "reauth_time": "INVALID"
            }
        },
        {
            "name": "Check re-auth time minimum constraint",
            "status": 400,
            "return": 2194,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "reauth_time": -1
            }
        },
        {
            "name": "Check rand time numeric constraint",
            "status": 400,
            "return": 2195,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "rand_time": "INVALID"
            }
        },
        {
            "name": "Check rand time minimum constraint",
            "status": 400,
            "return": 2195,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "rand_time": -1
            }
        },
        {
            "name": "Check lifetime numeric constraint",
            "status": 400,
            "return": 2196,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "lifetime": "INVALID"
            }
        },
        {
            "name": "Check lifetime greater than rekey_time constraint",
            "status": 400,
            "return": 2197,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "rekey_time": 100,
                "lifetime": 99
            }
        },
        {
            "name": "Check lifetime greater than reauth_time constraint",
            "status": 400,
            "return": 2197,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "reauth_time": 100,
                "lifetime": 99
            }
        },
        {
            "name": "Check start action options constraint",
            "status": 400,
            "return": 2198,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "startaction": "INVALID"
            }
        },
        {
            "name": "Check start action must be 'none' with any IP constraint",
            "status": 400,
            "return": 2199,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "0.0.0.0",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "startaction": "start"
            }
        },
        {
            "name": "Check close action options constraint",
            "status": 400,
            "return": 2200,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "closeaction": "INVALID"
            }
        },
        {
            "name": "Check NAT traversal options constraint",
            "status": 400,
            "return": 2201,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "INVALID"
            }
        },
        {
            "name": "Check IKE port minimum constraint",
            "status": 400,
            "return": 2202,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "ikeport": -1
            }
        },
        {
            "name": "Check IKE port maximum constraint",
            "status": 400,
            "return": 2202,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "ikeport": 65536
            }
        },
        {
            "name": "Check IKE port numeric constraint",
            "status": 400,
            "return": 2202,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "ikeport": "INVALID"
            }
        },
        {
            "name": "Check NAT-T port minimum constraint",
            "status": 400,
            "return": 2203,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "nattport": -1
            }
        },
        {
            "name": "Check NAT-T port maximum constraint",
            "status": 400,
            "return": 2203,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "nattport": 65536
            }
        },
        {
            "name": "Check NAT-T port numeric constraint",
            "status": 400,
            "return": 2203,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "nattport": "INVALID"
            }
        },
        {
            "name": "Check IKE port cannot match NAT-T port constraint",
            "status": 400,
            "return": 2204,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "nat_traversal": "force",
                "ikeport": 500,
                "nattport": 500
            }
        },
        {
            "name": "Check dead peer detection delay numeric constraint",
            "status": 400,
            "return": 2205,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "dpd_enable": True,
                "dpd_delay": "INVALID"
            }
        },
        {
            "name": "Check dead peer detection delay minimum constraint",
            "status": 400,
            "return": 2205,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "dpd_enable": True,
                "dpd_delay": -1
            }
        },
        {
            "name": "Check dead peer detection max-fails numeric constraint",
            "status": 400,
            "return": 2206,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "dpd_enable": True,
                "dpd_maxfail": "INVALID"
            }
        },
        {
            "name": "Check dead peer detection max-fails minimum constraint",
            "status": 400,
            "return": 2206,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "dpd_enable": True,
                "dpd_maxfail": -1
            }
        },
        {
            "name": "Check encryption required constraint",
            "status": 400,
            "return": 2217,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example"
            }
        },
        {
            "name": "Check encryption array constraint",
            "status": 400,
            "return": 2218,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": "INVALID"
            }
        },
        {
            "name": "Check encryption minimum constraint",
            "status": 400,
            "return": 2218,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": []
            }
        },
        {
            "name": "Check encryption algorithm required constraint",
            "status": 400,
            "return": 2209,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": [{}]
            }
        },
        {
            "name": "Check encryption algorithm options constraint",
            "status": 400,
            "return": 2210,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "INVALID", "keylen": "INVALID"}
                    }
                ]
            }
        },
        {
            "name": "Check hashing algorithm required constraint",
            "status": 400,
            "return": 2211,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256}
                    }
                ]
            }
        },
        {
            "name": "Check hashing algorithm options constraint",
            "status": 400,
            "return": 2212,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256},
                        "hash-algorithm": "INVALID"
                    }
                ]
            }
        },
        {
            "name": "Check DH group required constraint",
            "status": 400,
            "return": 2213,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256},
                        "hash-algorithm": "sha256"
                    }
                ]
            }
        },
        {
            "name": "Check DH group options constraint",
            "status": 400,
            "return": 2214,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256},
                        "hash-algorithm": "sha256",
                        "dhgroup": "INVALID"
                    }
                ]
            }
        },
        {
            "name": "Check PRF algorithm required constraint with 'prfselect_enable'",
            "status": 400,
            "return": 2215,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "prfselect_enable": True,
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
            "name": "Check PRF algorithm options constraint with 'prfselect_enable'",
            "status": 400,
            "return": 2216,
            "payload": {
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.1",
                "authentication_method": "pre_shared_key",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "pre-shared-key": "example",
                "prfselect_enable": True,
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 256},
                        "hash-algorithm": "sha256",
                        "dhgroup": 14,
                        "prf-algorithm": "INVALID"
                    }
                ]
            }
        }
    ]

    put_tests = [
        {
            "name": "Check updating valid IPsec phase 1 with PSK auth",
            "payload": {
                "ikeid": 1,
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.9",
                "authentication_method": "pre_shared_key",
                "pre-shared-key": "new_example",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "aes", "keylen": 128},
                        "hash-algorithm": "sha512",
                        "dhgroup": 14
                    }
                ]
            }
        },
        {
            "name": "Check updating valid IPsec phase 1 with cert auth",
            "needs_certref": True,
            "needs_caref": True,
            "payload": {
                "ikeid": 2,
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.8",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "3des"},
                        "hash-algorithm": "sha384",
                        "dhgroup": 15
                    }
                ],
                "apply": True
            }
        },
        {
            "name": "Check updating phase1 with no changes",
            "needs_certref": True,
            "needs_caref": True,
            "payload": {
                "ikeid": 2,
                "iketype": "ikev2",
                "protocol": "inet",
                "interface": "wan",
                "remote-gateway": "127.0.0.8",
                "authentication_method": "cert",
                "myid_type": "myaddress",
                "peerid_type": "peeraddress",
                "encryption": [
                    {
                        "encryption-algorithm": {"name": "3des"},
                        "hash-algorithm": "sha384",
                        "dhgroup": 15
                    }
                ],
                "apply": True
            }
        },
        {
            "name": "Check updating myid_type to 'address' requires 'myid_data' to be IP",
            "status": 400,
            "return": 2176,
            "payload": {
                "ikeid": 1,
                "myid_type": "address"
            }
        },
        {
            "name": "Check updating myid_type to 'fqdn' requires 'myid_data' to be FQDN",
            "status": 400,
            "return": 2177,
            "payload": {
                "ikeid": 1,
                "myid_type": "fqdn"
            }
        },
        {
            "name": "Check updating myid_type to 'user_fqdn' requires 'myid_data' to be email address",
            "status": 400,
            "return": 2178,
            "payload": {
                "ikeid": 1,
                "myid_type": "user_fqdn"
            }
        },
        {
            "name": "Check updating myid_type to 'dyn_dns' requires 'myid_data' to be hostname",
            "status": 400,
            "return": 2179,
            "payload": {
                "ikeid": 1,
                "myid_type": "dyn_dns"
            }
        },
        {
            "name": "Check updating peerid_type to 'address' requires 'peerid_data' to be IP",
            "status": 400,
            "return": 2183,
            "payload": {
                "ikeid": 1,
                "peerid_type": "address"
            }
        },
        {
            "name": "Check updating peerid_type to 'fqdn' requires 'peerid_data' to be FQDN",
            "status": 400,
            "return": 2184,
            "payload": {
                "ikeid": 1,
                "peerid_type": "fqdn"
            }
        },
        {
            "name": "Check updating peerid_type to 'user_fqdn' requires 'peerid_data' to be email address",
            "status": 400,
            "return": 2185,
            "payload": {
                "ikeid": 1,
                "peerid_type": "user_fqdn"
            }
        },
        {
            "name": "Check pre-shared-key required constraint when updating from 'cert' authentication_method",
            "status": 400,
            "return": 2187,
            "payload": {
                "ikeid": 2,
                "authentication_method": "pre_shared_key"
            }
        },
        {
            "name": "Check certificate required constraint when updating from 'pre_shared_key' authentication_method",
            "status": 400,
            "return": 2188,
            "payload": {
                "ikeid": 1,
                "authentication_method": "cert"
            }
        },
        {
            "name": "Check certificate exists constraint when updating from 'pre_shared_key' authentication_method",
            "status": 400,
            "return": 2189,
            "payload": {
                "ikeid": 1,
                "authentication_method": "cert",
                "certref": "INVALID"
            }
        },
        {
            "name": "Check CA required constraint when updating from 'pre_shared_key' authentication_method",
            "status": 400,
            "return": 2190,
            "needs_certref": True,
            "payload": {
                "ikeid": 1,
                "authentication_method": "cert"
            }
        },
        {
            "name": "Check CA exists constraint when updating from 'pre_shared_key' authentication_method",
            "status": 400,
            "return": 2191,
            "needs_certref": True,
            "payload": {
                "ikeid": 1,
                "authentication_method": "cert",
                "caref": "INVALID"
            }
        },
        {
            "name": "Check lifetime minimum constraint when updating to value below rekey_time or reauth_time",
            "status": 400,
            "return": 2197,
            "payload": {
                "ikeid": 1,
                "lifetime": 1000
            }
        },
        {
            "name": "Check startaction must be 'none' constraint when 'remote-gateway' is updated to 0.0.0.0",
            "status": 400,
            "return": 2199,
            "payload": {
                "ikeid": 1,
                "remote-gateway": "0.0.0.0"
            }
        },
        {
            "name": "Check `nattport` unique constraint when updating `ikeport`",
            "status": 400,
            "return": 2204,
            "payload": {
                "ikeid": 1,
                "ikeport": 4500
            }
        },
        {
            "name": "Check `ikeport` unique constraint when updating `nattport`",
            "status": 400,
            "return": 2204,
            "payload": {
                "ikeid": 1,
                "nattport": 500
            }
        },
        {
            "name": "Check `encryption` minimum length constraint when updating",
            "status": 400,
            "return": 2218,
            "payload": {
                "ikeid": 1,
                "encryption": []
            }
        }
    ]

    delete_tests = [
        {
            "name": "Check IKE ID required constraint",
            "status": 400,
            "return": 2207,
            "payload": {}
        },
        {
            "name": "Check IKE ID exists constraint",
            "status": 400,
            "return": 2208,
            "payload": {"ikeid": "INVALID"}
        },
        {
            "name": "Delete ikeid 1 used for IPsec testing",
            "payload": {"ikeid": 1}
        },
        {
            "name": "Delete ikeid 2 used for IPsec testing",
            "resp_time": 10,    # Allow a few seconds for IPsec to restart
            "payload": {"ikeid": 2, "apply": True}
        },
        {
            "name": "Delete cert used for IPsec testing",
            "uri": "/api/v1/system/certificate",
            "payload": {"descr": "IPSEC_CERT_RSA"}
        },
        {
            "name": "Delete CA used for IPsec testing",
            "uri": "/api/v1/system/ca",
            "payload": {"descr": "IPSEC_CA_RSA"}
        }
    ]

    def post_post(self):
        # Check our first POST response for the created CA's refid
        if len(self.post_responses) == 1:
            # Loop through all tests and auto-add the caref ID to tests that do not have the needs_caref key set
            counter = 0
            for test in self.post_tests:
                if "needs_caref" in test:
                    self.post_tests[counter]["payload"]["caref"] = self.post_responses[0]["data"]["refid"]
                counter = counter + 1

            counter = 0
            for test in self.put_tests:
                if "needs_caref" in test:
                    self.put_tests[counter]["payload"]["caref"] = self.post_responses[0]["data"]["refid"]
                counter = counter + 1

        # Check our second POST response for the created certs's refid
        if len(self.post_responses) == 2:
            # Loop through all tests and auto-add the certref ID to tests that do not have the needs_certref key set
            counter = 0
            for test in self.post_tests:
                if "needs_certref" in test:
                    self.post_tests[counter]["payload"]["certref"] = self.post_responses[1]["data"]["refid"]
                counter = counter + 1

            counter = 0
            for test in self.put_tests:
                if "needs_certref" in test:
                    self.put_tests[counter]["payload"]["certref"] = self.post_responses[1]["data"]["refid"]
                counter = counter + 1


APIE2ETestServicesIPsecPhase1()
