# Copyright 2021 Jared Hendrickson
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

import unit_test_framework

class APIUnitTestFirewallTrafficShaper(unit_test_framework.APIUnitTest):
    uri = "/api/v1/firewall/traffic_shaper/limiter"
    get_tests = [{"name": "Read all traffic shaper limiters"}]
    post_tests = [
        {
            "name": "Create firewall traffic shaper limiter",
            "payload": {
                "name": "Test_Limiter",
                "bandwidth": [{"bw": 100, "bwscale": "Mb"}],
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "Unit test",
                "aqm": "codel",
                "sched": "fq_pie",
                "delay": 1,
                "plr": 0.01,
                "buckets": 16,
                "apply": True
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4167
        },
        {
            "name": "Check name character validation",
            "status": 400,
            "return": 4168,
            "payload": {
                "name": "THIS NAME IS INVALID!"
            }
        },
        {
            "name": "Check name character length minimum constraint",
            "status": 400,
            "return": 4168,
            "payload": {
                "name": ""
            }
        },
        {
            "name": "Check name character length maximum constraint",
            "status": 400,
            "return": 4169,
            "payload": {
                "name": "This_name_is_too-long_for-the_system_to_handle"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 4170,
            "payload": {
                "name": "Test_Limiter"
            }
        },
        {
            "name": "Check bandwidth requirement",
            "status": 400,
            "return": 4207,
            "payload": {
                "name": "New_Limiter"
            }
        },
        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4208,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": []
            }
        },
        {
            "name": "Check bandwidth type constraint",
            "status": 400,
            "return": 4208,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": ""
            }
        },
        {
            "name": "Check mask with invalid option",
            "status": 400,
            "return": 4171,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "INVALID"
            }
        },
        {
            "name": "Check bit mask minimum constraint",
            "status": 400,
            "return": 4172,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "dstaddress",
                "maskbits": 0
            }
        },
        {
            "name": "Check bit mask maximum constraint",
            "status": 400,
            "return": 4172,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "dstaddress",
                "maskbits": 33
            }
        },
        {
            "name": "Check bit mask type constraint",
            "status": 400,
            "return": 4172,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "dstaddress",
                "maskbits": "INVALID"
            }
        },
        {
            "name": "Check IPV6 bit mask minimum constraint",
            "status": 400,
            "return": 4173,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "dstaddress",
                "maskbitsv6": 0
            }
        },
        {
            "name": "Check IPV6 bit mask maximum constraint",
            "status": 400,
            "return": 4173,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "dstaddress",
                "maskbitsv6": 129
            }
        },
        {
            "name": "Check IPV6 bit mask type constraint",
            "status": 400,
            "return": 4173,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "dstaddress",
                "maskbitsv6": "INVALID"
            }
        },
        {
            "name": "Check AQM requirement",
            "status": 400,
            "return": 4174,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}]
            }
        },
        {
            "name": "Check AQM with invalid choice",
            "status": 400,
            "return": 4114,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "INVALID"
            }
        },
        {
            "name": "Check scheduler requirement",
            "status": 400,
            "return": 4113,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail"
            }
        },
        {
            "name": "Check scheduler with invalid choice",
            "status": 400,
            "return": 4114,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "INVALID"
            }
        },
        {
            "name": "Check ECN with invalid AQM or scheduler",
            "status": 400,
            "return": 4176,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "ecn": True
            }
        },
        {
            "name": "Check queue limit minimum constraint",
            "status": 400,
            "return": 4120,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "qlimit": 0
            }
        },
        {
            "name": "Check queue limit type constraint",
            "status": 400,
            "return": 4120,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "qlimit": "INVALID"
            }
        },
        {
            "name": "Check delay minimum constraint",
            "status": 400,
            "return": 4177,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "delay": -1
            }
        },
        {
            "name": "Check delay maximum constraint",
            "status": 400,
            "return": 4177,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "delay": 10001
            }
        },
        {
            "name": "Check delay type constraint",
            "status": 400,
            "return": 4177,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "delay": "INVALID"
            }
        },
        {
            "name": "Check packet loss rate minimum constraint",
            "status": 400,
            "return": 4178,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "plr": -1
            }
        },
        {
            "name": "Check packet loss maximum constraint",
            "status": 400,
            "return": 4178,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "plr": 1.1
            }
        },
        {
            "name": "Check packet loss type constraint",
            "status": 400,
            "return": 4178,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "plr": "INVALID"
            }
        },
        {
            "name": "Check buckets minimum constraint",
            "status": 400,
            "return": 4179,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "buckets": 15
            }
        },
        {
            "name": "Check buckets maximum constraint",
            "status": 400,
            "return": 4179,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "buckets": 65536
            }
        },
        {
            "name": "Check buckets type constraint",
            "status": 400,
            "return": 4179,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail",
                "sched": "fifo",
                "buckets": "INVALID"
            }
        },
        {
            "name": "Check CoDel target parameter minimum constraint",
            "status": 400,
            "return": 4180,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "codel",
                "sched": "fifo",
                "param_codel_target": -1
            }
        },
        {
            "name": "Check CoDel target parameter type constraint",
            "status": 400,
            "return": 4180,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "codel",
                "sched": "fifo",
                "param_codel_target": "INVALID"
            }
        },
        {
            "name": "Check CoDel interval parameter minimum constraint",
            "status": 400,
            "return": 4181,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "codel",
                "sched": "fifo",
                "param_codel_interval": -1
            }
        },
        {
            "name": "Check CoDel interval parameter type constraint",
            "status": 400,
            "return": 4181,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "codel",
                "sched": "fifo",
                "param_codel_interval": "INVALID"
            }
        },
        {
            "name": "Check PIE target parameter minimum constraint",
            "status": 400,
            "return": 4182,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_target": -1
            }
        },
        {
            "name": "Check PIE target parameter type constraint",
            "status": 400,
            "return": 4182,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_target": "INVALID"
            }
        },
        {
            "name": "Check PIE tupdate parameter minimum constraint",
            "status": 400,
            "return": 4183,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_tupdate": -1
            }
        },
        {
            "name": "Check PIE tupdate parameter type constraint",
            "status": 400,
            "return": 4183,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_tupdate": "INVALID"
            }
        },
        {
            "name": "Check PIE alpha parameter minimum constraint",
            "status": 400,
            "return": 4184,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_alpha": -1
            }
        },
        {
            "name": "Check PIE alpha parameter type constraint",
            "status": 400,
            "return": 4184,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_alpha": "INVALID"
            }
        },
        {
            "name": "Check PIE beta parameter minimum constraint",
            "status": 400,
            "return": 4185,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_beta": -1
            }
        },
        {
            "name": "Check PIE beta parameter type constraint",
            "status": 400,
            "return": 4185,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_beta": "INVALID"
            }
        },
        {
            "name": "Check PIE max_burst parameter minimum constraint",
            "status": 400,
            "return": 4186,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_max_burst": -1
            }
        },
        {
            "name": "Check PIE max_burst parameter type constraint",
            "status": 400,
            "return": 4186,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_max_burst": "INVALID"
            }
        },
        {
            "name": "Check PIE max_ecnth parameter minimum constraint",
            "status": 400,
            "return": 4187,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_max_ecnth": -1
            }
        },
        {
            "name": "Check PIE max_ecnth parameter type constraint",
            "status": 400,
            "return": 4187,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "pie",
                "sched": "fifo",
                "param_pie_max_ecnth": "INVALID"
            }
        },
        {
            "name": "Check RED w_g parameter minimum constraint",
            "status": 400,
            "return": 4188,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_w_q": -1
            }
        },
        {
            "name": "Check RED w_g parameter type constraint",
            "status": 400,
            "return": 4188,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_w_q": "INVALID"
            }
        },
        {
            "name": "Check RED min_th parameter minimum constraint",
            "status": 400,
            "return": 4189,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_min_th": -1
            }
        },
        {
            "name": "Check RED min_th parameter type constraint",
            "status": 400,
            "return": 4189,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_min_th": "INVALID"
            }
        },
        {
            "name": "Check RED max_th parameter minimum constraint",
            "status": 400,
            "return": 4190,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_max_th": -1
            }
        },
        {
            "name": "Check RED max_th parameter type constraint",
            "status": 400,
            "return": 4190,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_max_th": "INVALID"
            }
        },
        {
            "name": "Check RED max_p parameter minimum constraint",
            "status": 400,
            "return": 4191,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_max_p": -1
            }
        },
        {
            "name": "Check RED max_p parameter type constraint",
            "status": 400,
            "return": 4191,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "red",
                "sched": "fifo",
                "param_red_max_p": "INVALID"
            }
        },
        {
            "name": "Check GRED w_q parameter minimum constraint",
            "status": 400,
            "return": 4192,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_w_q": -1
            }
        },
        {
            "name": "Check GRED w_q parameter type constraint",
            "status": 400,
            "return": 4192,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_w_q": "INVALID"
            }
        },
        {
            "name": "Check GRED min_th parameter minimum constraint",
            "status": 400,
            "return": 4193,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_min_th": -1
            }
        },
        {
            "name": "Check GRED min_th parameter type constraint",
            "status": 400,
            "return": 4193,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_min_th": "INVALID"
            }
        },
        {
            "name": "Check GRED max_th parameter minimum constraint",
            "status": 400,
            "return": 4194,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_max_th": -1
            }
        },
        {
            "name": "Check GRED max_th parameter type constraint",
            "status": 400,
            "return": 4194,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_max_th": "INVALID"
            }
        },
        {
            "name": "Check GRED max_p parameter minimum constraint",
            "status": 400,
            "return": 4195,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_max_p": -1
            }
        },
        {
            "name": "Check GRED max_p parameter type constraint",
            "status": 400,
            "return": 4195,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fifo",
                "param_gred_max_p": "INVALID"
            }
        },
        {
            "name": "Check FQ_CoDel target parameter minimum constraint",
            "status": 400,
            "return": 4196,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_target": -1
            }
        },
        {
            "name": "Check FQ_CoDel target parameter type constraint",
            "status": 400,
            "return": 4196,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_target": "INVALID"
            }
        },
        {
            "name": "Check FQ_CoDel interval parameter minimum constraint",
            "status": 400,
            "return": 4197,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_interval": -1
            }
        },
        {
            "name": "Check FQ_CoDel interval parameter type constraint",
            "status": 400,
            "return": 4197,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_interval": "INVALID"
            }
        },
        {
            "name": "Check FQ_CoDel quantum parameter minimum constraint",
            "status": 400,
            "return": 4198,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_quantum": -1
            }
        },
        {
            "name": "Check FQ_CoDel quantum parameter type constraint",
            "status": 400,
            "return": 4198,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_quantum": "INVALID"
            }
        },
        {
            "name": "Check FQ_CoDel limit parameter minimum constraint",
            "status": 400,
            "return": 4199,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_limit": -1
            }
        },
        {
            "name": "Check FQ_CoDel limit parameter type constraint",
            "status": 400,
            "return": 4199,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_limit": "INVALID"
            }
        },
        {
            "name": "Check FQ_CoDel flows parameter minimum constraint",
            "status": 400,
            "return": 4200,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_flows": -1
            }
        },
        {
            "name": "Check FQ_CoDel flows parameter type constraint",
            "status": 400,
            "return": 4200,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_codel",
                "param_fq_codel_flows": "INVALID"
            }
        },
        {
            "name": "Check FQ_PIE target parameter minimum constraint",
            "status": 400,
            "return": 4201,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_target": -1
            }
        },
        {
            "name": "Check FQ_PIE target parameter type constraint",
            "status": 400,
            "return": 4201,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_target": "INVALID"
            }
        },
        {
            "name": "Check FQ_PIE tupdate parameter minimum constraint",
            "status": 400,
            "return": 4202,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_tupdate": -1
            }
        },
        {
            "name": "Check FQ_PIE tupdate parameter type constraint",
            "status": 400,
            "return": 4202,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_tupdate": "INVALID"
            }
        },
        {
            "name": "Check FQ_PIE alpha parameter minimum constraint",
            "status": 400,
            "return": 4203,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_alpha": -1
            }
        },
        {
            "name": "Check FQ_PIE alpha parameter type constraint",
            "status": 400,
            "return": 4203,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_alpha": "INVALID"
            }
        },
        {
            "name": "Check FQ_PIE beta parameter minimum constraint",
            "status": 400,
            "return": 4204,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_beta": -1
            }
        },
        {
            "name": "Check FQ_PIE beta parameter type constraint",
            "status": 400,
            "return": 4204,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_beta": "INVALID"
            }
        },
        {
            "name": "Check FQ_PIE max_burst parameter minimum constraint",
            "status": 400,
            "return": 4205,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_max_burst": -1
            }
        },
        {
            "name": "Check FQ_PIE max_burst parameter type constraint",
            "status": 400,
            "return": 4205,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_max_burst": "INVALID"
            }
        },
        {
            "name": "Check FQ_PIE max_ecnth parameter minimum constraint",
            "status": 400,
            "return": 4206,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_max_ecnth": -1
            }
        },
        {
            "name": "Check FQ_PIE max_ecnth parameter type constraint",
            "status": 400,
            "return": 4206,
            "payload": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "gred",
                "sched": "fq_pie",
                "param_fq_pie_max_ecnth": "INVALID"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete limiter",
            "payload": {
                "name": "Test_Limiter"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4167
        },
        {
            "name": "Check deleting non-existing limiter",
            "status": 400,
            "return": 4209,
            "payload": {
                "name": "Test_Limiter"
            }
        }
    ]


APIUnitTestFirewallTrafficShaper()