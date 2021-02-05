<?php
//   Copyright 2021 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIServicesNTPdUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-ntpd"];
        $this->change_note = "Modified NTP settings via API";
        $this->validated_data = $this->config["ntpd"];
    }

    public function action() {
        # Save our updated configuration
        $this->config["ntpd"] = $this->validated_data;
        $this->write_config();

        # Apply our change and return the response. Include all configured timeservers from system > timeservers as it
        # is need to provide the full context of the configuration
        system_ntp_configure();
        $this->validated_data["timeservers"] = explode(" ", $this->config["system"]["timeservers"]);;
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_interface();
        $this->__validate_orphan();
        $this->__validate_logsys();
        $this->__validate_logpeer();
        $this->__validate_clockstats();
        $this->__validate_loopstats();
        $this->__validate_peerstats();
        $this->__validate_statsgraph();
        $this->__validate_leapsec();
    }

    private function __validate_interface() {
        # Check for our optional 'interface' payload value
        if (isset($this->initial_data['interface'])) {
            $this->validated_data["interface"] = [];

            # Loop through each requested interface and ensure it is valid
            foreach ($this->initial_data["interface"] as $if) {
                # Convert the interface to the pfSense interface ID if it exists, otherwise leave original input.
                $if = (APITools\get_pfsense_if_id($if)) ? APITools\get_pfsense_if_id($if) : $if;

                # Check that this interface exists
                if (in_array($if, $this->__get_interfaces())) {
                    $this->validated_data["interface"][] = $if;
                } else {
                    $this->errors[] = APIResponse\get(2047);
                }
            }

            # Convert value to internal XML value
            $this->validated_data["interface"] = implode(",", $this->validated_data["interface"]);
        }
    }

    private function __validate_timeservers() {
        # Check for our optional 'timeservers' payload value
        if (isset($this->initial_data["timeservers"])) {
            # Loop through each requested timeserver and ensure it is valid
            foreach ($this->initial_data["timeservers"] as $timeserver) {
                # Use the APIServicesNTPdTimeServerCreate model to validate each requested time server. This essentially
                # creates a nested/internal API call to validate/create NTPd timeservers
                $cts = new APIServicesNTPdTimeServerCreate();
                $cts->initial_data = $timeserver;
                $cts->validate_payload();

                # If errors were encountered while validating the payload, save those errors to this models error field
                if (!empty($cts->errors)) {
                    $this->errors = $this->errors + $cts->errors;
                }
            }
        }
    }

    private function __validate_orphan() {
        # Check for our optional 'orphan' payload value
        if (isset($this->initial_data["orphan"])) {
            # Ensure the value is between 1 and 15. Otherwise return an error.
            if ($this->initial_data['orphan'] >= 1 and $this->initial_data['orphan'] <= 15) {
                $this->validated_data["orphan"] = $this->initial_data['orphan'];
            } else {
                $this->errors[] = APIResponse\get(2046);
            }
        }
    }

    private function __validate_logsys() {
        # Check for our optional 'logsys' payload value
        if ($this->initial_data['logsys'] === true) {
            $this->validated_data["logsys"] = "yes";
        } elseif ($this->initial_data['logsys'] === false) {
            unset($this->validated_data["logsys"]);
        }
    }

    private function __validate_logpeer() {
        # Check for our optional 'logpeer' payload value
        if ($this->initial_data['logpeer'] === true) {
            $this->validated_data["logpeer"] = "yes";
        } elseif ($this->initial_data['logpeer'] === false) {
            unset($this->validated_data["logpeer"]);
        }
    }

    private function __validate_clockstats() {
        # Check for our optional 'clockstats' payload value
        if ($this->initial_data['clockstats'] === true) {
            $this->validated_data["clockstats"] = "yes";
        } elseif ($this->initial_data['clockstats'] === false) {
            unset($this->validated_data["clockstats"]);
        }
    }

    private function __validate_loopstats() {
        # Check for our optional 'loopstats' payload value
        if ($this->initial_data['loopstats'] === true) {
            $this->validated_data["loopstats"] = "yes";
        } elseif ($this->initial_data['loopstats'] === false) {
            unset($this->validated_data["loopstats"]);
        }
    }

    private function __validate_peerstats() {
        # Check for our optional 'peerstats' payload value
        if ($this->initial_data['peerstats'] === true) {
            $this->validated_data["peerstats"] = "yes";
        } elseif ($this->initial_data['peerstats'] === false) {
            unset($this->validated_data["peerstats"]);
        }
    }

    private function __validate_statsgraph() {
        # Check for our optional 'statsgraph' payload value
        if ($this->initial_data['statsgraph'] === true) {
            $this->validated_data["statsgraph"] = "yes";
        } elseif ($this->initial_data['statsgraph'] === false) {
            unset($this->validated_data["statsgraph"]);
        }
    }

    private function __validate_leapsec() {
        # Check for our optional 'leapsec' payload value
        if (isset($this->initial_data["leapsec"])) {
            # XML configuration expects the leap seconds configuration in a base64 format. Convert before saving.
            $this->validated_data["leapsec"] = base64_encode(strval($this->initial_data["leapsec"]));
        }
    }

    private function __get_interfaces() {
        # Local variables
        $if_raw = get_configured_interface_with_descr();
        $if_list = ["lo0"];

        # Loop through each configured interface but only capture interfaces with valid IP addresses
        foreach ($if_raw as $if => $if_descr) {
            if (is_ipaddr(get_interface_ip($if)) or is_ipaddrv6(get_interface_ipv6($if))) {
                $if_list[] = $if;
            }
        }

        return $if_list;
    }
}