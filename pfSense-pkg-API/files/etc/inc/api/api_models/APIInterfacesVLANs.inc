<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIInterfacesVLANs extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-interfaces-vlan"];

    }

    public function action() {
        $vlan_array = [];
        if (!empty($this->config["vlans"]["vlan"])) {
            $vlan_array = $this->config["vlans"]["vlan"];
        }
        return APIResponse\get(0, $vlan_array);
    }
}