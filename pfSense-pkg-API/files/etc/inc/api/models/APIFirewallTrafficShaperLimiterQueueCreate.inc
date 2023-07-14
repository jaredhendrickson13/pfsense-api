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

class APIFirewallTrafficShaperLimiterQueueCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Added firewall traffic shaper limiter queue via API";
    }

    public function action() {
        # Initialize the traffic shaper configuration and save our shaper, mark the subsystem as un-applied
        $next_id = $this->get_next_id("dnshaper/queue/{$this->id}/queue");
        $this->set_config("dnshaper/queue/{$this->id}/queue/{$next_id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty('shaper');

        # Only reload the filter immediately if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            # Reload the filter, reset RRD logs and mark the subsystem as applied
            filter_configure();
            system("rm -f /var/db/rrd/*queuedrops.rrd");
            system("rm -f /var/db/rrd/*queues.rrd");
            enable_rrd_graphing();
            clear_subsystem_dirty('shaper');
        }
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Validate client specified fields
        $this->__validate_limiter();
        $this->__validate_name();
        $this->__validate_mask();
        $this->__validate_maskbits();
        $this->__validate_maskbitsv6();
        $this->__validate_description();
        $this->__validate_aqm();
        $this->__validate_enabled();
        $this->__validate_qlimit();
        $this->__validate_ecn();
        $this->__validate_weight();
        $this->__validate_plr();
        $this->__validate_buckets();
        $this->__validate_param_codel_interval();
        $this->__validate_param_codel_target();
        $this->__validate_param_pie_target();
        $this->__validate_param_pie_tupdate();
        $this->__validate_param_pie_alpha();
        $this->__validate_param_pie_beta();
        $this->__validate_param_pie_max_burst();
        $this->__validate_param_pie_max_ecnth();
        $this->__validate_param_red_w_q();
        $this->__validate_param_red_max_p();
        $this->__validate_param_red_max_th();
        $this->__validate_param_red_min_th();
        $this->__validate_param_gred_w_q();
        $this->__validate_param_gred_max_p();
        $this->__validate_param_gred_max_th();
        $this->__validate_param_gred_min_th();
    }

    private function __validate_limiter() {
        # Check for our required `limiter` payload value
        if (isset($this->initial_data["limiter"])) {
            if ($this->get_limiter_id_by_name($this->initial_data["limiter"], true)) {
                $this->id = $this->get_limiter_id_by_name($this->initial_data["limiter"]);
                $this->validated_data["number"] = $this->get_config("dnshaper/queue/{$this->id}/number");
            } else {
                $this->errors[] = APIResponse\get(4219);
            }
        } else {
            $this->errors[] = APIResponse\get(4218);
        }
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            # Ensure the name only contains alphanumeric, underscores, and hyphens
            if (preg_match('/^[\w-]+$/', $this->initial_data["name"])) {
                # Ensure the name is greater than 1 character and less than equal to 32 characters
                if (strlen($this->initial_data["name"]) >= 1 and strlen($this->initial_data["name"]) <= 32) {
                    # Ensure the name is not already in use
                    if (!$this->is_limiter_name_in_use($this->initial_data["name"])) {
                        $this->validated_data["name"] = $this->initial_data["name"];
                    } else {
                        $this->errors[] = APIResponse\get(4170);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4169);
                }
            } else {
                $this->errors[] = APIResponse\get(4168);
            }
        } else {
            $this->errors[] = APIResponse\get(4167);
        }
    }

    private function __validate_mask() {
        # Check for our required `mask` payload value
        if (isset($this->initial_data["mask"])) {
            # Check that the mask type is supported
            if (in_array($this->initial_data["mask"], ["none", "srcaddress", "dstaddress"])) {
                $this->validated_data["mask"] = $this->initial_data["mask"];
            } else {
                $this->errors[] = APIResponse\get(4171);
            }
        } else {
            $this->validated_data["mask"] = "none";
        }
    }

    private function __validate_maskbits() {
        # Only validate this field if the mask is source address or destination address
        if (in_array($this->validated_data["mask"], ["srcaddress", "dstaddress"])) {
            # Check for our optional `maskbits` payload value
            if (isset($this->initial_data["maskbits"])) {
                # Require input to be numeric
                if (is_numeric($this->initial_data["maskbits"])) {
                    $bitmask = intval($this->initial_data["maskbits"]);

                    # Require input to be between 1 and 32
                    if ($bitmask >= 1 and $bitmask <= 32) {
                        $this->validated_data["maskbits"] = $bitmask;
                    } else {
                        $this->errors[] = APIResponse\get(4172);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4172);
                }
            } else {
                $this->validated_data["maskbits"] = 32;
            }
        }
    }

    private function __validate_maskbitsv6() {
        # Only validate this field if the mask is source address or destination address
        if (in_array($this->validated_data["mask"], ["srcaddress", "dstaddress"])) {
            # Check for our optional `maskbitsv6` payload value
            if (isset($this->initial_data["maskbitsv6"])) {
                # Require input to be numeric
                if (is_numeric($this->initial_data["maskbitsv6"])) {
                    $bitmask = intval($this->initial_data["maskbitsv6"]);

                    # Require input to be between 1 and 128
                    if ($bitmask >= 1 and $bitmask <= 128) {
                        $this->validated_data["maskbitsv6"] = $bitmask;
                    } else {
                        $this->errors[] = APIResponse\get(4173);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4173);
                }
            } else {
                $this->validated_data["maskbitsv6"] = 128;
            }
        }
    }

    private function __validate_description() {
        # Check for our optional `description` payload value
        if (isset($this->initial_data["description"])) {
            $this->validated_data["description"] = strval($this->initial_data["description"]);
        }
    }

    private function __validate_aqm() {
        # Check for our required `aqm` payload value
        if (isset($this->initial_data["aqm"])) {
            # Check that the AQM type is supported
            if (in_array($this->initial_data["aqm"], ["droptail", "codel", "pie", "red", "gred"])) {
                $this->validated_data["aqm"] = $this->initial_data["aqm"];
            } else {
                $this->errors[] = APIResponse\get(4114);
            }
        } else {
            $this->errors[] = APIResponse\get(4174);
        }
    }

    private function __validate_enabled() {
        # Enable this shaper by default if a non-false value was provided
        if ($this->initial_data["enabled"] !== false) {
            $this->validated_data["enabled"] = "on";
        }
    }

    private function __validate_ecn() {
        # Set the Explicit Congestion Notification option if requested
        if ($this->initial_data["ecn"] === true) {
            $aqm = $this->validated_data["aqm"];

            # Only allow this option if either the AQM or the scheduler type support it
            if (in_array($aqm, ["codel", "pie", "red", "gred"])) {
                $this->validated_data["ecn"] = "on";
            } else {
                $this->errors[] = APIResponse\get(4176);
            }
        }
    }

    private function __validate_qlimit() {
        # Check for our optional `qlimit` payload value
        if (isset($this->initial_data["qlimit"])) {
            # Ensure the qlimit is 1 or greater
            if (is_numeric($this->initial_data["qlimit"]) and intval($this->initial_data["qlimit"]) >= 1) {
                $this->validated_data["qlimit"] = intval($this->initial_data["qlimit"]);
            } else {
                $this->errors[] = APIResponse\get(4120);
            }
        }
    }

    private function __validate_weight() {
        # Check for our optional `weight` payload value
        if (isset($this->initial_data["weight"])) {
            # Ensure the weight is numeric
            if (is_numeric($this->initial_data["weight"])) {
                # Ensure the weight value is between 1 and 100
                if (intval($this->initial_data["weight"]) >= 1 and intval($this->initial_data["weight"]) <= 100) {
                    $this->validated_data["weight"] = intval($this->initial_data["weight"]);
                } else {
                    $this->errors[] = APIResponse\get(4217);
                }
            } else {
                $this->errors[] = APIResponse\get(4217);
            }
        }
    }

    private function __validate_plr() {
        # Check for our optional `plr` payload value
        if (isset($this->initial_data["plr"])) {
            # Ensure the plr is numeric
            if (is_numeric($this->initial_data["plr"])) {
                # Ensure the plr value is between 0 and 1
                if (floatval($this->initial_data["plr"]) >= 0 and floatval($this->initial_data["plr"]) <= 1) {
                    $this->validated_data["plr"] = floatval($this->initial_data["plr"]);
                } else {
                    $this->errors[] = APIResponse\get(4178);
                }
            } else {
                $this->errors[] = APIResponse\get(4178);
            }
        } else {
            $this->validated_data["plr"] = 0;
        }
    }

    private function __validate_buckets() {
        # Check for our optional `buckets` payload value
        if (isset($this->initial_data["buckets"])) {
            # Ensure the buckets is numeric
            if (is_numeric($this->initial_data["buckets"])) {
                # Ensure the buckets value is between 0 and 1
                if (intval($this->initial_data["buckets"]) >= 16 and intval($this->initial_data["buckets"]) <= 65535) {
                    $this->validated_data["buckets"] = intval($this->initial_data["buckets"]);
                } else {
                    $this->errors[] = APIResponse\get(4179);
                }
            } else {
                $this->errors[] = APIResponse\get(4179);
            }
        }
    }

    ### AQM SPECIFIC FIELDS ###
    # CODEL AQM PARAMS #
    private function __validate_param_codel_target() {
        # Only validate this field if the AQM is set to codel
        if ($this->validated_data["aqm"] === "codel") {
            # Check for our optional `param_codel_target` payload value
            if (isset($this->initial_data["param_codel_target"])) {
                # Ensure the param_codel_target is numeric
                if (is_numeric($this->initial_data["param_codel_target"])) {
                    # Ensure the param_codel_target value is 0 or greater
                    if (intval($this->initial_data["param_codel_target"]) >= 0) {
                        $this->validated_data["param_codel_target"] = intval($this->initial_data["param_codel_target"]);
                    } else {
                        $this->errors[] = APIResponse\get(4180);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4180);
                }
            } else {
                $this->validated_data["param_codel_target"] = 5;
            }
        }
    }

    private function __validate_param_codel_interval() {
        # Only validate this field if the AQM is set to codel
        if ($this->validated_data["aqm"] === "codel") {
            # Check for our optional `param_codel_interval` payload value
            if (isset($this->initial_data["param_codel_interval"])) {
                # Ensure the param_codel_interval is numeric
                if (is_numeric($this->initial_data["param_codel_interval"])) {
                    # Ensure the param_codel_interval value is 0 or greater
                    if (intval($this->initial_data["param_codel_interval"]) >= 0) {
                        $this->validated_data["param_codel_interval"] = intval($this->initial_data["param_codel_interval"]);
                    } else {
                        $this->errors[] = APIResponse\get(4181);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4181);
                }
            } else {
                $this->validated_data["param_codel_interval"] = 100;
            }
        }
    }

    # PIE AQM PARAMS #
    private function __validate_param_pie_target() {
        # Only validate this field if the AQM is set to pie
        if ($this->validated_data["aqm"] === "pie") {
            # Check for our optional `param_pie_target` payload value
            if (isset($this->initial_data["param_pie_target"])) {
                # Ensure the param_pie_target is numeric
                if (is_numeric($this->initial_data["param_pie_target"])) {
                    # Ensure the param_codel_target value is 0 or greater
                    if (intval($this->initial_data["param_pie_target"]) >= 0) {
                        $this->validated_data["param_pie_target"] = intval($this->initial_data["param_pie_target"]);
                    } else {
                        $this->errors[] = APIResponse\get(4182);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4182);
                }
            } else {
                $this->validated_data["param_pie_target"] = 15;
            }
        }
    }

    private function __validate_param_pie_tupdate() {
        # Only validate this field if the AQM is set to pie
        if ($this->validated_data["aqm"] === "pie") {
            # Check for our optional `param_pie_tupdate` payload value
            if (isset($this->initial_data["param_pie_tupdate"])) {
                # Ensure the param_pie_tupdate is numeric
                if (is_numeric($this->initial_data["param_pie_tupdate"])) {
                    # Ensure the param_pie_tupdate value is between 0 or greater
                    if (intval($this->initial_data["param_pie_tupdate"]) >= 0) {
                        $this->validated_data["param_pie_tupdate"] = intval($this->initial_data["param_pie_tupdate"]);
                    } else {
                        $this->errors[] = APIResponse\get(4183);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4183);
                }
            } else {
                $this->validated_data["param_pie_tupdate"] = 15;
            }
        }
    }

    private function __validate_param_pie_alpha() {
        # Only validate this field if the AQM is set to pie
        if ($this->validated_data["aqm"] === "pie") {
            # Check for our optional `param_pie_alpha` payload value
            if (isset($this->initial_data["param_pie_alpha"])) {
                # Ensure the param_pie_alpha is numeric
                if (is_numeric($this->initial_data["param_pie_alpha"])) {
                    # Ensure the param_pie_alpha value is 0 or greater
                    if (intval($this->initial_data["param_pie_alpha"]) >= 0) {
                        $this->validated_data["param_pie_alpha"] = intval($this->initial_data["param_pie_alpha"]);
                    } else {
                        $this->errors[] = APIResponse\get(4184);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4184);
                }
            } else {
                $this->validated_data["param_pie_alpha"] = 125;
            }
        }
    }

    private function __validate_param_pie_beta() {
        # Only validate this field if the AQM is set to pie
        if ($this->validated_data["aqm"] === "pie") {
            # Check for our optional `param_pie_beta` payload value
            if (isset($this->initial_data["param_pie_beta"])) {
                # Ensure the param_pie_beta is numeric
                if (is_numeric($this->initial_data["param_pie_beta"])) {
                    # Ensure the param_pie_beta value is 0 or greater
                    if (intval($this->initial_data["param_pie_beta"]) >= 0) {
                        $this->validated_data["param_pie_beta"] = intval($this->initial_data["param_pie_beta"]);
                    } else {
                        $this->errors[] = APIResponse\get(4185);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4185);
                }
            } else {
                $this->validated_data["param_pie_beta"] = 1250;
            }
        }
    }

    private function __validate_param_pie_max_burst() {
        # Only validate this field if the AQM is set to pie
        if ($this->validated_data["aqm"] === "pie") {
            # Check for our optional `param_pie_max_burst` payload value
            if (isset($this->initial_data["param_pie_max_burst"])) {
                # Ensure the param_pie_max_burst is numeric
                if (is_numeric($this->initial_data["param_pie_max_burst"])) {
                    # Ensure the param_pie_max_burst value is 0 or greater
                    if (intval($this->initial_data["param_pie_max_burst"]) >= 0) {
                        $this->validated_data["param_pie_max_burst"] = intval($this->initial_data["param_pie_max_burst"]);
                    } else {
                        $this->errors[] = APIResponse\get(4186);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4186);
                }
            } else {
                $this->validated_data["param_pie_max_burst"] = 150000;
            }
        }
    }

    private function __validate_param_pie_max_ecnth() {
        # Only validate this field if the AQM is set to pie
        if ($this->validated_data["aqm"] === "pie") {
            # Check for our optional `param_pie_max_ecnth` payload value
            if (isset($this->initial_data["param_pie_max_ecnth"])) {
                # Ensure the param_pie_max_ecnth is numeric
                if (is_numeric($this->initial_data["param_pie_max_ecnth"])) {
                    # Ensure the param_pie_max_ecnth value is 0 or greater
                    if (intval($this->initial_data["param_pie_max_ecnth"]) >= 0) {
                        $this->validated_data["param_pie_max_ecnth"] = intval($this->initial_data["param_pie_max_ecnth"]);
                    } else {
                        $this->errors[] = APIResponse\get(4187);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4187);
                }
            } else {
                $this->validated_data["param_pie_max_ecnth"] = 99;
            }
        }
    }

    # RED AQM PARAMS #
    private function __validate_param_red_w_q() {
        # Only validate this field if the AQM is set to red
        if ($this->validated_data["aqm"] === "red") {
            # Check for our optional `param_red_w_q` payload value
            if (isset($this->initial_data["param_red_w_q"])) {
                # Ensure the param_red_w_q is numeric
                if (is_numeric($this->initial_data["param_red_w_q"])) {
                    # Ensure the param_red_w_q value is 1 or greater
                    if (intval($this->initial_data["param_red_w_q"]) >= 1) {
                        $this->validated_data["param_red_w_q"] = intval($this->initial_data["param_red_w_q"]);
                    } else {
                        $this->errors[] = APIResponse\get(4188);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4188);
                }
            } else {
                $this->validated_data["param_red_w_q"] = 1;
            }
        }
    }

    private function __validate_param_red_min_th() {
        # Only validate this field if the AQM is set to red
        if ($this->validated_data["aqm"] === "red") {
            # Check for our optional `param_red_min_th` payload value
            if (isset($this->initial_data["param_red_min_th"])) {
                # Ensure the param_red_min_th is numeric
                if (is_numeric($this->initial_data["param_red_min_th"])) {
                    # Ensure the param_red_min_th value is 1 or greater
                    if (intval($this->initial_data["param_red_min_th"]) >= 0) {
                        $this->validated_data["param_red_min_th"] = intval($this->initial_data["param_red_min_th"]);
                    } else {
                        $this->errors[] = APIResponse\get(4189);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4189);
                }
            } else {
                $this->validated_data["param_red_min_th"] = 0;
            }
        }
    }

    private function __validate_param_red_max_th() {
        # Only validate this field if the AQM is set to red
        if ($this->validated_data["aqm"] === "red") {
            # Check for our optional `param_red_max_th` payload value
            if (isset($this->initial_data["param_red_max_th"])) {
                # Ensure the param_red_max_th is numeric
                if (is_numeric($this->initial_data["param_red_max_th"])) {
                    # Ensure the param_red_max_th value is 1 or greater
                    if (intval($this->initial_data["param_red_max_th"]) >= 1) {
                        $this->validated_data["param_red_max_th"] = intval($this->initial_data["param_red_max_th"]);
                    } else {
                        $this->errors[] = APIResponse\get(4190);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4190);
                }
            } else {
                $this->validated_data["param_red_max_th"] = 1;
            }
        }
    }

    private function __validate_param_red_max_p() {
        # Only validate this field if the AQM is set to red
        if ($this->validated_data["aqm"] === "red") {
            # Check for our optional `param_red_max_p` payload value
            if (isset($this->initial_data["param_red_max_p"])) {
                # Ensure the param_red_max_p is numeric
                if (is_numeric($this->initial_data["param_red_max_p"])) {
                    # Ensure the param_red_max_p value is 1 or greater
                    if (intval($this->initial_data["param_red_max_p"]) >= 1) {
                        $this->validated_data["param_red_max_p"] = intval($this->initial_data["param_red_max_p"]);
                    } else {
                        $this->errors[] = APIResponse\get(4191);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4191);
                }
            } else {
                $this->validated_data["param_red_max_p"] = 1;
            }
        }
    }

    # GRED AQM PARAMS #
    private function __validate_param_gred_w_q() {
        # Only validate this field if the AQM is set to red
        if ($this->validated_data["aqm"] === "gred") {
            # Check for our optional `param_gred_w_q` payload value
            if (isset($this->initial_data["param_gred_w_q"])) {
                # Ensure the param_gred_w_q is numeric
                if (is_numeric($this->initial_data["param_gred_w_q"])) {
                    # Ensure the param_gred_w_q value is 1 or greater
                    if (intval($this->initial_data["param_gred_w_q"]) >= 1) {
                        $this->validated_data["param_gred_w_q"] = intval($this->initial_data["param_gred_w_q"]);
                    } else {
                        $this->errors[] = APIResponse\get(4192);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4192);
                }
            } else {
                $this->validated_data["param_gred_w_q"] = 1;
            }
        }
    }

    private function __validate_param_gred_min_th() {
        # Only validate this field if the AQM is set to gred
        if ($this->validated_data["aqm"] === "gred") {
            # Check for our optional `param_gred_min_th` payload value
            if (isset($this->initial_data["param_gred_min_th"])) {
                # Ensure the param_gred_min_th is numeric
                if (is_numeric($this->initial_data["param_gred_min_th"])) {
                    # Ensure the param_gred_min_th value is 1 or greater
                    if (intval($this->initial_data["param_gred_min_th"]) >= 0) {
                        $this->validated_data["param_gred_min_th"] = intval($this->initial_data["param_gred_min_th"]);
                    } else {
                        $this->errors[] = APIResponse\get(4193);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4193);
                }
            } else {
                $this->validated_data["param_gred_min_th"] = 0;
            }
        }
    }

    private function __validate_param_gred_max_th() {
        # Only validate this field if the AQM is set to gred
        if ($this->validated_data["aqm"] === "gred") {
            # Check for our optional `param_gred_max_th` payload value
            if (isset($this->initial_data["param_gred_max_th"])) {
                # Ensure the param_gred_max_th is numeric
                if (is_numeric($this->initial_data["param_gred_max_th"])) {
                    # Ensure the param_gred_max_th value is 1 or greater
                    if (intval($this->initial_data["param_gred_max_th"]) >= 1) {
                        $this->validated_data["param_gred_max_th"] = intval($this->initial_data["param_gred_max_th"]);
                    } else {
                        $this->errors[] = APIResponse\get(4194);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4194);
                }
            } else {
                $this->validated_data["param_gred_max_th"] = 1;
            }
        }
    }

    private function __validate_param_gred_max_p() {
        # Only validate this field if the AQM is set to gred
        if ($this->validated_data["aqm"] === "gred") {
            # Check for our optional `param_gred_max_p` payload value
            if (isset($this->initial_data["param_gred_max_p"])) {
                # Ensure the param_gred_max_p is numeric
                if (is_numeric($this->initial_data["param_gred_max_p"])) {
                    # Ensure the param_gred_max_p value is 1 or greater
                    if (intval($this->initial_data["param_gred_max_p"]) >= 1) {
                        $this->validated_data["param_gred_max_p"] = intval($this->initial_data["param_gred_max_p"]);
                    } else {
                        $this->errors[] = APIResponse\get(4195);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4195);
                }
            } else {
                $this->validated_data["param_gred_max_p"] = 1;
            }
        }
    }

    public function is_limiter_name_in_use($name) {
        # Loop through each configured limiter and see if it uses this name
        foreach ($this->get_config("dnshaper/queue", []) as $limiter) {
            # Check for matching names
            if ($name === $limiter["name"]) {
                return true;
            }

            # Also loop through child queues to ensure they do not use the same name
            foreach ($limiter["queue"] as $queue) {
                # Check for matching names
                if ($name === $queue["name"]) {
                    return true;
                };
            }
        }
        return false;
    }

    public function get_limiter_id_by_name($name, $as_bool=false) {
        # Loop through each configured limiter and see if it uses this name
        foreach ($this->get_config("dnshaper/queue", []) as $id=>$limiter) {
            # Check for matching names
            if ($name === $limiter["name"]) {
                return ($as_bool) ? true : $id;
            }
        }
        return ($as_bool) ? false : null;
    }
}
