<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIRoutingStaticRouteRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-staticroutes"];
    }

    public function action() {
        // Check that we have a configuration
        if (!empty($this->config["staticroutes"]["route"])) {
            $routes = $this->config["staticroutes"]["route"];
            foreach ($routes as $id=>$route) {$routes[$id]["id"] = $id;}
        } else {
            $routes = [];
        }
        return APIResponse\get(0, $routes);
    }
}