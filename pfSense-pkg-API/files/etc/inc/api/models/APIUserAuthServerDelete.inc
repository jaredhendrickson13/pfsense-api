<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUserAuthServerDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-user-authserver"];
        $this->change_note = "Deleted authentication server via API";
    }

    public function action() {
        $del_server = $this->config["system"]["authserver"][$this->validated_data["id"]];
        unset($this->config["system"]["authserver"][$this->validated_data["id"]]);
        if ($this->validated_data["revert"] === true) {
            unset($this->config["system"]["webgui"]["authmode"]);
        }
        $this->write_config();
        return APIResponse\get(0, $del_server);
    }

    public function validate_payload() {
        if (isset($this->initial_data['name'])) {
            $this->validated_data["name"] = $this->initial_data['name'];
        } else {
            $this->errors[] = APIResponse\get(5010);
        }

        // Loop through our servers and check that this server exists
        foreach ($this->config["system"]["authserver"] as $asid => $asc) {
            if ($this->validated_data["name"] === $asc["name"]) {
                $this->validated_data["id"] = $asid;    // Save our config before deleting
                // Check if this auth server is our default, if so revert to local database
                if ($this->validated_data["name"] === $this->config["system"]["webgui"]["authmode"]) {
                    $this->validated_data["revert"] = True;
                }
            }
        }
        if (is_null($this->validated_data["id"])) {
            $this->errors[] = APIResponse\get(5025);
        }
    }
}