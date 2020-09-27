<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUserGroupDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager-addprivs", "page-system-groupmanager-addprivs"];
        $this->change_note = "Deleted group membership for user via API";
    }

    public function action() {
        local_user_set_groups($this->validated_data["user_config"], $this->validated_data["new_groups"]);    // Set our user's groups
        local_user_set($this->validated_data["user_config"]);    // Reset our user
        $this->write_config();    // Write our config
        // Update our current user's groups
        $this->validated_data["user_config"] = getUserEntry($this->validated_data["username"]);
        $this->validated_data["user_groups"] = local_user_get_groups($this->validated_data["user_config"], true);
        return APIResponse\get(0, $this->validated_data["group"]);
    }

    public function validate_payload() {
        if (isset($this->initial_data['username'])) {
            if (!array_key_exists($this->initial_data["username"], index_users())) {
                $this->errors[] = APIResponse\get(5001);
            } else {
                $this->validated_data["username"] = $this->initial_data['username'];
                $this->validated_data["username"] = trim($this->validated_data["username"]);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
        if (isset($this->initial_data['group'])) {
            if (is_string($this->initial_data["group"])) {
                $this->initial_data["group"] = array($this->initial_data["group"]);
            }
            $this->validated_data["group"] = $this->initial_data['group'];
        } else {
            $this->errors[] = APIResponse\get(5007);
        }

        if (is_array($this->validated_data["group"])) {
            // Get our current user's groups
            $this->validated_data["user_config"] = getUserEntry($this->validated_data["username"]);
            $this->validated_data["user_groups"] = local_user_get_groups($this->validated_data["user_config"], true);
            // Loop through our del group list and ensure it is valid
            foreach ($this->validated_data["group"] as $dg) {
                if (!array_key_exists($dg, index_groups())) {
                    $this->errors[] = APIResponse\get(5008);
                }
                if (in_array($dg, $this->validated_data["user_groups"])) {
                    $this->validated_data["new_groups"] = \array_diff($this->validated_data["user_groups"], array($dg));
                }
            }
        } else {
            $this->errors[] = APIResponse\get(5009);
        }
    }
}