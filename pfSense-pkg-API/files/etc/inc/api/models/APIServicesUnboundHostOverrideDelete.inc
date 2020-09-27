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
        $del_data = $this->config["unbound"]["hosts"][$this->id];
        unset($this->config["unbound"]["hosts"][$this->id]);
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->initial_data["apply"] === true) {
            APITools\unbound_reload_config();
        }
        return APIResponse\get(0, $del_data);
    }
    
    public function validate_payload() {
        # Check for our required 'id' payload value.
        if (isset($this->initial_data["id"])) {
            if (array_key_exists($this->initial_data["id"], $this->config["unbound"]["hosts"])) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->config["unbound"]["hosts"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(2016);
            }
        } else {
            $this->errors[] = APIResponse\get(2015);
        }
    }
}