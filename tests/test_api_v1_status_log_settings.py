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
"""Script used to test the /api/v1/status/log/settings endpoint."""
import e2e_test_framework


class APIE2ETestStatusLogSettings(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/status/log/settings endpoint."""
    uri = "/api/v1/status/log/settings"
    put_tests = [
        {
            "name": "Update log settings",
            "payload": {
                "format": "rfc3164",
                "reverse": True,
                "nentries": 5,
                "nologdefaultblock": True,
                "nologdefaultpass": True,
                "nologbogons": True,
                "nologprivatenets": True,
                "nolognginx": True,
                "rawfilter": True,
                "disablelocallogging": True,
                "logconfigchanges": True,
                "filterdescriptions": 0,
                "enable": True,
                "logfilesize": 100000,
                "rotatecount": 0,
                "logcompressiontype": "bzip2",
                "remoteserver": "127.0.0.1",
                "remoteserver2": "127.0.0.1:55555",
                "remoteserver3": "0000:0000:0000:0000:0000:0000:0000:0001",
                "sourceip": "any",
                "ipprotocol": "ipv4",
                "logall": True,
                "filter": True,
                "dhcp": True,
                "auth": True,
                "portalauth": True,
                "vpn": True,
                "dpinger": True,
                "hostapd": True,
                "system": True,
                "resolver": True,
                "ppp": True,
                "routing": True
            },
        },
        {
            "name": "Update log settings",
            "payload": {
                "format": "rfc5424",
                "reverse": False,
                "nentries": 200000,
                "nologdefaultblock": False,
                "nologdefaultpass": False,
                "nologbogons": False,
                "nologprivatenets": False,
                "nolognginx": False,
                "rawfilter": False,
                "disablelocallogging": False,
                "logconfigchanges": False,
                "filterdescriptions": 2,
                "enable": False,
                "logfilesize": 2147483648,
                "rotatecount": 99,
                "logcompressiontype": "gzip",
                "remoteserver": "",
                "remoteserver2": "",
                "remoteserver3": "0000:0000:0000:0000:0000:0000:0000:0001:55555",
                "sourceip": "WAN",
                "ipprotocol": "ipv6",
                "logall": False,
                "filter": False,
                "dhcp": False,
                "auth": False,
                "portalauth": False,
                "vpn": False,
                "dpinger": False,
                "hostapd": False,
                "system": False,
                "resolver": False,
                "ppp": False,
                "routing": False,
            },
        },
        {
            "name": "Check format is known validation",
            "delay": 8,    # Wait a while before starting this one, the previous tests restarts services
            "status": 400,
            "return": 8000,
            "payload": {
                "format": "INVALID"
            }
        },
        {
            "name": "Check nentries minimum constraint",
            "status": 400,
            "return": 8001,
            "payload": {
                "nentries": 4
            }
        },
        {
            "name": "Check nentries maximum constraint",
            "status": 400,
            "return": 8001,
            "payload": {
                "nentries": 200001
            }
        },
        {
            "name": "Check filterdescriptions option constraint",
            "status": 400,
            "return": 8002,
            "payload": {
                "filterdescriptions": 3
            }
        },
        {
            "name": "Check logfilesize minimum constraint",
            "status": 400,
            "return": 8003,
            "payload": {
                "logfilesize": 99999
            }
        },
        {
            "name": "Check logfilesize maximum constraint",
            "status": 400,
            "return": 8003,
            "payload": {
                "logfilesize": 2147483649
            }
        },
        {
            "name": "Check rotatecount minimum constraint",
            "status": 400,
            "return": 8004,
            "payload": {
                "rotatecount": -1
            }
        },
        {
            "name": "Check rotatecount maximum constraint",
            "status": 400,
            "return": 8004,
            "payload": {
                "rotatecount": 100
            }
        },
        {
            "name": "Check logcompressiontype option constraint",
            "status": 400,
            "return": 8005,
            "payload": {
                "logcompressiontype": "INVALID"
            }
        },
        {
            "name": "Check remoteserver IP or hostname constraint",
            "status": 400,
            "return": 8006,
            "payload": {
                "enable": True,
                "remoteserver": "@@@INVALID@@@"
            }
        },
        {
            "name": "Check remoteserver2 IP or hostname constraint",
            "status": 400,
            "return": 8006,
            "payload": {
                "enable": True,
                "remoteserver": "127.0.0.1",
                "remoteserver2": "@@@INVALID@@@"
            }
        },
        {
            "name": "Check remoteserver3 IP or hostname constraint",
            "status": 400,
            "return": 8006,
            "payload": {
                "enable": True,
                "remoteserver": "127.0.0.1",
                "remoteserver2": "127.0.0.2",
                "remoteserver3": "@@@INVALID@@@"
            }
        },
        {
            "name": "Check sourceip option constraint",
            "status": 400,
            "return": 8007,
            "payload": {
                "enable": True,
                "sourceip": "INVALID"
            }
        },
        {
            "name": "Check sourceip option constraint",
            "status": 400,
            "return": 8008,
            "payload": {
                "enable": True,
                "sourceip": "WAN",
                "ipprotocol": "INVALID"
            }
        }
    ]


APIE2ETestStatusLogSettings()
