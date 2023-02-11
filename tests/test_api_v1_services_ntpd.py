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
"""Class used to test the /api/v1/services/ntpd/restart endpoint."""
import e2e_test_framework


class APIE2ETestServicesNTPd(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/ntpd endpoint."""
    uri = "/api/v1/services/ntpd"

    get_privileges = ["page-all", "page-services-ntpd"]
    put_privileges = ["page-all", "page-services-ntpd"]

    get_tests = [{"name": "Read the NTPd configuration"}]
    put_tests = [
        {
            "name": "Update the NTPd configuration to enable all options",
            "post_test_callable": "is_ntpd_updated",
            "resp_time": 3,
            "req_data": {
                "interface": ["em0", "em1", "lo0"],
                "orphan": 15,
                "timeservers": [
                    {"timeserver": "1.pfsense.ntp.pool.org", "ispool": True},
                    {"timeserver": "2.pfsense.ntp.pool.org", "ispool": True},
                    {"timeserver": "1.pfsense.ntp.pool.org", "ispool": True},
                    {"timeserver": "time2.google.com", "ispool": False}
                ],
                "logpeer": True,
                "logsys": True,
                "clockstats": True,
                "peerstats": True,
                "loopstats": True,
                "statsgraph": True,
                "leapsec": "Test leap configuration"
            },
        },
        {
            "name": "Update the NTPd configuration to disable all options",
            "resp_time": 3,
            "req_data": {
                "interface": ["wan", "lo0"],
                "orphan": 12,
                "timeservers": [{"timeserver": "ntp.pool.org", "ispool": True}],
                "logpeer": False,
                "logsys": False,
                "clockstats": False,
                "peerstats": False,
                "loopstats": False,
                "statsgraph": False,
                "leapsec": ""
            },
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 2049,
            "req_data": {"interface": ["INVALID"]}
        },
        {
            "name": "Check timeservers maximum constraint",
            "status": 400,
            "return": 2052,
            "req_data": {"timeservers": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]}
        },
        {
            "name": "Check orphan minimum constraint",
            "status": 400,
            "return": 2048,
            "req_data": {"orphan": 0}
        },
        {
            "name": "Check orphan maximum constraint",
            "status": 400,
            "return": 2048,
            "req_data": {"orphan": 16}
        }
    ]

    def is_ntpd_updated(self):
        """Checks if the NTP configuration was updated properly"""
        # Many levels of logic require to test various configuration values/areas
        # pylint: disable=too-many-branches

        # Local variables
        ntp_conf = self.pfsense_shell("cat /var/etc/ntpd.conf")
        leap_conf = self.pfsense_shell("cat /var/db/leap-seconds")
        req_data = self.last_request.get("req_data", {})

        # Ensure each interface is present in the NTP configuration
        for iface in req_data.get("interface", []):
            if f"interface listen {iface}" not in ntp_conf:
                raise AssertionError(f"Expected interface '{iface}' to be present in /var/etc/ntpd.conf")

        # Ensure each timeserver is present in the NTP configuration
        for timeserver in req_data.get("timeservers", []):
            # Set the type to pool if a pool, otherwise consider it a server
            ts_type = "pool" if timeserver.get("ispool", False) else "server"
            ts_host = timeserver.get('timeserver')

            # Ensure the timeserver is properly set
            if f"{ts_type} {ts_host}" not in ntp_conf:
                raise AssertionError(f"Expected timeserver '{ts_host}' with type `{ts_type}' in /var/etc/ntpd.conf")

        # Ensure orphan is properly set in the NTP configuration
        if "orphan" in req_data and f"orphan {req_data.get('orphan')}" not in ntp_conf:
            raise AssertionError(f"Expected orphan to be set to {req_data.get('orphan')} in /var/etc/ntpd.conf")

        # Ensure the logpeer option is properly set
        if "logpeer" in req_data:
            # If log peer is enabled, check for the +peerall option
            if req_data.get("logpeer") and "+peerall" not in ntp_conf:
                raise AssertionError("Expected +peerall to be present in /var/etc/ntpd.conf")

        # Ensure the logsys option is properly set
        if "logsys" in req_data:
            # If log sys is enabled, check for the +sysall option
            if req_data.get("logsys") and "+sysall" not in ntp_conf:
                raise AssertionError("Expected +sysall to be present in /var/etc/ntpd.conf")

        # Ensure logging options are properly set
        for log_opt in ["clockstats", "loopstats", "peerstats"]:
            if log_opt in req_data:
                # If log option is enabled, check for the option in the configuration
                if req_data.get(log_opt) and log_opt not in ntp_conf:
                    raise AssertionError(f"Expected {log_opt} to be present in /var/etc/ntpd.conf")

        # Ensure leapsec is properly set in config
        if req_data.get("leapsec"):
            # Ensure leapsec db file is present
            if leap_conf != req_data.get("leapsec"):
                raise AssertionError("Expected leapsec to be set in /var/db/leap-seconds")

            # Ensure leapsec db is reference in ntp conf
            if "leapfile /var/db/leap-seconds" not in ntp_conf:
                raise AssertionError("Expected leapfile to be set in /var/etc/ntpd.conf")


APIE2ETestServicesNTPd()
