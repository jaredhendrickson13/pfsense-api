<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesSSHdModify extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system-advanced-admin"];
        $this->change_note = "Modified sshd configuration via API";
    }

    public function action() {
        $this->write_config();
        send_event("filter reload");
        // Check that something was changed before altering service
        if (!empty($this->validated_data)) {
            killbyname("sshd");    // Kill SSHD
            log_error(gettext("secure shell configuration has changed. Stopping sshd."));
            if ($this->config['system']['ssh']['enable']) {
                log_error(gettext("secure shell configuration has changed. Restarting sshd."));
                send_event("service restart sshd");
            }
        }
        return APIResponse\get(0, $this->config["system"]["ssh"]);
    }

    public function validate_payload() {
        if (isset($this->initial_data['enable'])) {
            $enable = $this->initial_data['enable'];
            if ($enable === true) {
                $this->config["system"]["ssh"]["enable"] = "enabled";
            } elseif ($enable === false) {
                unset($this->config["system"]["ssh"]["enable"]);
            } else {
                $this->errors[] = APIResponse\get(2000);
            }
        }
        if (isset($this->initial_data["sshdkeyonly"])) {
            $this->validated_data["sshdkeyonly"] = $this->initial_data["sshdkeyonly"];
            $allowed_auth_types = array("disabled", "enabled", "both");    // Array of allowed auth types
            // Check if our auth type is valid
            if (in_array($this->validated_data["sshdkeyonly"], $allowed_auth_types)) {
                if ($this->validated_data["sshdkeyonly"] === "disabled") {
                    unset($this->config["system"]["ssh"]["sshdkeyonly"]);
                } else {
                    $this->config["system"]["ssh"]["sshdkeyonly"] = $this->validated_data["sshdkeyonly"];
                }
            } else {
                $this->errors[] = APIResponse\get(2001);
            }
        }
        if (isset($this->initial_data['sshdagentforwarding'])) {
            $this->validated_data["sshdagentforwarding"] = $this->initial_data['sshdagentforwarding'];
            if ($this->validated_data["sshdagentforwarding"] === true) {
                $this->config["system"]["ssh"]["sshdagentforwarding"] = "enabled";
            } elseif ($this->validated_data["sshdagentforwarding"] === false) {
                unset($this->config["system"]["ssh"]["sshdagentforwarding"]);
            } else {
                $this->errors[] = APIResponse\get(2002);
            }
        }
        if (isset($this->initial_data['port'])) {
            $this->validated_data["port"] = strval($this->initial_data['port']);
            // Convert string to array
            if (is_port($this->validated_data["port"])) {
                $this->config["system"]["ssh"]["port"] = $this->validated_data["port"];
            } else {
                $this->errors[] = APIResponse\get(2003);
            }
        }
    }
}