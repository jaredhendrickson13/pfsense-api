<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallAliasUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Modified firewall alias address via API";
    }

    public function action() {
        $this->config["aliases"]["alias"][$this->id] = $this->validated_data;    // Write to our master config
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $this->config["aliases"]["alias"][$this->id]);
    }
    
    public function validate_payload() {
        $allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types
        if (isset($this->initial_data['id'])) {
            $name = $this->initial_data['id'];
            $name = APITools\sanitize_str($name);
        } else {
            $this->errors[] = APIResponse\get(4074);
        }
        if (isset($this->initial_data['name'])) {
            $new_name = $this->initial_data['name'];
            $new_name = APITools\sanitize_str($new_name);
        }
        if (isset($this->initial_data['type'])) {
            $type = $this->initial_data['type'];
            $type = trim($type);
        } else {
            $type = alias_get_type($name);
        }
        if (isset($this->initial_data['address'])) {
            $address = $this->initial_data['address'];
            // Convert string to array
            if (!is_array($address)) {
                $address = array($address);
            }
        }
        if (isset($this->initial_data['descr'])) {
            $descr = strval($this->initial_data['descr']);
            $descr = str_replace(PHP_EOL, "", $descr);
        }
        if (isset($this->initial_data['detail'])) {
            $detail = $this->initial_data['detail'];
            // Convert string to array
            if (!is_array($detail)) {
                $detail = array($detail);
            }
        }
        // INPUT VALIDATION
        if (!is_alias($name)) {
            $this->errors[] = APIResponse\get(4055);
        } elseif (isset($new_name) and is_alias($new_name)) {
            $this->errors[] = APIResponse\get(4056);
        } elseif (isset($type) and !in_array($type, $allowed_alias_types)) {
            $this->errors[] = APIResponse\get(4057);
        }
        if (isset($address)) {
            foreach ($address as $nae) {
                if ($type === "host") {
                    if (!is_ipaddr($nae) and !is_hostname($nae)) {
                        $this->errors[] = APIResponse\get(4058);
                        break;
                    }
                }
                if ($type === "network") {
                    if (!is_subnet($nae) and !is_hostname($nae)) {
                        $this->errors[] = APIResponse\get(4059);
                        break;
                    }
                }
                if ($type === "port") {
                    if (!is_port_or_range($nae)) {
                        $this->errors[] = APIResponse\get(4060);
                        break;
                    }
                }
            }
        }
        // Save our alias ID
        $c_count = 0;
        foreach ($this->config["aliases"]["alias"] as $ca) {
            if ($name === $ca["name"]) {
                $this->id = $c_count;
                break;
            }
            $c_count++;
        }
        // Make our alias change
        $this->validated_data = $this->config["aliases"]["alias"][$this->id];    // Save our current alias entry
        if (isset($new_name)) {
            $this->validated_data["name"] = $new_name;
            update_alias_name($new_name, $name);    // Update our alias name
        }
        if (isset($type)) {
            $this->validated_data["type"] = $type;
        }
        if (isset($descr)) {
            $this->validated_data["descr"] = $descr;
        }
        if (isset($address)) {
            $this->validated_data["address"] = implode(" ", $address);
        }
        if (isset($detail)) {
            $this->validated_data["detail"] = implode("||", $detail);
        }
    }
}