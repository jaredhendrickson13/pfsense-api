<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class User extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/user";
        $this->model_name = "User";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-system-usermanager"];
        $this->post_privileges = ["page-all", "page-system-usermanager"];
        $this->patch_privileges = ["page-all", "page-system-usermanager"];
        $this->delete_privileges = ["page-all", "page-system-usermanager"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
