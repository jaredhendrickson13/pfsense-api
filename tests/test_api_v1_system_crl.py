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
            "name": "Create internal CRL",
            "payload": {
                "caref": "61c410f04b782",
                "method": "internal",
                "descr": "INTERNAL_CRL_TEST",
                "lifetime": 3650,
                "serial": 10,
            },
            "resp_time": 10
        },
        # {
        #     "name": "Create existing CRL",
        #     "payload": {
        #         "method": "existing",
        #         "descr": "EXISTING_CRL",
        #         "lifetime": 3650,
        #         "crl_data": crl_text
        #     },
        #     "resp_time": 10
        # },
        {
            "name": "Check signing CA reference ID requirement for intermediate method",
            "status": 400,
            "return": 1047,
            "payload": {"method": "internal", "descr": "TestCRL"}
        },
        {
            "name": "Check non-existing signing CA reference ID for intermediate method",
            "status": 400,
            "return": 1048,
            "payload": {"method": "internal", "descr": "TestCRL", "caref": "invalid"}
        },
        {
            "name": "Check method requirement",
            "status": 400,
            "return": 1031,
            "payload": {"descr": "TestCRL", "caref": "61c410f04b782"}
        },
        {
            "name": "Check unsupported method",
            "status": 400,
            "return": 1032,
            "payload": {"method": "INVALID_METHOD", "descr": "TestCRL", "caref": "61c410f04b782"}
        },
        {
            "name": "Check description character validation",
            "status": 400,
            "return": 1037,
            "payload": {"method": "internal", "descr": "<>?&>", "caref": "61c410f04b782"}
        },
        {
            "name": "Check description requirement",
            "status": 400,
            "return": 1002,
            "payload": {"method": "internal", "caref": "61c410f04b782"}
        },
        {
            "name": "Check description requirement",
            "status": 400,
            "return": 6030,
            "payload": {"method": "existing", "descr": "TestCRL", "caref": "61c410f04b782"}
        },
        {
            "name": "Check lifetime maximum constraint for internal method",
            "status": 400,
            "return": 1046,
            "payload": {"method": "internal", "descr": "TestCA", "caref": "61c410f04b782", "lifetime": 50000}
        },
        {
            "name": "Check serial numeric validation with existing method",
            "status": 400,
            "return": 1033,
            "payload": {"method": "internal", "descr": "TestCA", "caref": "61c410f04b782", "serial": "invalid"}
        },
    ]
    delete_tests = [
        {
            "name": "Delete CRL certificate without key",
            "payload": {"descr": "INTERNAL_CRL_TEST"},
            "resp_time": 10
        },
        {
            "name": "Check deletion of non-existing CRL",
            "status": 400,
            "return": 6032,
            "payload": {"refid": "INVALID"}
        },
        # TODO: add test to check that CRLs in use cannot be deleted
    ]

    # def post_post(self):
    #     # Check if this is after the first POST test
    #     if len(self.post_responses) == 1:
    #         self.is_return_crt_base64(self.post_responses[0])


    # def is_return_crt_base64(self, response):
    #     """
    #     Takes a test response and checks if the return 'crt' data field is a Base64 encoded certificate
    #     """
    #     # Variables
    #     test_params = {"name": "Ensure cert is Base64 encoded"}
    #     crt = base64.b64decode(response["data"]["crl"])

    #     # Ensure the returned Base64 decoded crt value is a certificate
    #     if "BEGIN CERTIFICATE" not in crt.decode():
    #         self.exit_code = 1
    #         print(self.__format_msg__("POST", test_params, "Returned certificate is not Base64 encoded"))
    #     else:
    #         print(self.__format_msg__("POST", test_params, "Response is valid", "ok"))

APIE2ETestSystemCRL()
