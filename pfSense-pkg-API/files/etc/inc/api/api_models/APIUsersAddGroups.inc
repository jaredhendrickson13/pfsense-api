<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUsersAddGroups extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system-groupmanager"];
        $this->change_note = "Added group membership for user via API";
    }

    public function action() {
        local_user_set_groups($this->validated_data["user_config"], $this->validated_data["user_groups"]);    // Set our user's groups
        $this->write_config();
        return APIResponse\get(0, $this->validated_data["user_groups"]);
    }

    public function validate_payload() {
        if (isset($this->initial_data['username'])) {
            if (!array_key_exists($this->initial_data['username'], index_users())) {
                $this->errors[] = APIResponse\get(5001);
            } else {
                $this->validated_data["username"] = $this->initial_data['username'];
                $this->validated_data["user_config"] = getUserEntry($this->validated_data["username"]);
                $user_groups = local_user_get_groups($this->validated_data["user_config"], true);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
        if (isset($this->initial_data['group'])) {
            if (is_string($this->initial_data['group'])) {
                $this->initial_data['group'] = array($this->initial_data['group']);
            }
            if (is_array($this->initial_data['group'])) {
                $this->validated_data["user_groups"] = [];
                foreach ($this->initial_data['group'] as $ng) {
                    if (!array_key_exists($ng, index_groups())) {
                        $this->errors[] = APIResponse\get(5008);
                    }
                    if (!in_array($ng, $user_groups)) {
                        $this->validated_data["user_groups"][] = $ng;
                    }
                }
                // Add our new groups to our existing groups
                $this->validated_data["user_groups"] = array_merge($user_groups, $this->validated_data["user_groups"]);
            } else {
                $this->errors[] = APIResponse\get(5009);
            }
        } else {
            $this->errors[] = APIResponse\get(5007);
        }
    }
}