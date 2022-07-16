from operator import truediv
import e2e_test_framework
import base64

class APIE2ETestOpenVPNServer(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/services/openvpn/server"
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
    get_tests = [{"name": "Read all OpenVPN Server instances"}]
    post_tests = [
        {
            "name": "OpenVPN Server No Certificate Authority found",
            "status": 400,
            "return": 2144,
            "payload": {}
        },
        {
            "name": "Create RSA internal CA",
            "uri": "/api/v1/system/ca",
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {
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
            "name": "OpenVPN Server No Server Certificate found",
            "status": 400,
            "return": 2135,
            "payload": {}
        },
        {
            "name": "Create internal certificate with RSA key",
            "uri": "/api/v1/system/certificate",
            "no_certref": True,    # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "payload": {
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
            "name": "Create OpenVPN Server",
            "status": 200,
            "return": 0,
            "payload": {
                "protocol": "udp4",
                "description": "TEST_Create_OpenVPN_SERVER",
                "tls": "",
                "tls_type": "crypt",
                "tlsauth_keydir": "both-directions",
                "ecdh_curve": "prime256v1",
                "ncp_enable": True,
                "data_ciphers": "AES-128-GCM,CHACHA20-POLY1305",
                "data_ciphers_fallback": "AES-256-CBC",
                "digest": "SHA512",
                "dh_length": "4096",
                "tunnel_network": "1.2.3.4/24",
                "concurrent_connections": 15,
                "custom_options": 'push "route 10.0.0.0 255.255.255.0";'
            }
        },
        {
            "name": "Create Interface (ovpns1)",
            "uri": "/api/v1/interface",
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "no_certref": True,    # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            # "resp_time": 30,
            "payload": {
                "if": "ovpns1",
                "descr": "TEST_INTERFACE_OVPNS1",
            }
        },
        {
            "name": "Unknown OpenVPN Server Mode",
            "status": 400,
            "return": 2104,
            "payload": {
                "mode": "INVALID_MODE"
            }
        },
        {
            "name": "Unknown OpenVPN Server Protocol",
            "status": 400,
            "return": 2105,
            "payload": {
                "protocol": "INVALID_PROTOCOL"
            }
        },
        {
            "name": "Unknown OpenVPN Server Device mode",
            "status": 400,
            "return": 2106,
            "payload": {
                "dev_mode": "INVALID_DEV_MODE"
            }
        },
        {
            "name": "Unknown OpenVPN Server Interface",
            "status": 400,
            "return": 2107,
            "payload": {
                "interface": "INVALID_INTERFACE"
            }
        },
        {
            "name": "Invalid or taken OpenVPN Server Local port (allowed range 1-65535)",
            "status": 500,
            "return": 2108,
            "payload": {
                "local_port": "99999"
            }
        },
        {
            "name": "Unknown OpenVPN Server TLS Key Usage Mode",
            "status": 400,
            "return": 2109,
            "payload": {
                "tls": tls_key_text,
                "tls_type": "INVALID_TLS_TYPE"
            }
        },
        {
            "name": "Unknown OpenVPN Server TLS keydir direction",
            "status": 400,
            "return": 2110,
            "payload": {
                "tls": tls_key_text,
                "tlsauth_keydir": "INVALID_TLS_KEYDIR"
            }
        },
        {
            "name": "Unknown OpenVPN Server TLS DH Parameter Length",
            "status": 400,
            "return": 2111,
            "payload": {
                "dh_length": "INVALID_DH_LENGTH"
            }
        },
        {
            "name": "Unknown OpenVPN Server ECDH Curve",
            "status": 400,
            "return": 2112,
            "payload": {
                "ecdh_curve": "INVALID_ECDH_CURVE"
            }
        },
        {
            "name": "Unknown OpenVPN Server Certificate Depth",
            "status": 400,
            "return": 2113,
            "payload": {
                "cert_depth": "INVALID_CERT_DEPTH"
            }
        },
        {
            "name": "Unknown OpenVPN Server Fallback Data Encryption Algorithm",
            "status": 400,
            "return": 2114,
            "payload": {
                "data_ciphers_fallback": "INVALID_DATA_CIPHERS_FALLBACK"
            }
        },
        {
            "name": "Unknown OpenVPN Server Auth Digest Algorithm",
            "status": 400,
            "return": 2115,
            "payload": {
                "digest": "INVALID_DIGEST"
            }
        },
        {
            "name": "Unknown OpenVPN Server Hardware Crypto Engine",
            "status": 400,
            "return": 2116,
            "payload": {
                "engine": "INVALID_ENGINE"
            }
        },
        {
            "name": "Unknown or already in use OpenVPN Server Tunnel Network",
            "status": 400,
            "return": 2117,
            "payload": {
                "tunnel_network": "INVALID_TUNNEL_NETWORK"
            }
        },
        {
            "name": "Unknown OpenVPN Server Remote network(s)",
            "status": 400,
            "return": 2118,
            "payload": {
                "remote_network": "INVALID_REMOTE_NETWORK"
            }
        },
        {
            "name": "Unknown OpenVPN Server Local network(s)",
            "status": 400,
            "return": 2119,
            "payload": {
                "local_network": "INVALID_LOCAL_NETWORK"
            }
        },
        {
            "name": "Unknown OpenVPN Server Allow Compression",
            "status": 400,
            "return": 2120,
            "payload": {
                "allow_compression": "INVALID_ALLOW_COMPRESSION"
            }
        },
        {
            "name": "Unknown OpenVPN Server Compression",
            "status": 400,
            "return": 2121,
            "payload": {
                "allow_compression": "yes",
                "compression": "INVALID_COMPRESSION"
            }
        },
        {
            "name": "Unknown OpenVPN Server Topology",
            "status": 400,
            "return": 2122,
            "payload": {
                "dev_mode": "tun",
                "topology": "INVALID_TOPOLOGY"
            }
        },
        {
            "name": "Unknown OpenVPN Server Ping Method",
            "status": 400,
            "return": 2123,
            "payload": {
                "ping_method": "INVALID_PING_METHOD"
            }
        },
        {
            "name": "Unknown OpenVPN Server Keepalive Interval",
            "status": 400,
            "return": 2124,
            "payload": {
                "ping_method": "keepalive",
                "keepalive_interval": "INVALID_KEEPALIVE_INTERVAL"
            }
        },
        {
            "name": "Unknown OpenVPN Server Keepalive Timeout",
            "status": 400,
            "return": 2125,
            "payload": {
                "ping_method": "keepalive",
                "keepalive_timeout": "INVALID_KEEPALIVE_TIMEOUT"
            }
        },
        {
            "name": "Unknown OpenVPN Server Ping Seconds",
            "status": 400,
            "return": 2126,
            "payload": {
                "ping_method": "ping",
                "ping_seconds": "INVALID_PING_SECONDS"
            }
        },
        {
            "name": "Unknown OpenVPN Server Ping restart or exit seconds",
            "status": 400,
            "return": 2127,
            "payload": {
                "ping_method": "ping",
                "ping_action_seconds": "INVALID_PING_ACTION_SECONDS"
            }
        },
        {
            "name": "Unknown OpenVPN Server Ping restart or exit",
            "status": 400,
            "return": 2128,
            "payload": {
                "ping_method": "ping",
                "ping_action": "INVALID_PING_ACTION"
            }
        },
        {
            "name": "Unknown OpenVPN Server Exit Notify",
            "status": 400,
            "return": 2129,
            "payload": {
                "exit_notify": "INVALID_EXIT_NOTIFY"
            }
        },
        {
            "name": "Unknown OpenVPN Server Send/Receive Buffer",
            "status": 400,
            "return": 2130,
            "payload": {
                "sndrcvbuf": "INVALID_SNDRCVBUF"
            }
        },
        {
            "name": "Unknown OpenVPN Server Gateway Creation",
            "status": 400,
            "return": 2131,
            "payload": {
                "create_gw": "INVALID_CREATE_GW"
            }
        },
        {
            "name": "Unknown OpenVPN Server Verbosity Level",
            "status": 400,
            "return": 2132,
            "payload": {
                "verbosity_level": "INVALID_VERBOSITY_LEVEL"
            }
        },
        {
            "name": "Unknown OpenVPN Server Concurrent Connections",
            "status": 400,
            "return": 2133,
            "payload": {
                "concurrent_connections": "INVALID_CONCURRENT_CONNECTIONS"
            }
        },
        {
            "name": "Unknown OpenVPN Server Inactive Time",
            "status": 400,
            "return": 2134,
            "payload": {
                "inactive_seconds": "INVALID_INACTIVE_SECONDS"
            }
        },
        {
            "name": "Missing OpenVPN Server Certificate",
            "status": 400,
            "return": 2135,
            "no_certref": True,    # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "payload": {}
        },
        {
            "name": "Missing OpenVPN Server Certificate Authority",
            "status": 400,
            "return": 2136,
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {}
        },
        {
            "name": "Unknown OpenVPN Server Peer Certificate Revocation List",
            "status": 400,
            "return": 2137,
            "payload": {
                "crlref": "INVALID_CRLREF"
            }
        },
        {
            "name": "Unknown OpenVPN Server Data Encryption Algorithms",
            "status": 400,
            "return": 2138,
            "payload": {
                "data_ciphers": "INVALID_DATA_ENCRYP_ALGO"
            }
        },
        {
            "name": "Missing OpenVPN Server 'shared_key'. This parameter is needed for server mode 'p2p_shared_key'",
            "status": 400,
            "return": 2141,
            "no_certref": True,    # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {
                "mode": "p2p_shared_key"
            }
        },
        {
            "name": "Unknown OpenVPN Server Certificate Authority",
            "status": 400,
            "return": 2142,
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {
                "caref": "INVALID_CA_REF",
            }
        },
        {
            "name": "Unknown OpenVPN Server Certificate",
            "status": 400,
            "return": 2143,
            "no_certref": True,    # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "payload": {
                "certref": "INVALID_CERT_REF",
            }
        },
        {
            "name": "Unknown OpenVPN Server Bridge Interface",
            "status": 400,
            "return": 2146,
            "payload": {
                "mode": "server_tls",
                "dev_mode": "tap",
                "serverbridge_dhcp": True,
                "serverbridge_interface": "INVALID_BRIDGE_INTERFACE"
            }
        },
        {
            "name": "Unknown OpenVPN Server Bridge Route Gateway requires a valid Bridge Interface",
            "status": 400,
            "return": 2147,
            "payload": {
                "mode": "server_tls",
                "dev_mode": "tap",
                "serverbridge_dhcp": True,
                "serverbridge_routegateway": True,
                "serverbridge_interface": "none"
            }
        },
        {
            "name": "Unknown OpenVPN Server Bridge DHCP Start and End must both be empty, or defined, or The Server Bridge DHCP range is invalid (start higher than end).",
            "status": 400,
            "return": 2148,
            "payload": {
                "mode": "server_tls",
                "dev_mode": "tap",
                "serverbridge_dhcp": True,
                "serverbridge_dhcp_start": "INVALID_DHCP_START"
            }
        },
        {
            "name": "OpenVPN Server The field 'NTP Server' must contain a valid IP address.",
            "status": 400,
            "return": 2149,
            "payload": {
                "mode": "server_tls",
                "ntp_servers": "INVALID_NTP_SERVER"
            }
        },
        {
            "name": "OpenVPN Server The field 'DNS Server' must contain a valid IP address.",
            "status": 400,
            "return": 2150,
            "payload": {
                "mode": "server_tls",
                "dns_servers": "INVALID_DNS_SERVER"
            }
        },
        {
            "name": "OpenVPN Server The field 'WINS Server' must contain a valid IP address.",
            "status": 400,
            "return": 2151,
            "payload": {
                "mode": "server_tls",
                "wins_servers": "INVALID_WINS_SERVER"
            }
        },
    ]
    put_tests = [
        {
            "name": "OpenVPN Server Update 1",
            "status": 200,
            "return": 0,
            "no_certref": True,    # Prevents the overriden post_post() method from auto-adding the created Certificate ref ID
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {
                "description": "TEST_Update_OpenVPN_SERVER_1",
                "mode": "p2p_shared_key",
                "shared_key": ""
            }
        },
        {
            "name": "OpenVPN Server Update 2",
            "status": 200,
            "return": 0,
            "payload": {
                "description": "TEST_Update_OpenVPN_SERVER_2",
                "mode": "server_tls_user",
                "tls": "false",
                "dev_mode": "tap",
                "local_network": "4.3.2.1/24",
                "compression_push": True,
                "client2client": True,
                "dynamic_ip": True,
                "exit_notify": "twice",
                "netbios_enable": True,
                "netbios_ntype": "m",
                "wins_servers": "10.0.0.10,10.0.0.20",
                "serverbridge_dhcp": True,
                "strictusercn": True,
                "username_as_common_name": True,
            }
        }
    ]
    delete_tests = [
        {
            "name": "OpenVPN Server cannot delete an OpenVPN instance while the interface is assigned. Remove the interface assignment first.",
            "status": 400,
            "return": 2152,
            "payload": {}
        },
        {
            "name": "Delete Interface (ovpns1)",
            "uri": "/api/v1/interface",
            "status": 200,
            "return": 0,
            "payload": {}
        },
        {
            "name": "Delete OpenVPN Server Instance",
            "status": 200,
            "return": 0,
            "payload": {},
            "resp_time": 10
        }, # vpnid gets populated by post_post() method
        {
            "name": "Delete Certificate",
            "uri": "/api/v1/system/certificate",
            "status": 200,
            "return": 0,
            "payload": {"descr": "INTERNAL_CERT_RSA"}
        },
        {
            "name": "Delete CA certificate",
            "uri": "/api/v1/system/ca",
            "status": 200,
            "return": 0,
            "payload": {"descr": "INTERNAL_CA_TEST"}
        },
        {
            "name": "Unknown OpenVPN Server 'vpnid'",
            "status": 404,
            "return": 2139,
            "payload": {
                "vpnid": "INVALID_VPNID"
            }
        },
        {
            "name": "Missing OpenVPN Server 'vpnid'. This parameter is needed to identify the server to modify/delete",
            "status": 400,
            "return": 2140,
            "payload": {}
        },
    ]
    
    # Override our PRE/POST methods
    def post_post(self):
        if len(self.post_responses) == 2:
            # Variables
            counter = 0
            for test in self.post_tests:
                # Assign the required refid created in the POST request to the POST/PUT payloads
                if "payload" in test.keys() and "no_caref" not in test.keys():
                    self.post_tests[counter]["payload"]["caref"] = self.post_responses[1]["data"]["refid"]
                    self.put_tests[1]["payload"]["caref"] = self.post_responses[1]["data"]["refid"]
                    # self.put_tests[1]["payload"]["caref"] = self.post_responses[1]["data"]["refid"]
                counter = counter + 1
        
        if len(self.post_responses) == 4:
            # Variables
            counter = 0
            for test in self.post_tests:
                # Assign the required refid created in the POST request to the POST/PUT payloads
                if "payload" in test.keys() and "no_certref" not in test.keys():
                    self.post_tests[counter]["payload"]["certref"] = self.post_responses[3]["data"]["refid"]
                    self.put_tests[1]["payload"]["certref"] = self.post_responses[3]["data"]["refid"]                    
                counter = counter + 1
        
        if len(self.post_responses) == 5:
            # Variables
            # counter = 0
            for test in self.post_tests:
                # Assign the required vpnid created in the POST request to the DELETE/PUT payloads
                self.delete_tests[0]["payload"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                self.delete_tests[2]["payload"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                self.put_tests[0]["payload"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                self.put_tests[1]["payload"]["vpnid"] = self.post_responses[4]["data"]["vpnid"]
                # counter = counter + 1

        if len(self.post_responses) == 6:
            # Variables
            # counter = 0
            for test in self.post_tests:
                # Assign the required vpnid created in the POST request to the DELETE payloads
                self.delete_tests[1]["payload"]["if"] = self.post_responses[5]["data"]["if"]

APIE2ETestOpenVPNServer()
