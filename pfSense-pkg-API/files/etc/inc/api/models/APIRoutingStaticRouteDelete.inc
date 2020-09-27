<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIRoutingStaticRouteDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-staticroutes-editroute"];
        $this->change_note = "Deleted static route via API";
    }

    public function action() {
        $del_data = $this->config["staticroutes"]["route"][$this->id];
        APITools\delete_static_route($this->id);
        unset($this->config["staticroutes"]["route"][$this->id]);
        $this->write_config();
        system_routing_configure();
        filter_configure();
        setup_gateways_monitor();
        return APIResponse\get(0, $del_data);
    }

    public function validate_payload() {
        # Check that this static route ID exists
        if (isset($this->initial_data["id"])) {
            if (array_key_exists($this->initial_data["id"], $this->config["staticroutes"]["route"])) {
                $this->id = $this->initial_data["id"];
            } else {
                $this->errors[] = APIResponse\get(6006);
            }
        } else {
            $this->errors[] = APIResponse\get(6005);
        }
    }

}