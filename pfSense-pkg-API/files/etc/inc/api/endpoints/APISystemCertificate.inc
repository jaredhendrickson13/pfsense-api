<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemCertificate extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/certificate";
    }

    protected function get() {
        return (new APISystemCertificateRead())->call();
    }

    protected function create() {
        return (new APISystemCertificateCreate())->call();
    }

    protected function delete() {
        return (new APISystemCertificateDelete())->call();
    }
}