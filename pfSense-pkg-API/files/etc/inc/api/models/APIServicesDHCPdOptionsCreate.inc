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


class APIServicesDHCPdOptionsCreate extends APIModel {
    private $__if_conf;
    private $__parent_model;

    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added DHCP options via API";
        $this->__if_conf = [];
        $this->__parent_model = new APIServicesDHCPdUpdate($this->initial_data);
    }

    public function action() {
        $next_id = $this->get_next_id("dhcpd/{$this->id}/numberoptions/item");
        $this->set_config("dhcpd/{$this->id}/numberoptions/item/{$next_id}", $this->validated_data);
        $this->write_config();
        $this->__parent_model->reload_dhcpd();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_interface();
        $this->__validate_number();
        $this->__validate_type();
        $this->__validate_value();
    }

    private function __validate_interface() {
        # Check for our required 'interface' payload value
        if (isset($this->initial_data["interface"])) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            # Check that interface exists
            if (is_string($this->initial_data["interface"])) {
                $this->__if_conf = $this->get_config("interfaces/{$this->initial_data["interface"]}", []);
                # Check that interface hosts a static network with multiple addresses
                if (isset($this->__if_conf["enable"]) and is_ipaddrv4($this->__if_conf["ipaddr"]) and $this->__if_conf["subnet"] <= 31) {
                    $this->id = $this->initial_data["interface"];
                    $this->__parent_model->init_config($this->id);
                    $this->validated_data = $this->get_config("dhcpd/{$this->id}", []);
                } else {
                    $this->errors[] = APIResponse\get(2019);
                }
            } else {
                $this->errors[] = APIResponse\get(2018);
            }
        } else {
            $this->errors[] = APIResponse\get(2017);
        }
    }

    # Must be public for nested API model invocations
    public function __validate_number() {
        # Check for our required number field
        if (isset($this->initial_data["number"])) {
            # Ensure the option number is between 1 and 254
            if (intval($this->initial_data["number"]) >= 1 and intval($this->initial_data["number"]) <= 254) {
                $this->validated_data["number"] = intval($this->initial_data["number"]);
            } else {
                $this->errors[] = APIResponse\get(2084);
            }
        } else {
            $this->errors[] = APIResponse\get(2083);
        }
    }

    # Must be public for nested API model invocations
    public function __validate_type() {
        # Local variables
        $TYPE_OPTS = [
            "text",
            "string",
            "boolean",
            "unsigned integer 8",
            "unsigned integer 16",
            "unsigned integer 32",
            "signed integer 8",
            "signed integer 16",
            "signed integer 32",
            "ip-address"
        ];

        # Check for our required type field
        if (isset($this->initial_data["type"])) {
            # Check if the requested type is a supported option type
            if (in_array(strtolower($this->initial_data["type"]), $TYPE_OPTS)) {
                $this->validated_data["type"] = strtolower($this->initial_data["type"]);
            } else {
                $this->errors[] = APIResponse\get(2086);
            }
        } else {
            $this->errors[] = APIResponse\get(2085);
        }
    }

    # Must be public for nested API model invocations
    public function __validate_value() {
        # Local variables
        $type = $this->validated_data["type"];
        $str_rgx_1 = '/^"[^"]*"$/';    // Regex to use when checking if string type value is enclosed in quotes
        $str_rgx_2 = '/^[0-9a-f]{2}(?:\:[0-9a-f]{2})*$/i';    // Regex to use when checking if string type value is hex
        $bool_opts = [true, false, "on", "off", "true", "false"];

        # Check for our required value field
        if (isset($this->initial_data["value"])) {
            # Validate this field when the type is set to 'text'
            if ($type === "text" and strstr($this->initial_data["value"], '"')) {
                $this->errors[] = APIResponse\get(2088);
            }
            # Validate this field when the type is set to 'string'
            elseif ($type === "string" and !preg_match($str_rgx_1, $this->initial_data["value"]) and !preg_match($str_rgx_2, $this->initial_data["value"])) {
                $this->errors[] = APIResponse\get(2089);
            }
            # Validate this field when the type is set to 'boolean'
            elseif ($type === "boolean" and !in_array($this->initial_data["value"], $bool_opts, true)) {
                $this->errors[] = APIResponse\get(2090);
            }
            # Validate this field when the type is set to 'unsigned integer 8'
            elseif ($type === "unsigned integer 8" and (!is_numeric($this->initial_data["value"]) or $this->initial_data["value"] < 0 or $this->initial_data["value"] > 255)) {
                $this->errors[] = APIResponse\get(2091);
            }
            # Validate this field when the type is set to 'unsigned integer 16'
            elseif ($type === "unsigned integer 16" and (!is_numeric($this->initial_data["value"]) or $this->initial_data["value"] < 0 or $this->initial_data["value"] > 65535)) {
                $this->errors[] = APIResponse\get(2092);
            }
            # Validate this field when the type is set to 'unsigned integer 32'
            elseif ($type === "unsigned integer 32" and (!is_numeric($this->initial_data["value"]) or $this->initial_data["value"] < 0 or $this->initial_data["value"] > 4294967295)) {
                $this->errors[] = APIResponse\get(2093);
            }
            # Validate this field when the type is set to 'signed integer 8'
            elseif ($type === "signed integer 8" and (!is_numeric($this->initial_data["value"]) or $this->initial_data["value"] < -128 or $this->initial_data["value"] > 127)) {
                $this->errors[] = APIResponse\get(2094);
            }
            # Validate this field when the type is set to 'signed integer 16'
            elseif ($type === "signed integer 16" and (!is_numeric($this->initial_data["value"]) or $this->initial_data["value"] < -32768 or $this->initial_data["value"] > 32767)) {
                $this->errors[] = APIResponse\get(2095);
            }
            # Validate this field when the type is set to 'signed integer 32'
            elseif ($type === "signed integer 32" and (!is_numeric($this->initial_data["value"]) or $this->initial_data["value"] < -2147483648 or $this->initial_data["value"] > 2147483647)) {
                $this->errors[] = APIResponse\get(2096);
            }
            # Validate this field when the type is set to 'ip-address'
            elseif ($type === "ip-address" and (!is_ipaddrv4($this->initial_data["value"]) or !is_hostname($this->initial_data["value"]))) {
                $this->errors[] = APIResponse\get(2097);
            }
            # Otherwise, input is valid. Save a Base64 representation of the data as expected by pfSense
            else {
                $this->validated_data["value"] = base64_encode(strval($this->initial_data["value"]));
            }
        } else {
            $this->errors[] = APIResponse\get(2087);
        }
    }
}
