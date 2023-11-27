<?php

namespace api\endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class ServicesDNSResolverAccessLists extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/services/dns_resolver/access_lists";
        $this->many = true;
        $this->model_name = "DNSResolverAccessList";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PUT"];
        $this->get_privileges = ["page-all", "page-services-dnsresolver-acls"];
        $this->put_privileges = ["page-all", "page-services-dnsresolver-acls"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
