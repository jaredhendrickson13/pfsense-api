import unit_test_framework


class APIUnitTestSystemCA(unit_test_framework.APIUnitTest):
    crt = "LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUR5VENDQXJHZ0F3SUJBZ0lVZUZacVZwcXlDNXRqa0I2TWNwdnIybGlHRDc0d0RRWUpLb1pJaHZjTkFRRUwKQlFBd2N6RUxNQWtHQTFVRUJoTUNWVk14RFRBTEJnTlZCQWdNQkZWMFlXZ3hEakFNQmdOVkJBY01CVkJ5YjNadgpNUnN3R1FZRFZRUUtEQkpxWVhKbFpHaGxibVJ5YVdOcmMyOXVNVE14RWpBUUJnTlZCQXNNQ1VSbGRtVnNiM0JsCmNqRVVNQklHQTFVRUF3d0xjR1p6Wlc1elpTMWhjR2t3SUJjTk1qQXdPVEk0TVRnek1EQXlXaGdQTXpBeU1EQXgKTXpBeE9ETXdNREphTUhNeEN6QUpCZ05WQkFZVEFsVlRNUTB3Q3dZRFZRUUlEQVJWZEdGb01RNHdEQVlEVlFRSApEQVZRY205MmJ6RWJNQmtHQTFVRUNnd1NhbUZ5WldSb1pXNWtjbWxqYTNOdmJqRXpNUkl3RUFZRFZRUUxEQWxFClpYWmxiRzl3WlhJeEZEQVNCZ05WQkFNTUMzQm1jMlZ1YzJVdFlYQnBNSUlCSWpBTkJna3Foa2lHOXcwQkFRRUYKQUFPQ0FROEFNSUlCQ2dLQ0FRRUFvQ2x3d1Jzajg4Tnk0Z1Zid2NhRUYzU0s4SlQvZEowQUxjV1F4Wnh1WUt3MgpHMldCK0Y3RWZBbTNVN21qNEt0bWF0ZEhEWVppZ1c0T0dzSWE0dVZKaGhVWDJ0RlMvcGQ2UHlFa2ZyMHFvcm1nCm84MnNJUW9WZS84YTRVRzJYeXl1SkROVks2SjJJS1hodUt2dEpCVk5xZlJoZExVNDNHLzAxZjBnTkwrSlE4VDMKVlpCUFgyZXpMK1hNUGg2ZkFpRG5MNmp2c0F3ZTZ4cEhEYTVDL0l1VmJ6Z2V6YUNiREFneVcvZFowcDltNzNPNApBWnV3UXVwUUNTUkZWenJnS3dyaUF0SDhnVGRtVWtKdG10a2hwL0R3bTRha2k3dmpsYTI0R0JsZXVlQzJ6azhZCkhMMGxERENBeWtsM2o5UEpUM0ttbC9LVzFkQ3FvcTZTWmYxNTRZZzlEd0lEQVFBQm8xTXdVVEFkQmdOVkhRNEUKRmdRVU5veC9UdE45SWxLMlA3YjF1ZzJ1dHJ2ZGtXY3dId1lEVlIwakJCZ3dGb0FVTm94L1R0TjlJbEsyUDdiMQp1ZzJ1dHJ2ZGtXY3dEd1lEVlIwVEFRSC9CQVV3QXdFQi96QU5CZ2txaGtpRzl3MEJBUXNGQUFPQ0FRRUFhT2xsCmMrbDRHTXZjNDBKUnlGRDdoeUJlSkVsN0x5NmVpeFpxNUdzU0hpbEJiT2M5MmQ3b1dja3ptZGNIT0hqdlRwU3kKTXpOclVqcGFoMlZDSXZXMXhXaHEwMWJMQnJwRmtqNmNwbkY3d2NTVnlSODdSOG4za0x2dlRqMEhoVE9rb1FRVwp2VGVTei9RaytFVm9SeHdob3J5U2VnWW9yQTRScUZyd2c1a3puZGVrM0gwSXcyQzkxZVBUbjRmSU5mTkpUTnhHCmc3eDhxWCtySFl4L0R2Y0hjSVEzYVlzYVJ1TXNTYmtHYjdwUXZmOXNneE1weC9ucU8xS0RKVUUrOTVRQTJOa3oKTldYeDFaeVVVcUNOd0RVVENaczNzczVYSWJrdTJSWXhmNWxMTG03YnQrUHZwY3RVOVRSUzlmQWUvQXpldjI3KwpTQzM2Nm1uYnh0OG5xVnR1K0E9PQotLS0tLUVORCBDRVJUSUZJQ0FURS0tLS0tCg=="
    key = "LS0tLS1CRUdJTiBQUklWQVRFIEtFWS0tLS0tCk1JSUV2UUlCQURBTkJna3Foa2lHOXcwQkFRRUZBQVNDQktjd2dnU2pBZ0VBQW9JQkFRQ2dLWERCR3lQenczTGkKQlZ2QnhvUVhkSXJ3bFA5MG5RQXR4WkRGbkc1Z3JEWWJaWUg0WHNSOENiZFR1YVBncTJacTEwY05obUtCYmc0YQp3aHJpNVVtR0ZSZmEwVkwrbDNvL0lTUit2U3FpdWFDanphd2hDaFY3L3hyaFFiWmZMSzRrTTFVcm9uWWdwZUc0CnErMGtGVTJwOUdGMHRUamNiL1RWL1NBMHY0bER4UGRWa0U5Zlo3TXY1Y3crSHA4Q0lPY3ZxTyt3REI3ckdrY04KcmtMOGk1VnZPQjdOb0pzTUNESmI5MW5TbjJidmM3Z0JtN0JDNmxBSkpFVlhPdUFyQ3VJQzBmeUJOMlpTUW0yYQoyU0duOFBDYmhxU0x1K09WcmJnWUdWNjU0TGJPVHhnY3ZTVU1NSURLU1hlUDA4bFBjcWFYOHBiVjBLcWlycEpsCi9YbmhpRDBQQWdNQkFBRUNnZ0VBRlpCKzFnRkpmZkM2N3lPNWp3V2prMlRsc0M3ZmxsdnRRanh2bWF2T1VNWGYKSXlFdnRybEx5MGVqbjJwSFhtQzFrWDBhMi85VUZBazFiUFRsbWRjMVp4QS8vZjVoSmxaTzUyRVhBTm1IZkJGeQpSNXZScVVFcVUxK3R4dGFLTDVaY2ZCTk5UR3E3YlBub3dteWpxVkFVL09VaW1nd3NjOEcvUFhDdmZXcXNtS3NkCit5WEd0dlJJNC9sdmtrbDJMRzRGc000bE9hMXZJNXovZ2Zhb0FEdUZsVUpsVHk4S2FCVElMZW9tTmJRc01jSEsKeDVkTEFXR0lieXB4K1pQQnQ3VTZ0SnFlOElScHZlbmR2cDZoTThFS2x4dlNxMVNVbFNya3ltT0dtL0NKSzRSbgprVmV1L1pnUjlTbW1tTm9MM2RTUk0vMzJIUTkxTkpJdlJWVGRMU1RhZ1FLQmdRREt3VXI1dlJJYURjSGlGZjRzCnhicW1Gbm4vYkVBWWNUclFXaFllZGRQY3dkczJNWjNwcXh6Z3doSGlUYVFnMEwrUFJsV000TytlbXAwOFVDM3QKcWF4dys4ZDNYVjgzSGZadHJFRTJqQkd5V0l5dHFzWlBuS2haa3VhdkN1NWkzUlNLVG1CcUw3cGF6TDZGRXhEeAo4OTU2aWdPR3hXbkhvRE1yKzRla1ZQZzExd0tCZ1FES09MZDRpd20xTlpHWHA3K21TVVpsVnM4a0J1aysyRGNhCjBvUWdKaEJ4M2FPZmRESTU0Q0Zqb0ZQUytBZjc2T3FSMUlNYWl5a1BMWFVoZzBsdHBac3VJWlBSTFFmdUpuK20KajhXbGFWT1NDeFoxMjkvKzZ1RGZwTUR1WFVZeW9wNS9QK0RJSzJhY2NGTWlzTHRHLzZncEJWN2ZBU2QxazQ4RQpRUWNOYlBCYmlRS0JnUUNDZXBmRVZhOVRndXoxa00rc2dtYVdRYnFxN0Qvbk90N3RmRHZseUUvYUxncmpPbFQwCkxnRDhod2U1U2R2SW5tM1lSeHdBK0RSY0xnWG43WFZSRDdNQVZwZExzcFAyeFZwenc3bUgzK1gzanRLaFpGZ1EKbmJFZFM5TVdiSU55cmZGcysvbEIvSXNCcWVjbGZscVdTaWt2VktmbVVCNjlyOU9laDFVSUpRSkNxd0tCZ0FveQpyQVgzTlFrZlozVTNiM0hLVmpOOEdqd2Q0UnRiT2dRdlE1eC9idXJmRzRaS0ROSmdYQzZ6QWljc2ZQS1dQMllWClNudEhNMDNoby91SnJHVk1LYlE4MjBCOFBkOGpyK0pOYzlFd3E1YzgyZWdkcTRFbWhTcWlHMXlwOVlWT01DSUkKcmFSS2xBVWxvUHVwMy9mbm9xcFc2LzdoQndWbDZKdDFVQTY4Uks3SkFvR0FXL0ZkSVk0Y3djMEZHMjN6dWYvUApTZTNESktTZ3BYSlA2ZDZ2SjRDNnAvRDcwNytUQ1JpWVBQMnJ4QlpDRk1ZczJ2M01BN2lvM0lpV3VjOStabHFMCkxWbk4zNEQ1dGZhMExnQzArVFZtOTNEZHE2SFd1WWdYMUx0MzFXeFhMMkpXREpmazcxc3prNW51NGRkSTNkdnAKU0ducTB1UjRlR0IxK2ZRNHluNlgydkk9Ci0tLS0tRU5EIFBSSVZBVEUgS0VZLS0tLS0K"
    uri = "/api/v1/system/ca"
    get_tests = [{"name": "Read system CAs"}]
    post_tests = [
        {
            "name": "Import CA without key",
            "payload": {
                "method": "import",
                "crt": crt,
                "descr": "CA_NO_KEY",
                "randomserial": True
            },
        },
        {
            "name": "Import CA with key",
            "payload": {
                "method": "import",
                "crt": crt,
                "prv": key,
                "descr": "CA_WITH_KEY",
                "trust": True
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
            "payload": {"method": "INVALID_METHOD"}
        },
        {
            "name": "Check description requirement",
            "status": 400,
            "return": 1002,
            "payload": {"method": "import"}
        },
        {
            "name": "Check certificate requirement with import method",
            "status": 400,
            "return": 1003,
            "payload": {"method": "import", "descr": "TestCA"}
        },
        {
            "name": "Check certificate key matching with import method",
            "status": 400,
            "return": 1004,
            "payload": {"method": "import", "descr": "TestCA", "crt": crt, "prv": "INVALID KEY"}
        },
        {
            "name": "Check certificate serial number minimum constraint with import method",
            "status": 400,
            "return": 1033,
            "payload": {"method": "import", "descr": "TestCA", "crt": crt, "serial": 0}
        },

    ]
    delete_tests = [
        {
            "name": "Delete CA certificate without key",
            "payload": {"descr": "CA_NO_KEY"}
        },
        {
            "name": "Delete CA certificate with key",
            "payload": {"descr": "CA_WITH_KEY"}
        },
        {
            "name": "Check deletion of non-existing CA",
            "status": 400,
            "return": 1009,
            "payload": {"descr": "INVALID"}
        },
        # TODO: add test to check that CAs in use cannot be deleted
    ]

APIUnitTestSystemCA()
