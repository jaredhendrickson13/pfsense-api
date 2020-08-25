<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUsersAuthserversLDAP extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-system-authservers"];
    }

    public function action() {
        $as_array = [];
        if (!empty($this->config["system"]["authserver"])) {
            foreach ($this->config["system"]["authserver"] as $a) {
                if ($a["type"] === "ldap") {
                    $as_array[] = $a;
                }
            }
        }
        return APIResponse\get(0, $as_array);
    }
}