<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemTunableDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-sysctl"];
        $this->change_note = "Deleted system tunable via API";
    }

    public function action() {
        unset($this->config["sysctl"]["item"][$this->id]);
        $this->write_config();
        mark_subsystem_dirty('sysctl');
        system_setup_sysctl();
        clear_subsystem_dirty('sysctl');
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Ensure we have a tunable ID to delete
        if (!empty($this->initial_data["id"])) {
            $match_found = false;
            $count = 0;
            foreach ($this->config["sysctl"]["item"] as $tunable) {
                if ($tunable["tunable"] === $this->initial_data["id"]) {
                    $this->validated_data = $tunable;
                    $this->id = $count;
                    $match_found = true;
                    break;
                }
                $count++;
            }
            # Throw error if we could not find a tunable matching this id
            if (!$match_found) {
                $this->errors[] = APIResponse\get(1017);
            }
        } else {
            $this->errors[] = APIResponse\get(1016);
        }
    }
}