<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesDpingerStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/dpinger/start";
    }

    protected function post() {
        return (new APIServicesDpingerStartCreate())->call();
    }
}