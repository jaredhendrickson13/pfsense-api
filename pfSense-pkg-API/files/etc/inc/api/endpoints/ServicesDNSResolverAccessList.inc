<?php

namespace api\endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class ServicesDNSResolverAccessList extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/services/dns_resolver/access_list";
        $this->model_name = "DNSResolverAccessList";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-services-dnsresolver-acls"];
        $this->post_privileges = ["page-all", "page-services-dnsresolver-acls"];
        $this->patch_privileges = ["page-all", "page-services-dnsresolver-acls"];
        $this->delete_privileges = ["page-all", "page-services-dnsresolver-acls"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
