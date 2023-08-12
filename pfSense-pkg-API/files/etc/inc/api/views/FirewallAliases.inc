<?php

namespace API\Views;

require_once("api/auto_loader.inc");

use API\Core\View;

class FirewallAliases extends View
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/aliases";
        $this->model_name = "FirewallAlias";
        $this->many = true;

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PUT"];
        $this->get_privileges = ["page-all", "page-firewall-aliases"];
        $this->put_privileges = ["page-all", "page-firewall-alias-edit"];

        # Construct the parent View object
        parent::__construct();
    }
}
