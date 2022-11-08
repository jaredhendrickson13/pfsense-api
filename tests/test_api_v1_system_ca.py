"""Script used to test the /api/v1/system/ca endpoint."""
import base64
import e2e_test_framework

# Constants
CRT = "LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUR5VENDQXJHZ0F3SUJBZ0lVZUZacVZwcXlDNXRqa0I2TWNwdnIybGlHRDc0d0RRWU" \
      "pLb1pJaHZjTkFRRUwKQlFBd2N6RUxNQWtHQTFVRUJoTUNWVk14RFRBTEJnTlZCQWdNQkZWMFlXZ3hEakFNQmdOVkJBY01CVkJ5YjNadgpN" \
      "UnN3R1FZRFZRUUtEQkpxWVhKbFpHaGxibVJ5YVdOcmMyOXVNVE14RWpBUUJnTlZCQXNNQ1VSbGRtVnNiM0JsCmNqRVVNQklHQTFVRUF3d0" \
      "xjR1p6Wlc1elpTMWhjR2t3SUJjTk1qQXdPVEk0TVRnek1EQXlXaGdQTXpBeU1EQXgKTXpBeE9ETXdNREphTUhNeEN6QUpCZ05WQkFZVEFs" \
      "VlRNUTB3Q3dZRFZRUUlEQVJWZEdGb01RNHdEQVlEVlFRSApEQVZRY205MmJ6RWJNQmtHQTFVRUNnd1NhbUZ5WldSb1pXNWtjbWxqYTNOdm" \
      "JqRXpNUkl3RUFZRFZRUUxEQWxFClpYWmxiRzl3WlhJeEZEQVNCZ05WQkFNTUMzQm1jMlZ1YzJVdFlYQnBNSUlCSWpBTkJna3Foa2lHOXcw" \
      "QkFRRUYKQUFPQ0FROEFNSUlCQ2dLQ0FRRUFvQ2x3d1Jzajg4Tnk0Z1Zid2NhRUYzU0s4SlQvZEowQUxjV1F4Wnh1WUt3MgpHMldCK0Y3RW" \
      "ZBbTNVN21qNEt0bWF0ZEhEWVppZ1c0T0dzSWE0dVZKaGhVWDJ0RlMvcGQ2UHlFa2ZyMHFvcm1nCm84MnNJUW9WZS84YTRVRzJYeXl1SkRO" \
      "Vks2SjJJS1hodUt2dEpCVk5xZlJoZExVNDNHLzAxZjBnTkwrSlE4VDMKVlpCUFgyZXpMK1hNUGg2ZkFpRG5MNmp2c0F3ZTZ4cEhEYTVDL0" \
      "l1VmJ6Z2V6YUNiREFneVcvZFowcDltNzNPNApBWnV3UXVwUUNTUkZWenJnS3dyaUF0SDhnVGRtVWtKdG10a2hwL0R3bTRha2k3dmpsYTI0" \
      "R0JsZXVlQzJ6azhZCkhMMGxERENBeWtsM2o5UEpUM0ttbC9LVzFkQ3FvcTZTWmYxNTRZZzlEd0lEQVFBQm8xTXdVVEFkQmdOVkhRNEUKRm" \
      "dRVU5veC9UdE45SWxLMlA3YjF1ZzJ1dHJ2ZGtXY3dId1lEVlIwakJCZ3dGb0FVTm94L1R0TjlJbEsyUDdiMQp1ZzJ1dHJ2ZGtXY3dEd1lE" \
      "VlIwVEFRSC9CQVV3QXdFQi96QU5CZ2txaGtpRzl3MEJBUXNGQUFPQ0FRRUFhT2xsCmMrbDRHTXZjNDBKUnlGRDdoeUJlSkVsN0x5NmVpeF" \
      "pxNUdzU0hpbEJiT2M5MmQ3b1dja3ptZGNIT0hqdlRwU3kKTXpOclVqcGFoMlZDSXZXMXhXaHEwMWJMQnJwRmtqNmNwbkY3d2NTVnlSODdS" \
      "OG4za0x2dlRqMEhoVE9rb1FRVwp2VGVTei9RaytFVm9SeHdob3J5U2VnWW9yQTRScUZyd2c1a3puZGVrM0gwSXcyQzkxZVBUbjRmSU5mTk" \
      "pUTnhHCmc3eDhxWCtySFl4L0R2Y0hjSVEzYVlzYVJ1TXNTYmtHYjdwUXZmOXNneE1weC9ucU8xS0RKVUUrOTVRQTJOa3oKTldYeDFaeVVV" \
      "cUNOd0RVVENaczNzczVYSWJrdTJSWXhmNWxMTG03YnQrUHZwY3RVOVRSUzlmQWUvQXpldjI3KwpTQzM2Nm1uYnh0OG5xVnR1K0E9PQotLS" \
      "0tLUVORCBDRVJUSUZJQ0FURS0tLS0tCg=="
