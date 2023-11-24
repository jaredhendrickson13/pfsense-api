<?php

namespace API\Endpointss;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallRule extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/rule";
        $this->model_name = "FirewallRule";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-firewall-rules"];
        $this->post_privileges = ["page-all", "page-firewall-rules-edit"];
        $this->patch_privileges = ["page-all", "page-firewall-rules-edit"];
        $this->delete_privileges = ["page-all", "page-firewall-rules-edit"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
