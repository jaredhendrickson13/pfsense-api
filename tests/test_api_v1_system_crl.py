import e2e_test_framework
import base64


class APIE2ETestSystemCRL(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/crl"
    crl_text = "-----BEGIN X509 CRL----- \
MIICnzCCAYcCAQEwDQYJKoZIhvcNAQEFBQAwgYUxKjAoBgNVBAMTIWludGVybmFs \
LWNhLXVuaXQtdGVzdC5leGFtcGxlLmNvbTELMAkGA1UEBhMCVVMxDTALBgNVBAgT \
BFV0YWgxFzAVBgNVBAcTDlNhbHQgTGFrZSBDaXR5MRUwEwYDVQQKEwxUZXN0IENv \
bXBhbnkxCzAJBgNVBAsTAklUFw0yMjAxMTYxMDU3MTJaFw00OTA2MDIxMDU3MTJa \
oIHMMIHJMIG5BgNVHSMEgbEwga6AFO1y5+VKpptoGIfaSmV4qC+4OmjVoYGLpIGI \
MIGFMSowKAYDVQQDEyFpbnRlcm5hbC1jYS11bml0LXRlc3QuZXhhbXBsZS5jb20x \
CzAJBgNVBAYTAlVTMQ0wCwYDVQQIEwRVdGFoMRcwFQYDVQQHEw5TYWx0IExha2Ug \
Q2l0eTEVMBMGA1UEChMMVGVzdCBDb21wYW55MQswCQYDVQQLEwJJVIIIBnad7uJF \
O0owCwYDVR0UBAQCAicQMA0GCSqGSIb3DQEBBQUAA4IBAQAAQ7RmrztHk+CjOlAZ \
eJ8CB5kpzFA5GaGXNGXipzZbwkeWUHmvx6Q5PoFOtCzX+2xaYnQUAQE5IAnBPKMj \
QeU+nzUk25jrsrVxgow9n/ZLKL5178/5HP6h0Bks90x7IwgaZiWMK4wip0t9f2N4 \
DGCqxKKphijOPLYknZOtGOUhir3ZGQit5UIF1XJwGKyw0evc4u3lwIGtHMCFa9Zo \
YVmJzk1NE+UiENtk1GXZO9R/0/PdyskeeZA4gC0XDNWXqD/kvVc5kFHicX1SRk27 \
BEswL+tABUNMaIVoGkVPSzlnSzHqEIVwC23S4w34o2pQUP0DRdhFaA+v21cAsBNa \
4bV6 \
-----END X509 CRL----- \
"
    get_tests = [{"name": "Read system CRLs"}]
    post_tests = [
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
            "name": "Check signing CA reference ID requirement for intermediate method",
            "status": 400,
            "return": 1047,
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {"method": "internal", "descr": "TestCRL"}
        },
        {
            "name": "Check non-existing signing CA reference ID for intermediate method",
            "status": 400,
            "return": 1048,
            "no_caref": True,    # Prevents the overriden post_post() method from auto-adding the created CA ref ID
            "payload": {"method": "internal", "descr": "TestCRL", "caref": "INVALID"}
        },
        {
            "name": "Create internal CRL",
            "payload": {
                "method": "internal",
                "descr": "INTERNAL_CRL_TEST",
                "lifetime": 3650,
                "serial": 10,
            },
            "resp_time": 10
        },
        {
            "name": "Create existing CRL",
            "payload": {
                "method": "existing",
                "descr": "EXISTING_CRL_TEST",
                "lifetime": 3650,
                "crl_data": crl_text
            },
            "resp_time": 10
        },
        {
            "name": "Check method requirement",
            "status": 400,
            "return": 1031,
            "payload": {"descr": "TestCRL"}
        },
        {
            "name": "Check unsupported method",
            "status": 400,
            "return": 1032,
            "payload": {"method": "INVALID_METHOD", "descr": "TestCRL"}
        },
        {
            "name": "Check description character validation",
            "status": 400,
            "return": 1037,
            "payload": {"method": "internal", "descr": "<>?&>"}
        },
        {
            "name": "Check description requirement",
            "status": 400,
            "return": 1002,
            "payload": {"method": "internal"}
        },
        {
            "name": "Check description requirement",
            "status": 400,
            "return": 1080,
            "payload": {"method": "existing", "descr": "TestCRL"}
        },
        {
            "name": "Check lifetime maximum constraint for internal method",
            "status": 400,
            "return": 1046,
            "payload": {"method": "internal", "descr": "TestCRL", "lifetime": 50000}
        },
        {
            "name": "Check serial numeric validation with existing method",
            "status": 400,
            "return": 1033,
            "payload": {"method": "internal", "descr": "TestCRL", "serial": "invalid"}
        },
    ]
    delete_tests = [
        {"name": "Delete CRL certificate with refid", "payload": {}, "resp_time": 10}, # refid gets populated by post_post() method
        {
            "name": "Delete CRL certificate with descr",
            "payload": {"descr": "EXISTING_CRL_TEST"},
            "resp_time": 10
        },
        {
            "name": "Delete CA certificate",
            "uri": "/api/v1/system/ca",
            "payload": {"descr": "INTERNAL_CA_TEST"}
        },
        {
            "name": "Check deletion of non-existing CRL",
            "status": 400,
            "return": 1082,
            "payload": {"refid": "INVALID"}
        },
        # TODO: add test to check that CRLs in use cannot be deleted
    ]
    
    # Override our PRE/POST methods
    def post_post(self):
        if len(self.post_responses) == 1:
            # Variables
            counter = 0
            for test in self.post_tests:
                # Assign the required refid created in the POST request to the DELETE payloads]
                if "payload" in test.keys() and "no_caref" not in test.keys():
                    self.post_tests[counter]["payload"]["caref"] = self.post_responses[0]["data"]["refid"]
                counter = counter + 1

        if len(self.post_responses) == 4:
            for test in self.post_tests:
                # Assign the required refid created in the POST request to the DELETE payloads
                self.delete_tests[0]["payload"]["refid"] = self.post_responses[3]["data"]["refid"]

APIE2ETestSystemCRL()