KEY = "LS0tLS1CRUdJTiBQUklWQVRFIEtFWS0tLS0tCk1JSUV2UUlCQURBTkJna3Foa2lHOXcwQkFRRUZBQVNDQktjd2dnU2pBZ0VBQW9JQkFRQ2" \
      "dLWERCR3lQenczTGkKQlZ2QnhvUVhkSXJ3bFA5MG5RQXR4WkRGbkc1Z3JEWWJaWUg0WHNSOENiZFR1YVBncTJacTEwY05obUtCYmc0YQp3" \
      "aHJpNVVtR0ZSZmEwVkwrbDNvL0lTUit2U3FpdWFDanphd2hDaFY3L3hyaFFiWmZMSzRrTTFVcm9uWWdwZUc0CnErMGtGVTJwOUdGMHRUam" \
      "NiL1RWL1NBMHY0bER4UGRWa0U5Zlo3TXY1Y3crSHA4Q0lPY3ZxTyt3REI3ckdrY04KcmtMOGk1VnZPQjdOb0pzTUNESmI5MW5TbjJidmM3" \
      "Z0JtN0JDNmxBSkpFVlhPdUFyQ3VJQzBmeUJOMlpTUW0yYQoyU0duOFBDYmhxU0x1K09WcmJnWUdWNjU0TGJPVHhnY3ZTVU1NSURLU1hlUD" \
      "A4bFBjcWFYOHBiVjBLcWlycEpsCi9YbmhpRDBQQWdNQkFBRUNnZ0VBRlpCKzFnRkpmZkM2N3lPNWp3V2prMlRsc0M3ZmxsdnRRanh2bWF2" \
      "T1VNWGYKSXlFdnRybEx5MGVqbjJwSFhtQzFrWDBhMi85VUZBazFiUFRsbWRjMVp4QS8vZjVoSmxaTzUyRVhBTm1IZkJGeQpSNXZScVVFcV" \
      "UxK3R4dGFLTDVaY2ZCTk5UR3E3YlBub3dteWpxVkFVL09VaW1nd3NjOEcvUFhDdmZXcXNtS3NkCit5WEd0dlJJNC9sdmtrbDJMRzRGc000" \
      "bE9hMXZJNXovZ2Zhb0FEdUZsVUpsVHk4S2FCVElMZW9tTmJRc01jSEsKeDVkTEFXR0lieXB4K1pQQnQ3VTZ0SnFlOElScHZlbmR2cDZoTT" \
      "hFS2x4dlNxMVNVbFNya3ltT0dtL0NKSzRSbgprVmV1L1pnUjlTbW1tTm9MM2RTUk0vMzJIUTkxTkpJdlJWVGRMU1RhZ1FLQmdRREt3VXI1" \
      "dlJJYURjSGlGZjRzCnhicW1Gbm4vYkVBWWNUclFXaFllZGRQY3dkczJNWjNwcXh6Z3doSGlUYVFnMEwrUFJsV000TytlbXAwOFVDM3QKcW" \
      "F4dys4ZDNYVjgzSGZadHJFRTJqQkd5V0l5dHFzWlBuS2haa3VhdkN1NWkzUlNLVG1CcUw3cGF6TDZGRXhEeAo4OTU2aWdPR3hXbkhvRE1y" \
      "KzRla1ZQZzExd0tCZ1FES09MZDRpd20xTlpHWHA3K21TVVpsVnM4a0J1aysyRGNhCjBvUWdKaEJ4M2FPZmRESTU0Q0Zqb0ZQUytBZjc2T3" \
      "FSMUlNYWl5a1BMWFVoZzBsdHBac3VJWlBSTFFmdUpuK20KajhXbGFWT1NDeFoxMjkvKzZ1RGZwTUR1WFVZeW9wNS9QK0RJSzJhY2NGTWlz" \
      "THRHLzZncEJWN2ZBU2QxazQ4RQpRUWNOYlBCYmlRS0JnUUNDZXBmRVZhOVRndXoxa00rc2dtYVdRYnFxN0Qvbk90N3RmRHZseUUvYUxncm" \
      "pPbFQwCkxnRDhod2U1U2R2SW5tM1lSeHdBK0RSY0xnWG43WFZSRDdNQVZwZExzcFAyeFZwenc3bUgzK1gzanRLaFpGZ1EKbmJFZFM5TVdi" \
      "SU55cmZGcysvbEIvSXNCcWVjbGZscVdTaWt2VktmbVVCNjlyOU9laDFVSUpRSkNxd0tCZ0FveQpyQVgzTlFrZlozVTNiM0hLVmpOOEdqd2" \
      "Q0UnRiT2dRdlE1eC9idXJmRzRaS0ROSmdYQzZ6QWljc2ZQS1dQMllWClNudEhNMDNoby91SnJHVk1LYlE4MjBCOFBkOGpyK0pOYzlFd3E1" \
      "YzgyZWdkcTRFbWhTcWlHMXlwOVlWT01DSUkKcmFSS2xBVWxvUHVwMy9mbm9xcFc2LzdoQndWbDZKdDFVQTY4Uks3SkFvR0FXL0ZkSVk0Y3" \
      "djMEZHMjN6dWYvUApTZTNESktTZ3BYSlA2ZDZ2SjRDNnAvRDcwNytUQ1JpWVBQMnJ4QlpDRk1ZczJ2M01BN2lvM0lpV3VjOStabHFMCkxW" \
      "bk4zNEQ1dGZhMExnQzArVFZtOTNEZHE2SFd1WWdYMUx0MzFXeFhMMkpXREpmazcxc3prNW51NGRkSTNkdnAKU0ducTB1UjRlR0IxK2ZRNH" \
      "luNlgydkk9Ci0tLS0tRU5EIFBSSVZBVEUgS0VZLS0tLS0K"


