<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemTunableCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-sysctl"];
        $this->change_note = "Added system tunable via API";
    }

    public function action() {
        $this->config["sysctl"] = (!is_array($this->config["sysctl"])) ? [] : $this->config["sysctl"];
        $this->config["sysctl"]["item"] = (!is_array($this->config["sysctl"]["item"])) ? [] : $this->config["sysctl"]["item"];
        $this->config["sysctl"]["item"][] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty('sysctl');
        system_setup_sysctl();
        clear_subsystem_dirty('sysctl');
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Ensure we have a tunable name, that it doesn't already exist, and that it is a string
        if (!empty($this->initial_data["tunable"])) {
            if (is_string($this->initial_data["tunable"])) {
                foreach ($this->config["sysctl"]["item"] as $item) {
                    if ($item["tunable"] === $this->initial_data["tunable"]) {
                        $this->errors[] = APIResponse\get(1012);
                        break;
                    }
                }
                $this->validated_data["tunable"] = $this->initial_data["tunable"];
            } else {
                $this->errors[] = APIResponse\get(1011);
            }
        } else {
            $this->errors[] = APIResponse\get(1010);
        }

        # Ensure we have a tunable value and that it is a string or integer
        if (!empty($this->initial_data["value"])) {
            if (!preg_match("/[^a-zA-Z0-9.\-_%\/]/", $this->initial_data['value'])) {
                $this->validated_data["value"] = $this->initial_data["value"];
            } else {
                $this->errors[] = APIResponse\get(1014);
            }
        } else {
            $this->errors[] = APIResponse\get(1013);
        }

        # Optionally allow a tunable description, ensure it is a string if entered
        if (!empty($this->initial_data["descr"])) {
            if (is_string($this->initial_data["descr"])) {
                $this->validated_data["descr"] = $this->initial_data["descr"];
            } else {
                $this->errors[] = APIResponse\get(1015);
            }
        }
    }
}