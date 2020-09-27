<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesUnboundHostOverrideDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Deleted DNS Resolver host override via API";
    }

    public function action() {
        usort($this->validated_data["hosts_conf"], "strcmp");
        $this->config["unbound"]["hosts"] = $this->validated_data["hosts_conf"];
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->validated_data["apply"] === true) {
            APITools\unbound_reload_config();
        }
        // Return success if our function was successful
        return APIResponse\get(0, $this->validated_data["del_list"]);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['host'])) {
            $this->validated_data["hostname"] = trim($this->initial_data['host']);
            $h_mode = true;
        }
        if (isset($this->initial_data['domain'])) {
            $this->validated_data["domain"] = trim($this->initial_data['domain']);
            $d_mode = true;
        }
        if (isset($this->initial_data['ip'])) {
            $this->validated_data["ip"] = trim($this->initial_data['ip']);
            $i_mode = true;
        }
        if ($this->initial_data['aliases'] === true) {
            $a_mode = true;
        }
        if ($this->initial_data['apply'] === true) {
            $this->validated_data["apply"] = $this->initial_data['apply'];
        }
        // Determine criteria for deletion
        if ($h_mode and !$d_mode and !$i_mode) {
            $del_mode = "h";
        } elseif ($h_mode and $d_mode and !$i_mode) {
            $del_mode = "hd";
        } elseif ($h_mode and !$d_mode and $i_mode) {
            $del_mode = "hi";
        } elseif ($h_mode and $d_mode and $i_mode) {
            $del_mode = "hdi";
        } elseif (!$h_mode and $d_mode and !$i_mode) {
            $del_mode = "d";
        } elseif (!$h_mode and $d_mode and $i_mode) {
            $del_mode = "di";
        } elseif (!$h_mode and !$d_mode and $i_mode) {
            $del_mode = "i";
        } else {
            $this->errors [] = APIResponse\get(2014);
        }

        // Check that our configuration is a list and loop through each item, otherwise return ok resp
        if (array_key_exists("hosts", $this->config["unbound"]) and is_array($this->config["unbound"]["hosts"])) {
            $this->validated_data["del_list"] = array("hosts" => array(), "aliases" => array());    // List of deleted items
            $this->validated_data["hosts_conf"] = &$this->config["unbound"]["hosts"];    // Current Unbound host overrides
            $h_count = 0;    // Define counter for our hosts loop
            foreach ($this->validated_data["hosts_conf"] as $he) {
                // Check aliases for match if alias mode
                if ($a_mode and is_array($he["aliases"])) {
                    $a_count = 0;    // Define counter for our aliases loop
                    // Loop through aliases to check for matches
                    foreach ($he["aliases"]["item"] as $ae) {
                        if ($del_mode === "h") {
                            if ($this->validated_data["hostname"] === $ae["host"]) {
                                unset($this->validated_data["hosts_conf"][$h_count]["aliases"]["item"][$a_count]);
                                $this->validated_data["del_list"]["aliases"][] = $ae["host"].".".$ae["domain"];
                            }
                        } elseif ($del_mode === "d") {
                            if ($this->validated_data["domain"] === $ae["domain"]) {
                                unset($this->validated_data["hosts_conf"][$h_count]["aliases"]["item"][$a_count]);
                                $this->validated_data["del_list"]["aliases"][] = $ae["host"].".".$ae["domain"];
                            }
                        } elseif ($del_mode === "hd") {
                            if ($this->validated_data["hostname"] === $ae["host"] and $this->validated_data["domain"] === $ae["domain"]) {
                                unset($this->validated_data["hosts_conf"][$h_count]["aliases"]["item"][$a_count]);
                                $this->validated_data["del_list"]["aliases"][] = $ae["host"].".".$ae["domain"];
                            }
                        }
                        // If all aliases were removed, restore aliases key to empty string
                        if (empty($this->validated_data["hosts_conf"][$h_count]["aliases"]["item"])) {
                            $this->validated_data["hosts_conf"][$h_count]["aliases"] = "";
                        }
                        // Increase our alias counter
                        $a_count++;
                    }
                }
                // Check parent host entries
                if ($del_mode === "h") {
                    if ($this->validated_data["hostname"] === $he["host"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                } elseif ($del_mode === "d") {
                    if ($this->validated_data["domain"] === $he["domain"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                } elseif ($del_mode === "i") {
                    if ($this->validated_data["ip"] === $he["ip"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                } elseif ($del_mode === "hd") {
                    if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                } elseif ($del_mode === "hi") {
                    if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["ip"] === $he["ip"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                } elseif ($del_mode === "di") {
                    if ($this->validated_data["domain"] === $he["domain"] and $this->validated_data["ip"] === $he["ip"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                } elseif ($del_mode === "hdi") {
                    if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"] and $this->validated_data["ip"] === $he["ip"]) {
                        unset($this->validated_data["hosts_conf"][$h_count]);
                        $this->validated_data["del_list"]["hosts"][] = $he["host"].".".$he["domain"];
                    }
                }
                // Increase our host counter
                $h_count++;
            }
        } else {
            $this->errors[] = APIResponse\get(2013);
        }
    }
}