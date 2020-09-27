<?php
require_once("api/framework/APIEndpoint.inc");

class APIRoutingStaticRoute extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/routing/static_route";
    }

    protected function get() {
        return (new APIRoutingStaticRouteRead())->call();
    }

    protected function post() {
        return (new APIRoutingStaticRouteCreate())->call();
    }

    protected function put() {
        return (new APIRoutingStaticRouteUpdate())->call();
    }

    protected function delete() {
        return (new APIRoutingStaticRouteDelete())->call();
    }
}