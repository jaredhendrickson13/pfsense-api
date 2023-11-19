<?php

namespace api\endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class ServicesDNSResolverHostOverride extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/services/dns_resolver/host_override";
        $this->model_name = "DNSResolverHostOverride";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-services-dnsresolver"];
        $this->post_privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->patch_privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->delete_privileges = ["page-all", "page-services-dnsresolver-edithost"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
