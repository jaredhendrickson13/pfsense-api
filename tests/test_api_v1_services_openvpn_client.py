"""Script used to test the /api/v1/services/openvpn/client endpoint."""
import e2e_test_framework


class APIE2ETestOpenVPNClient(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/openvpn/client endpoint."""
    uri = "/api/v1/services/openvpn/client"
    tls_key_text = "-----BEGIN OpenVPN Static key V1----- \
db8701afd882d746be67f084bae68470 \
54a99ef3b61864cfe1864c6c02584335 \
fe706df150250bf7e294b8c35911817c \
133c8d9b505573ebeb65259bc54c70ae \
88cc3163fd11a20f73d2c6fb3eea7cc2 \
fdaefcde510486adc0acbb9481b3aef4 \
930db91806469218b2a4f92e71787cf4 \
635f5b3f773fefc28d492738d2648673 \
8fa9b23d41e5999f58c3f0004a59ecda \
119162e598764d9c8f6b99e7054f3ea4 \
fdb4461913d2e3273a8a0db29332406a \
237bcfccca2315445ef809eaa933fa30 \
a9a7510d2d167033edfd3580a824f3e1 \
1af57da6eee89e6318ec29c67da8a19d \
7c9d74c7afac6ee0f813a0278a6261d7 \
a28e7bdbf1743527346bda359bc92fc9 \
-----END OpenVPN Static key V1----- \
"
    get_tests = [{"name": "Read all OpenVPN Client instances"}]
    post_tests = [
        {
            "name": "OpenVPN Server/Client No Certificate Authority found",
            "status": 400,
            "return": 2144,
            "req_data": {
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Create RSA internal CA",
            "uri": "/api/v1/system/ca",
            "no_caref": True,  # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "req_data": {
                "method": "internal",
                "descr": "INTERNAL_CA_TEST",
                "trust": True,
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "dn_commonname": "internal-ca-e2e-test.example.com"
            },
        },
        {
            "name": "OpenVPN Server/Client No Server Certificate found",
            "status": 400,
            "return": 2135,
            "req_data": {
                "server_addr": "openvpn.example.com"
            }
        },
        {
            "name": "Create internal certificate with RSA key",
            "uri": "/api/v1/system/certificate",
            "no_certref": True,  # Prevents overriden post_post() method from auto-adding created Certificate ref ID
            "req_data": {
                "method": "internal",
                "descr": "INTERNAL_CERT_RSA",
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
            "name": "Create OpenVPN Client",
            "status": 200,
            "return": 0,
            "req_data": {
                "protocol": "udp4",
                "description": "TEST_Create_OpenVPN_Client",
                "dev_mode": "tun",
                "topology": "net30",
                "server_addr": "openvpn.example.com",
                "server_port": 1194,
                "proxy_addr": "10.0.0.1",
                "proxy_port": 999,
                "proxy_user": "proxy",
                "proxy_passwd": "proxy",
                "proxy_authtype": "basic",
                "tls": "",
                "tls_type": "crypt",
                "tlsauth_keydir": "both-directions",
                "ncp_enable": True,
                "data_ciphers": "AES-128-GCM,CHACHA20-POLY1305",
                "data_ciphers_fallback": "AES-256-CBC",
                "digest": "SHA512",
                "tunnel_network": "1.2.3.4/24",
                "custom_options": 'push "route 10.0.0.0 255.255.255.0";',
                "auth_user": "test",
                "pass_auth": "pass",
                "auth_retry_none": True,
                "inactive_seconds": 3600,
            }
        },
        {
            "name": "Create Interface (ovpnc1)",
            "uri": "/api/v1/interface",
            "no_caref": True,  # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "no_certref": True,
            # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            # "resp_time": 30,
            "req_data": {
                "if": "ovpnc1",
                "descr": "TEST_INTERFACE_OVPNC1",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Mode",
            "status": 400,
            "return": 2104,
            "req_data": {
                "mode": "INVALID_MODE",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Protocol",
            "status": 400,
            "return": 2105,
            "req_data": {
                "protocol": "INVALID_PROTOCOL",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Device mode",
            "status": 400,
            "return": 2106,
            "req_data": {
                "dev_mode": "INVALID_DEV_MODE",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Interface",
            "status": 400,
            "return": 2107,
            "req_data": {
                "interface": "INVALID_INTERFACE",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client TLS Key Usage Mode",
            "status": 400,
            "return": 2109,
            "req_data": {
                "tls": tls_key_text,
                "tls_type": "INVALID_TLS_TYPE",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client TLS keydir direction",
            "status": 400,
            "return": 2110,
            "req_data": {
                "tls": tls_key_text,
                "tlsauth_keydir": "INVALID_TLS_KEYDIR",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Fallback Data Encryption Algorithm",
            "status": 400,
            "return": 2114,
            "req_data": {
                "data_ciphers_fallback": "INVALID_DATA_CIPHERS_FALLBACK",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Auth Digest Algorithm",
            "status": 400,
            "return": 2115,
            "req_data": {
                "digest": "INVALID_DIGEST",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Hardware Crypto Engine",
            "status": 400,
            "return": 2116,
            "req_data": {
                "engine": "INVALID_ENGINE",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown or already in use OpenVPN Server/Client Tunnel Network",
            "status": 400,
            "return": 2117,
            "req_data": {
                "tunnel_network": "INVALID_TUNNEL_NETWORK",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Remote network(s)",
            "status": 400,
            "return": 2118,
            "req_data": {
                "remote_network": "INVALID_REMOTE_NETWORK",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Allow Compression",
            "status": 400,
            "return": 2120,
            "req_data": {
                "allow_compression": "INVALID_ALLOW_COMPRESSION",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Compression",
            "status": 400,
            "return": 2121,
            "req_data": {
                "allow_compression": "yes",
                "compression": "INVALID_COMPRESSION",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Topology",
            "status": 400,
            "return": 2122,
            "req_data": {
                "dev_mode": "tun",
                "topology": "INVALID_TOPOLOGY",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Ping Method",
            "status": 400,
            "return": 2123,
            "req_data": {
                "ping_method": "INVALID_PING_METHOD",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Keepalive Interval",
            "status": 400,
            "return": 2124,
            "req_data": {
                "ping_method": "keepalive",
                "keepalive_interval": "INVALID_KEEPALIVE_INTERVAL",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Keepalive Timeout",
            "status": 400,
            "return": 2125,
            "req_data": {
                "ping_method": "keepalive",
                "keepalive_timeout": "INVALID_KEEPALIVE_TIMEOUT",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Ping Seconds",
            "status": 400,
            "return": 2126,
            "req_data": {
                "ping_method": "ping",
                "ping_seconds": "INVALID_PING_SECONDS",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Ping restart or exit seconds",
            "status": 400,
            "return": 2127,
            "req_data": {
                "ping_method": "ping",
                "ping_action_seconds": "INVALID_PING_ACTION_SECONDS",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Ping restart or exit",
            "status": 400,
            "return": 2128,
            "req_data": {
                "ping_method": "ping",
                "ping_action": "INVALID_PING_ACTION",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Exit Notify",
            "status": 400,
            "return": 2129,
            "req_data": {
                "exit_notify": "INVALID_EXIT_NOTIFY",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Send/Receive Buffer",
            "status": 400,
            "return": 2130,
            "req_data": {
                "sndrcvbuf": "INVALID_SNDRCVBUF",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Gateway Creation",
            "status": 400,
            "return": 2131,
            "req_data": {
                "create_gw": "INVALID_CREATE_GW",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Verbosity Level",
            "status": 400,
            "return": 2132,
            "req_data": {
                "verbosity_level": "INVALID_VERBOSITY_LEVEL",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Inactive Time",
            "status": 400,
            "return": 2134,
            "req_data": {
                "inactive_seconds": "INVALID_INACTIVE_TIME",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Missing OpenVPN Server/Client Certificate",
            "status": 400,
            "return": 2135,
            "no_certref": True,
            # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "req_data": {
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Missing OpenVPN Server/Client Certificate Authority",
            "status": 400,
            "return": 2136,
            "no_caref": True,  # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "req_data": {
                "server_addr": "openvpn.example.com"
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Peer Certificate Revocation List",
            "status": 400,
            "return": 2137,
            "req_data": {
                "crlref": "INVALID_CRLREF",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Data Encryption Algorithms",
            "status": 400,
            "return": 2138,
            "req_data": {
                "data_ciphers": "INVALID_CRLREF",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Missing OpenVPN Server/Client 'shared_key', parameter is needed for server mode 'p2p_shared_key'",
            "status": 400,
            "return": 2141,
            "no_certref": True, # Prevents overriden post_post() method from auto-adding the created Certificate ref ID
            "no_caref": True,  # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "req_data": {
                "mode": "p2p_shared_key",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Certificate Authority",
            "status": 400,
            "return": 2142,
            "no_caref": True,  # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "req_data": {
                "caref": "INVALID_CA_REF",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Server/Client Certificate",
            "status": 400,
            "return": 2143,
            "no_certref": True,
            # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "req_data": {
                "certref": "INVALID_CERT_REF",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "OpenVPN Client the field 'Server port' must contain a valid port, ranging from 0 to 65535.",
            "status": 400,
            "return": 2154,
            "req_data": {
                "server_port": "99999",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "OpenVPN Client the field 'Local port' must contain a valid port, ranging from 0 to 65535.",
            "status": 500,
            "return": 2108,
            "req_data": {
                "local_port": "99999",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "OpenVPN Client the field 'Server host or address' must contain a valid IP address or domain name.",
            "status": 400,
            "return": 2153,
            "req_data": {}
        },
        {
            "name": "OpenVPN Client the field 'Proxy port' must contain a valid port, ranging from 0 to 65535.",
            "status": 400,
            "return": 2155,
            "req_data": {
                "proxy_port": "99999",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "Unknown OpenVPN Client Proxy Authentication type.",
            "status": 400,
            "return": 2156,
            "req_data": {
                "proxy_authtype": "INVALID_AUTH_TYPE",
                "server_addr": "openvpn.example.com",
            }
        },
        {
            "name": "OpenVPN Client the field 'User shaper' must be in valid range, ranging from 100 to 100,000,000.",
            "status": 400,
            "return": 2157,
            "req_data": {
                "use_shaper": "1",
                "server_addr": "openvpn.example.com",
            }
        },
    ]
    put_tests = [
        {
            "name": "OpenVPN Client Update 1",
            "status": 200,
            "return": 0,
            "no_certref": True,
            # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "no_caref": True,  # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "req_data": {
                "description": "TEST_Update_OpenVPN_Client_1",
                "mode": "p2p_shared_key",
                "shared_key": ""
            }
        },
        {
            "name": "OpenVPN Client Update 2",
            "status": 200,
            "return": 0,
            "req_data": {
                "description": "TEST_Update_OpenVPN_Client_2",
                "mode": "p2p_tls",
                "tls": "false",
                "dev_mode": "tap",
                "local_network": "4.3.2.1/24",
                "exit_notify": "x2",
                "dns_add": True,
                "route_no_exec": True,
                "passtos": True,
                "route_no_pull": True,
                "ping_method": "keepalive",
                "keepalive_timeout": "70",
                "keepalive_interval": "20",
                "udp_fast_io": True,
                "sndrcvbuf": "2MiB",
                "create_gw": "v4only"
            }
        }
    ]
    delete_tests = [
        {
            "name": "OpenVPN Server/Client cannot delete an OpenVPN instance while interface is assigned",
            "status": 400,
            "return": 2152,
            "req_data": {}
        },
        {
            "name": "Delete Interface (ovpnc1)",
            "uri": "/api/v1/interface",
            "status": 200,
            "return": 0,
            "req_data": {}
        },
        {
            "name": "Delete OpenVPN Client Instance",
            "status": 200,
            "return": 0,
            "req_data": {},
            "resp_time": 10
        },  # vpnid gets populated by post_post() method
        {
            "name": "Delete Certificate",
            "uri": "/api/v1/system/certificate",
            "status": 200,
            "return": 0,
            "req_data": {"descr": "INTERNAL_CERT_RSA"}
        },
        {
            "name": "Delete CA certificate",
            "uri": "/api/v1/system/ca",
            "status": 200,
            "return": 0,
            "req_data": {"descr": "INTERNAL_CA_TEST"}
        },
        {
            "name": "Unknown OpenVPN Server/Client vpnid",
            "status": 404,
            "return": 2139,
            "req_data": {
                "vpnid": "INVALID_VPNID"
            }
        },
        {
            "name": "Missing OpenVPN Server/Client 'vpnid', parameter is needed to identify server to modify/delete",
            "status": 400,
            "return": 2140,
            "req_data": {}
        },
    ]

    # Override our PRE/POST methods
    def post_post(self):
        if len(self.post_responses) == 2:
            # Variables
            counter = 0
            for test in self.post_tests:
                # Assign the required refid created in the POST request to the POST/PUT req_datas
                if "req_data" in test and "no_caref" not in test:
                    self.post_tests[counter]["req_data"]["caref"] = self.post_responses[1]["data"]["refid"]
                    self.put_tests[1]["req_data"]["caref"] = self.post_responses[1]["data"]["refid"]
                    # self.put_tests[1]["req_data"]["caref"] = self.post_responses[1]["data"]["refid"]
                counter = counter + 1

        if len(self.post_responses) == 4:
            # Variables
            counter = 0
            for test in self.post_tests:
                # Assign the required refid created in the POST request to the POST/PUT req_datas
                if "req_data" in test and "no_certref" not in test:
                    self.post_tests[counter]["req_data"]["certref"] = self.post_responses[3]["data"]["refid"]
                    self.put_tests[1]["req_data"]["certref"] = self.post_responses[3]["data"]["refid"]
                counter = counter + 1

        if len(self.post_responses) == 5:
            # Variables
            # counter = 0
            for _ in self.post_tests:
                # Assign the required vpnid created in the POST request to the DELETE/PUT req_datas
                self.delete_tests[0]["req_data"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                self.delete_tests[2]["req_data"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                self.put_tests[0]["req_data"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                self.put_tests[1]["req_data"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                # counter = counter + 1

        if len(self.post_responses) == 6:
            # Variables
            # counter = 0
            for _ in self.post_tests:
                # Assign the required vpnid created in the POST request to the DELETE req_datas
                self.delete_tests[1]["req_data"]["if"] = self.post_responses[5]["data"]["if"]


APIE2ETestOpenVPNClient()
