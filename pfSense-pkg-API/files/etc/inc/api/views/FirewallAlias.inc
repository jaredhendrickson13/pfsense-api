<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallAlias extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/alias";
        $this->model_name = "FirewallAlias";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-firewall-aliases"];
        $this->post_privileges = ["page-all", "page-firewall-alias-edit"];
        $this->patch_privileges = ["page-all", "page-firewall-alias-edit"];
        $this->delete_privileges = ["page-all", "page-firewall-alias-edit"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
