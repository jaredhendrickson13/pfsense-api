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
"""Script used to test the /api/v1/firewall/traffic_shaper/limiter/queue endpoint."""
import e2e_test_framework


class APIE2ETestFirewallTrafficShaperLimiterQueue(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/traffic_shaper/limiter/queue endpoint."""
    uri = "/api/v1/firewall/traffic_shaper/limiter/queue"
    post_tests = [
        {
            "name": "Create parent firewall traffic shaper limiter",
            "uri": "/api/v1/firewall/traffic_shaper/limiter",
            "payload": {
                "name": "Test_Limiter",
                "bandwidth": [{"bw": 100, "bwscale": "Mb"}],
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
                "aqm": "codel",
                "sched": "fq_pie",
                "qlimit": 7000,
                "delay": 1,
                "plr": 0.01,
                "buckets": 16,
                "ecn": True,
                "apply": True
            }
        },
        {
            "name": "Create firewall traffic shaper limiter queue",
            "payload": {
                "limiter": "Test_Limiter",
                "name": "Test_Queue",
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
                "aqm": "codel",
                "qlimit": 7000,
                "weight": 1,
                "plr": 0.01,
                "buckets": 16,
                "ecn": True,
                "apply": True
            }
        },
        {
            "name": "Create another firewall traffic shaper limiter queue",
            "payload": {
                "limiter": "Test_Limiter",
                "name": "Test_Queue2",
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
                "aqm": "codel",
                "qlimit": 7000,
                "weight": 1,
                "plr": 0.01,
                "buckets": 16,
                "ecn": True,
                "apply": True
            }
        },
        {
            "name": "Check limiter requirement",
            "status": 400,
            "return": 4218
        },
        {
            "name": "Check non-existent parent limiter",
            "status": 400,
            "return": 4219,
            "payload": {
                "limiter": "INVALID"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4167,
            "payload": {
                "limiter": "Test_Limiter"
            }
        },
        {
            "name": "Check name character validation",
            "status": 400,
            "return": 4168,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "THIS NAME IS INVALID!"
            }
        },
        {
            "name": "Check name character length minimum constraint",
            "status": 400,
            "return": 4168,
            "payload": {
                "limiter": "Test_Limiter",
                "name": ""
            }
        },
        {
            "name": "Check name character length maximum constraint",
            "status": 400,
            "return": 4169,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "This_name_is_too-long_for-the_system_to_handle"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 4170,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "Test_Queue"
            }
        },
        {
            "name": "Check name unique from parents constraint",
            "status": 400,
            "return": 4170,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "Test_Limiter"
            }
        },
        {
            "name": "Check mask with invalid option",
            "status": 400,
            "return": 4171,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "INVALID"
            }
        },
        {
            "name": "Check bit mask minimum constraint",
            "status": 400,
            "return": 4172,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "dstaddress",
                "maskbits": 0
            }
        },
        {
            "name": "Check bit mask maximum constraint",
            "status": 400,
            "return": 4172,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "dstaddress",
                "maskbits": 33
            }
        },
        {
            "name": "Check bit mask type constraint",
            "status": 400,
            "return": 4172,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "dstaddress",
                "maskbits": "INVALID"
            }
        },
        {
            "name": "Check IPV6 bit mask minimum constraint",
            "status": 400,
            "return": 4173,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "dstaddress",
                "maskbitsv6": 0
            }
        },
        {
            "name": "Check IPV6 bit mask maximum constraint",
            "status": 400,
            "return": 4173,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "dstaddress",
                "maskbitsv6": 129
            }
        },
        {
            "name": "Check IPV6 bit mask type constraint",
            "status": 400,
            "return": 4173,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "mask": "dstaddress",
                "maskbitsv6": "INVALID"
            }
        },
        {
            "name": "Check AQM requirement",
            "status": 400,
            "return": 4174,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
            }
        },
        {
            "name": "Check AQM with invalid choice",
            "status": 400,
            "return": 4114,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "INVALID"
            }
        },
        {
            "name": "Check ECN with invalid AQM",
            "status": 400,
            "return": 4176,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "ecn": True
            }
        },
        {
            "name": "Check queue limit minimum constraint",
            "status": 400,
            "return": 4120,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "qlimit": 0
            }
        },
        {
            "name": "Check queue limit type constraint",
            "status": 400,
            "return": 4120,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "qlimit": "INVALID"
            }
        },
        {
            "name": "Check weight minimum constraint",
            "status": 400,
            "return": 4217,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "weight": 0
            }
        },
        {
            "name": "Check weight maximum constraint",
            "status": 400,
            "return": 4217,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "weight": 101
            }
        },
        {
            "name": "Check weight type constraint",
            "status": 400,
            "return": 4217,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "weight": "INVALID"
            }
        },
        {
            "name": "Check packet loss rate minimum constraint",
            "status": 400,
            "return": 4178,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "plr": -1
            }
        },
        {
            "name": "Check packet loss maximum constraint",
            "status": 400,
            "return": 4178,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "plr": 1.1
            }
        },
        {
            "name": "Check packet loss type constraint",
            "status": 400,
            "return": 4178,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "plr": "INVALID"
            }
        },
        {
            "name": "Check buckets minimum constraint",
            "status": 400,
            "return": 4179,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "buckets": 15
            }
        },
        {
            "name": "Check buckets maximum constraint",
            "status": 400,
            "return": 4179,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "buckets": 65536
            }
        },
        {
            "name": "Check buckets type constraint",
            "status": 400,
            "return": 4179,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "droptail",
                "buckets": "INVALID"
            }
        },
        {
            "name": "Check CoDel target parameter minimum constraint",
            "status": 400,
            "return": 4180,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "codel",
                "param_codel_target": -1
            }
        },
        {
            "name": "Check CoDel target parameter type constraint",
            "status": 400,
            "return": 4180,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "codel",
                "param_codel_target": "INVALID"
            }
        },
        {
            "name": "Check CoDel interval parameter minimum constraint",
            "status": 400,
            "return": 4181,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "codel",
                "param_codel_interval": -1
            }
        },
        {
            "name": "Check CoDel interval parameter type constraint",
            "status": 400,
            "return": 4181,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "codel",
                "param_codel_interval": "INVALID"
            }
        },
        {
            "name": "Check PIE target parameter minimum constraint",
            "status": 400,
            "return": 4182,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_target": -1
            }
        },
        {
            "name": "Check PIE target parameter type constraint",
            "status": 400,
            "return": 4182,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_target": "INVALID"
            }
        },
        {
            "name": "Check PIE tupdate parameter minimum constraint",
            "status": 400,
            "return": 4183,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_tupdate": -1
            }
        },
        {
            "name": "Check PIE tupdate parameter type constraint",
            "status": 400,
            "return": 4183,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_tupdate": "INVALID"
            }
        },
        {
            "name": "Check PIE alpha parameter minimum constraint",
            "status": 400,
            "return": 4184,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_alpha": -1
            }
        },
        {
            "name": "Check PIE alpha parameter type constraint",
            "status": 400,
            "return": 4184,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_alpha": "INVALID"
            }
        },
        {
            "name": "Check PIE beta parameter minimum constraint",
            "status": 400,
            "return": 4185,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_beta": -1
            }
        },
        {
            "name": "Check PIE beta parameter type constraint",
            "status": 400,
            "return": 4185,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_beta": "INVALID"
            }
        },
        {
            "name": "Check PIE max_burst parameter minimum constraint",
            "status": 400,
            "return": 4186,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_max_burst": -1
            }
        },
        {
            "name": "Check PIE max_burst parameter type constraint",
            "status": 400,
            "return": 4186,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_max_burst": "INVALID"
            }
        },
        {
            "name": "Check PIE max_ecnth parameter minimum constraint",
            "status": 400,
            "return": 4187,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_max_ecnth": -1
            }
        },
        {
            "name": "Check PIE max_ecnth parameter type constraint",
            "status": 400,
            "return": 4187,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "pie",
                "param_pie_max_ecnth": "INVALID"
            }
        },
        {
            "name": "Check RED w_g parameter minimum constraint",
            "status": 400,
            "return": 4188,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_w_q": -1
            }
        },
        {
            "name": "Check RED w_g parameter type constraint",
            "status": 400,
            "return": 4188,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_w_q": "INVALID"
            }
        },
        {
            "name": "Check RED min_th parameter minimum constraint",
            "status": 400,
            "return": 4189,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_min_th": -1
            }
        },
        {
            "name": "Check RED min_th parameter type constraint",
            "status": 400,
            "return": 4189,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_min_th": "INVALID"
            }
        },
        {
            "name": "Check RED max_th parameter minimum constraint",
            "status": 400,
            "return": 4190,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_max_th": -1
            }
        },
        {
            "name": "Check RED max_th parameter type constraint",
            "status": 400,
            "return": 4190,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_max_th": "INVALID"
            }
        },
        {
            "name": "Check RED max_p parameter minimum constraint",
            "status": 400,
            "return": 4191,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_max_p": -1
            }
        },
        {
            "name": "Check RED max_p parameter type constraint",
            "status": 400,
            "return": 4191,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "red",
                "param_red_max_p": "INVALID"
            }
        },
        {
            "name": "Check GRED w_q parameter minimum constraint",
            "status": 400,
            "return": 4192,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_w_q": -1
            }
        },
        {
            "name": "Check GRED w_q parameter type constraint",
            "status": 400,
            "return": 4192,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_w_q": "INVALID"
            }
        },
        {
            "name": "Check GRED min_th parameter minimum constraint",
            "status": 400,
            "return": 4193,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_min_th": -1
            }
        },
        {
            "name": "Check GRED min_th parameter type constraint",
            "status": 400,
            "return": 4193,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_min_th": "INVALID"
            }
        },
        {
            "name": "Check GRED max_th parameter minimum constraint",
            "status": 400,
            "return": 4194,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_max_th": -1
            }
        },
        {
            "name": "Check GRED max_th parameter type constraint",
            "status": 400,
            "return": 4194,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_max_th": "INVALID"
            }
        },
        {
            "name": "Check GRED max_p parameter minimum constraint",
            "status": 400,
            "return": 4195,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_max_p": -1
            }
        },
        {
            "name": "Check GRED max_p parameter type constraint",
            "status": 400,
            "return": 4195,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "New_Queue",
                "aqm": "gred",
                "param_gred_max_p": "INVALID"
            }
        }
    ]
    delete_tests = [
        {
            "name": "Delete queue",
            "payload": {
                "limiter": "Test_Limiter",
                "name": "Test_Queue"
            }
        },
        {
            "name": "Check limiter requirement",
            "status": 400,
            "return": 4218
        },
        {
            "name": "Check deleting non-existing limiter",
            "status": 400,
            "return": 4219,
            "payload": {
                "limiter": "INVALID"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4220,
            "payload": {
                "limiter": "Test_Limiter"
            }
        },
        {
            "name": "Check deleting non-existing queue",
            "status": 400,
            "return": 4221,
            "payload": {
                "limiter": "Test_Limiter",
                "name": "INVALID"
            }
        },
        {
            "name": "Delete parent limiter",
            "uri": "/api/v1/firewall/traffic_shaper/limiter",
            "payload": {
                "name": "Test_Limiter",
            }
        }
    ]


APIE2ETestFirewallTrafficShaperLimiterQueue()
