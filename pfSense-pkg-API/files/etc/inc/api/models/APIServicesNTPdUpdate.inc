<?php
//   Copyright 2023 Jared Hendrickson
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
        $this->change_note = "Modified NTP settings via API";
    }

    public function action() {
        # Save our updated configuration
        $this->set_config("ntpd", $this->validated_data);
        $this->write_config();
        $this->__validate_timeservers(true);    // Revalidate timeservers. This time create if valid.

        # Apply our change to the backend
        system_ntp_configure();

        # Update our return values and return the response. Get the updated NTPd configuration and add the timeservers
        # from system > timeservers to provide full context of the NTP configuration.
        $this->validated_data = $this->get_config("ntpd", []);
        $this->validated_data["timeservers"] = explode(" ", $this->get_config("system/timeservers", []));
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Pull the existing NTP configuration or default if it's empty
        if (!empty($this->get_config("ntpd", []))) {
            $this->validated_data = $this->get_config("ntpd");
        } else {
            $this->validated_data = [];
            $this->set_config("ntpd", []);
        }

        # Validate each NTP field
        $this->__validate_interface();
        $this->__validate_timeservers();
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
                    $this->errors[] = APIResponse\get(2049);
                }
            }

            # Convert value to internal XML value
            $this->validated_data["interface"] = implode(",", $this->validated_data["interface"]);
        }
    }

    private function __validate_timeservers($create=false) {
        # Check for our optional 'timeservers' payload value
        if (isset($this->initial_data["timeservers"])) {
            # Only proceed if 10 or less timeservers were specified.
            if (is_array($this->initial_data["timeservers"]) and count($this->initial_data["timeservers"]) <= 10) {
                # Assume default if empty array was passed in
                if (empty($this->initial_data["timeservers"])) {
                    $this->initial_data["timeservers"][] = ["timeserver"=>"pool.ntp.org", "ispool"=>true];
                }

                # Unset all existing timeservers.
                $this->del_config("system/timeservers");
                $this->del_config("ntpd/ispool");
                $this->del_config("ntpd/noselect");
                $this->del_config("ntpd/prefer");

                # Loop through each requested timeserver and ensure it is valid
                foreach ($this->initial_data["timeservers"] as $timeserver) {
                    # Use the APIServicesNTPdTimeServerCreate model to validate each requested time server. This
                    # essentiall creates a nested/internal API call to validate/create NTPd timeservers
                    $cts = new APIServicesNTPdTimeServerCreate();
                    $cts->initial_data = $timeserver;
                    $cts->validate_payload();

                    # If errors were encountered validating the payload, save those errors to this models error field
                    if (!empty($cts->errors)) {
                        $this->errors = $this->errors + $cts->errors;
                    } # Otherwise, if our created parameter is true, create this timeserver after validation
                    elseif ($create === true) {
                        $cts->action();
                    }
                }
            }
            # If our limit was exceeded return an error
            else {
                $this->errors[] = APIResponse\get(2052);
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
                $this->errors[] = APIResponse\get(2048);
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

    # Private method to create the validated timeservers
    private function __create_timeservers() {
        # Only proceed if timeservers were requested and no errors were found
        if (isset($this->initial_data["timeservers"]) and empty($this->create_timeserver->errors)) {
            $this->create_timeserver->action();
        }
    }
}
