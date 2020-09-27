<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIInterfaceVLANRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-vlan", "page-interfaces-vlan-edit"];

    }

    public function action() {
        $vlan_array = [];
        if (!empty($this->config["vlans"]["vlan"])) {
            $vlan_array = $this->config["vlans"]["vlan"];
        }
        return APIResponse\get(0, $vlan_array);
    }
}