class APIE2ETestSystemCA(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/ca endpoint."""
    uri = "/api/v1/system/ca"

    post_privileges = ["page-all", "page-system-camanager"]
    get_privileges = ["page-all", "page-system-camanager"]
    delete_privileges = ["page-all", "page-system-camanager"]

    get_tests = [{"name": "Read system CAs"}]
    post_tests = [
        {
            "name": "Import CA without key",
            "req_data": {
                "method": "existing",  # Check that existing method works the same as import
                "crt": CRT,
                "descr": "CA_NO_KEY",
                "randomserial": True
            },
        },
        {
            "name": "Import CA with key",
            "req_data": {
                "method": "import",  # Check that existing method works the same as import
                "crt": CRT,
                "prv": KEY,
                "descr": "CA_WITH_KEY",
                "trust": True
            },
        },
        {
            "name": "Create RSA internal CA",
            "req_data": {
                "method": "internal",
                "descr": "INTERNAL_CA_RSA",
                "trust": True,
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "internal-ca-e2e-test.example.com",
                "dn_country": "US",
                "dn_city": "Salt Lake City",
                "dn_state": "Utah",
                "dn_organization": "Test Company",
                "dn_organizationalunit": "IT"
            },
        },
        {
            "name": "Create ECDSA internal CA",
            "req_data": {
                "method": "internal",
                "descr": "INTERNAL_CA_ECDSA",
                "trust": True,
                "keytype": "ECDSA",
                "ecname": "prime256v1",
                "digest_alg": "sha512",
                "lifetime": 365,
                "dn_commonname": "internal-ca-e2e-test.example.com",
                "dn_country": "US",
                "dn_city": "Salt Lake City",
                "dn_state": "Utah",
                "dn_organization": "Test Company",
                "dn_organizationalunit": "IT"
            },
        },
        {
            "name": "Check method requirement",
            "status": 400,
            "return": 1031
        },
        {
            "name": "Check unsupported method",
            "status": 400,
            "return": 1032,
            "req_data": {"method": "INVALID_METHOD"}
        },
        {
            "name": "Check description requirement",
            "status": 400,
            "return": 1002,
            "req_data": {"method": "internal"}
        },
        {
            "name": "Check description character validation",
            "status": 400,
            "return": 1037,
            "req_data": {"method": "internal", "descr": "<>?&>"}
        },
        {
            "name": "Check serial numeric validation with existing method",
            "status": 400,
            "return": 1033,
            "req_data": {"method": "existing", "descr": "TEST", "crt": CRT, "prv": KEY, "serial": "invalid"}
        },
        {
            "name": "Check certificate requirement with existing method",
            "status": 400,
            "return": 1003,
            "req_data": {"method": "existing", "descr": "TestCA"}
        },
        {
            "name": "Check encrypted key rejection",
            "status": 400,
            "return": 1036,
            "req_data": {"method": "existing", "descr": "TestCA", "crt": CRT, "prv": "RU5DUllQVEVECg=="}
        },
        {
            "name": "Check certificate key matching with existing method",
            "status": 400,
            "return": 1049,
            "req_data": {"method": "existing", "descr": "TestCA", "crt": CRT, "prv": "INVALID KEY"}
        },
        {
            "name": "Check signing CA reference ID requirement for intermediate method",
            "status": 400,
            "return": 1047,
            "req_data": {"method": "intermediate", "descr": "TestCA"}
        },
        {
            "name": "Check non-existing signing CA reference ID for intermediate method",
            "status": 400,
            "return": 1048,
            "req_data": {"method": "intermediate", "descr": "TestCA", "caref": "invalid"}
        },
        {
            "name": "Check key type requirement for internal method",
            "status": 400,
            "return": 1038,
            "req_data": {"method": "internal", "descr": "TestCA"}
        },
        {
            "name": "Check unknown key type for internal method",
            "status": 400,
            "return": 1039,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "invalid"}
        },
        {
            "name": "Check key length requirement for internal method",
            "status": 400,
            "return": 1040,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "RSA"}
        },
        {
            "name": "Check unknown key length for internal method",
            "status": 400,
            "return": 1041,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "RSA", "keylen": "invalid"}
        },
        {
            "name": "Check EC name requirement for internal method",
            "status": 400,
            "return": 1042,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "ECDSA"}
        },
        {
            "name": "Check unknown EC name for internal method",
            "status": 400,
            "return": 1043,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "ECDSA", "ecname": "invalid"}
        },
        {
            "name": "Check digest algorithm requirement for internal method",
            "status": 400,
            "return": 1044,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "RSA", "keylen": 2048}
        },
        {
            "name": "Check unknown digest algorithm for internal method",
            "status": 400,
            "return": 1045,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "ECDSA", "ecname": "prime256v1",
                        "digest_alg": "invalid"}
        },
        {
            "name": "Check lifetime maximum constraint for internal method",
            "status": 400,
            "return": 1046,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "ECDSA", "ecname": "prime256v1",
                        "digest_alg": "sha256", "lifetime": 50000}
        },
        {
            "name": "Check unknown country for internal method",
            "status": 400,
            "return": 1051,
            "req_data": {"method": "internal", "descr": "TestCA", "keytype": "ECDSA", "ecname": "prime256v1",
                        "digest_alg": "sha256", "lifetime": 365, "dn_country": "invalid"}
        },
    ]
    delete_tests = [
        {
            "name": "Delete CA certificate without key",
            "req_data": {"descr": "CA_NO_KEY"}
        },
        {
            "name": "Delete CA certificate with key",
            "req_data": {"descr": "CA_WITH_KEY"}
        },
        {
            "name": "Delete internal CA RSA",
            "req_data": {"descr": "INTERNAL_CA_RSA"}
        },
        {
            "name": "Delete internal CA ECDSA",
            "req_data": {"descr": "INTERNAL_CA_ECDSA"}
        },
        {
            "name": "Check deletion of non-existing CA",
            "status": 400,
            "return": 1009,
            "req_data": {"descr": "INVALID"}
        }
    ]

    def post_post(self):
        # Check if this is after the first POST test
        if len(self.post_responses) == 1:
            self.is_return_crt_base64(self.post_responses[0])

    def is_return_crt_base64(self, response):
        """
        Takes a test response and checks if the return 'CRT' data field is a Base64 encoded certificate
        """
        # Variables
        test_params = {"name": "Ensure cert is Base64 encoded"}
        crt = base64.b64decode(response["data"]["crt"])

        # Ensure the returned Base64 decoded CRT value is a certificate
        if "BEGIN CERTIFICATE" not in crt.decode():
            self.exit_code = 1
            print(self.__format_msg__("POST", test_params, "Returned certificate is not Base64 encoded"))
        else:
            print(self.__format_msg__("POST", test_params, "Response is valid", "ok"))


APIE2ETestSystemCA()
