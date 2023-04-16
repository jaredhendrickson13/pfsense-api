# Copyright 2023 Jared Hendrickson
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

"""Script used to test the /api/v1/firewall/traffic_shaper/limiter endpoint."""
import e2e_test_framework

# This test requires lots of field validation hence the length of the module.
# pylint: disable=too-many-lines


class APIE2ETestFirewallTrafficShaperLimiter(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/traffic_shaper/limiter endpoint."""
    uri = "/api/v1/firewall/traffic_shaper/limiter"

    get_privileges = ["page-all", "page-firewall-trafficshaper-limiter"]
    post_privileges = ["page-all", "page-firewall-trafficshaper-limiter"]
    delete_privileges = ["page-all", "page-firewall-trafficshaper-limiter"]

    get_tests = [{"name": "Read all traffic shaper limiters"}]
    post_tests = [
        {
            "name": "Create firewall traffic shaper limiter",
            "req_data": {
                "name": "Test_Limiter",
                "bandwidth": [{"bw": 100, "bwscale": "Mb"}],
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
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
            "name": "Check name requirement",
            "status": 400,
            "return": 4167
        },
        {
            "name": "Check name character validation",
            "status": 400,
            "return": 4168,
            "req_data": {
                "name": "THIS NAME IS INVALID!"
            }
        },
        {
            "name": "Check name character length minimum constraint",
            "status": 400,
            "return": 4168,
            "req_data": {
                "name": ""
            }
        },
        {
            "name": "Check name character length maximum constraint",
            "status": 400,
            "return": 4169,
            "req_data": {
                "name": "This_name_is_too-long_for-the_system_to_handle"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 4170,
            "req_data": {
                "name": "Test_Limiter"
            }
        },
        {
            "name": "Check bandwidth requirement",
            "status": 400,
            "return": 4207,
            "req_data": {
                "name": "New_Limiter"
            }
        },
        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4208,
            "req_data": {
                "name": "New_Limiter",
                "bandwidth": []
            }
        },
        {
            "name": "Check bandwidth type constraint",
            "status": 400,
            "return": 4208,
            "req_data": {
                "name": "New_Limiter",
                "bandwidth": ""
            }
        },
        {
            "name": "Check mask with invalid option",
            "status": 400,
            "return": 4171,
            "req_data": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "mask": "INVALID"
            }
        },
        {
            "name": "Check bit mask minimum constraint",
            "status": 400,
            "return": 4172,
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}]
            }
        },
        {
            "name": "Check AQM with invalid choice",
            "status": 400,
            "return": 4114,
            "req_data": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "INVALID"
            }
        },
        {
            "name": "Check scheduler requirement",
            "status": 400,
            "return": 4113,
            "req_data": {
                "name": "New_Limiter",
                "bandwidth": [{"bw": 10000, "bwscale": "Kb"}],
                "aqm": "droptail"
            }
        },
        {
            "name": "Check scheduler with invalid choice",
            "status": 400,
            "return": 4114,
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
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
            "req_data": {
                "name": "Test_Limiter"
            }
        }
    ]


APIE2ETestFirewallTrafficShaperLimiter()
