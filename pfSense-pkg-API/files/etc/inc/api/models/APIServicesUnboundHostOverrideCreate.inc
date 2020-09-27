<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

function host_cmp($a, $b) {
    return strcasecmp($a['host'], $b['host']);
}

class APIServicesUnboundHostOverrideCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Added DNS Resolver host override via API";
    }

    public function action() {
        $this->config["unbound"]["hosts"][] = $this->validated_data;
        usort($this->config["unbound"]["hosts"], "host_cmp");
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->initial_data['apply'] === true) {
            $applied = unbound_reload_config();
        }
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['host'])) {
            $this->validated_data["host"] = trim($this->initial_data['host']);
        } else {
            $this->errors[] = APIResponse\get(2004);
        }
        if (isset($this->initial_data['domain'])) {
            $this->validated_data["domain"] = trim($this->initial_data['domain']);
            if (APITools\unbound_host_override_exists($this->validated_data["host"], $this->validated_data["domain"])) {
                $this->errors[] = APIResponse\get(2010);
            }
        } else {
            $this->errors[] = APIResponse\get(2005);
        }
        if (isset($this->initial_data['ip'])) {
            if (!is_ipaddrv4($this->initial_data["ip"]) and !is_ipaddrv6($this->initial_data["ip"])) {
                $this->errors[] = APIResponse\get(2011);
            } else {
                $this->validated_data["ip"] = trim($this->initial_data['ip']);
            }
        } else {
            $this->errors[] = APIResponse\get(2006);
        }
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
        if (isset($this->initial_data['aliases'])) {
            $this->validated_data["aliases"] = $this->unbound_parse_aliases($this->initial_data['aliases']);
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