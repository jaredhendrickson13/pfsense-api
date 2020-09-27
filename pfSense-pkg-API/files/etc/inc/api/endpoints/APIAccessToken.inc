<?php
require_once("api/framework/APIEndpoint.inc");

class APIAccessToken extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/access_token";
    }

    protected function post() {
        return (new APIAccessTokenCreate())->call();
    }
}