<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesUnboundModifyHosts extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Modified DNS Resolver host override via API";
    }

    public function action() {
        // Sort and write our new configuration
        //usort($this->validated_data["hosts_conf"], "strcmp");
        $this->config["unbound"]["hosts"] = $this->validated_data["hosts_conf"];
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->validated_data["apply"] === true) {
            APITools\unbound_reload_config();
        }
        return APIResponse\get(0, $this->validated_data["update_list"]);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['host'])) {
            $this->validated_data["hostname"] = trim($this->initial_data['host']);
            $h_mode = true;
        }
        if (isset($this->initial_data['new_host'])) {
            $new_hostname = trim($this->initial_data['new_host']);
        }
        if (isset($this->initial_data['domain'])) {
            $this->validated_data["domain"] = trim($this->initial_data['domain']);
        } elseif ($h_mode) {
            $this->errors[] = APIResponse\get(2005);
        }
        if (isset($this->initial_data['new_domain'])) {
            $this->validated_data["new_domain"] = trim($this->initial_data['new_domain']);
        }
        if (isset($this->initial_data['ip']) and !$h_mode) {
            $this->validated_data["ip"] = trim($this->initial_data['ip']);
            $i_mode = true;
        }
        if (isset($this->initial_data['new_ip'])) {
            $this->validated_data["new_ip"] = trim($this->initial_data['new_ip']);
        } elseif ($i_mode) {
            $this->errors[] = APIResponse\get(2012);
        }
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
        if (isset($this->initial_data['aliases'])) {
            $this->validated_data["aliases"] = $this->initial_data['aliases'];
        }
        if ($this->initial_data['apply'] === true) {
            $this->validated_data["apply"] = $this->initial_data['apply'];
        }
        // Validate our input against our exist configuration
        if (APITools\unbound_host_override_exists($this->validated_data["hostname"], $this->validated_data["domain"]) or $i_mode) {
            $this->validated_data["hosts_conf"] = $this->config["unbound"]["hosts"];    // Current Unbound host overrides
            $h_count = 0;    // Assign a loop counter
            $this->validated_data["update_list"] = array();    // Assign array to track which values were updated
            // Check modification mode
            if ($i_mode) {
                if (is_ipaddrv4($this->validated_data["new_ip"]) or is_ipaddrv6($this->validated_data["new_ip"])) {
                    foreach ($this->validated_data["hosts_conf"] as $he) {
                        // If our IP matches, update our IP
                        if ($this->validated_data["ip"] === $he["ip"]) {
                            $this->validated_data["hosts_conf"][$h_count]["ip"] = $this->validated_data["new_ip"];
                            $this->validated_data["update_list"][] = $this->validated_data["hosts_conf"][$h_count];
                        }
                        // Increase our counter
                        $h_count++;
                    }
                } else {
                    $this->errors[] = APIResponse\get(2011);
                }
            } elseif ($h_mode) {
                foreach ($this->validated_data["hosts_conf"] as $he) {
                    $he_updated = false;
                    // Check if both our hostname and domain names were changed
                    if (isset($new_hostname) and isset($this->validated_data["new_domain"])) {
                        if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"]) {
                            if (!APITools\unbound_host_override_exists($new_hostname, $this->validated_data["new_domain"])) {
                                $this->validated_data["hosts_conf"][$h_count]["host"] = $new_hostname;
                                $this->validated_data["hosts_conf"][$h_count]["domain"] = $this->validated_data["new_domain"];
                                $he_updated = true;
                            } else {
                                $this->errors[] = APIResponse\get(2010);
                            }
                        }
                    } elseif (isset($new_hostname)) {
                        if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"]) {
                            if (!APITools\unbound_host_override_exists($new_hostname, $he["domain"])) {
                                $this->validated_data["hosts_conf"][$h_count]["host"] = $new_hostname;
                                $he_updated = true;
                            } else {
                                $this->errors[] = APIResponse\get(2010);
                            }
                        }
                    } elseif (isset($this->validated_data["new_domain"])) {
                        if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"]) {
                            if (!APITools\unbound_host_override_exists($he["host"], $this->validated_data["new_domain"])) {
                                $this->validated_data["hosts_conf"][$h_count]["domain"] = $this->validated_data["new_domain"];
                                $he_updated = true;
                            } else {
                                $this->errors[] = APIResponse\get(2010);
                            }
                        }
                    }
                    if (isset($this->validated_data["new_ip"])) {
                        if (is_ipaddrv4($this->validated_data["new_ip"]) or is_ipaddrv6($this->validated_data["new_ip"])) {
                            if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"]) {
                                $this->validated_data["hosts_conf"][$h_count]["ip"] = $this->validated_data["new_ip"];
                                $he_updated = true;
                            }
                        } else {
                            $this->errors[] = APIResponse\get(2011);
                        }
                    }
                    if (isset($this->validated_data["descr"])) {
                        if ($this->validated_data["hostname"] === $he["host"] and $this->validated_data["domain"] === $he["domain"]) {
                            $this->validated_data["hosts_conf"][$h_count]["descr"] = $this->validated_data["descr"];
                            $he_updated = true;
                        }
                    }
                    if (isset($this->validated_data["aliases"])) {
                        // Check if we have more than one
                        if (count($this->validated_data["update_list"]) <= 1) {
                            $alias_fin = $this->unbound_parse_aliases($this->validated_data["aliases"]);    // Parse our aliases
                            if ($alias_fin !== "") {
                                $this->validated_data["hosts_conf"][$h_count]["aliases"] = $alias_fin;
                                $he_updated = true;
                            }
                        }
                    }
                    // Check if our entry was updated, if so add it to our update list
                    if ($he_updated) {
                        $this->validated_data["update_list"][] = $this->validated_data["hosts_conf"][$h_count];
                    }
                    // Increase our counter
                    $h_count++;
                }
            }
        } else {
            $this->errors[] = APIResponse\get(2013);
        }
    }

    // Check user's DNS Resolver (Unbound) host override alias input for errors
    public function unbound_parse_aliases($aliases) {
        $aliases_fin = "";    // Default our alias value to blank string
        if (is_array($aliases)) {
            $a_count = 0;
            $aliases_fin = array();
            foreach ($aliases as $alias_ent) {
                // Check that each alias has a valid hostname
                if (array_key_exists("host", $alias_ent) and is_string($alias_ent["host"])) {
                    $aliases_fin[$a_count] = array("host" => trim($alias_ent["host"]));
                } else {
                    $this->errors[] = APIResponse\get(2007);
                }
                // Check that each alias has a valid domain name
                if (array_key_exists("domain", $alias_ent) and is_string($alias_ent["domain"])) {
                    $aliases_fin[$a_count]["domain"] = trim($alias_ent["domain"]);
                } else {
                    $this->errors[] = APIResponse\get(2008);
                }
                // Check for alias description under descr key
                if (array_key_exists("descr", $alias_ent) and is_string($alias_ent["descr"])) {
                    if (is_string($alias_ent["descr"])) {
                        $aliases_fin[$a_count]["description"] = $alias_ent["descr"];
                    }
                }
                // Check for alias description under description key
                if (array_key_exists("description", $alias_ent) and is_string($alias_ent["description"])) {
                    if (is_string($alias_ent["description"])) {
                        $aliases_fin[$a_count]["description"] = $alias_ent["description"];
                    }
                }
                // Increase our counter
                $a_count++;
            }
            // Check if our aliases already exist
            foreach ($aliases_fin as $af) {
                print_r($af);
                if (APITools\unbound_host_override_exists($af["host"], $af["domain"]) === true) {
                    $this->errors[] = APIResponse\get(2009);
                }
            }
            // Nest our aliases under the item key
            $aliases_fin = array("item" => $aliases_fin);
        }
        // Return our parsed aliases
        return $aliases_fin;
    }
}