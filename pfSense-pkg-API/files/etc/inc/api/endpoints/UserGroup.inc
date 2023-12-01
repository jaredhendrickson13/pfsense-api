<?php

namespace api\endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class UserGroup extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/user/group";
        $this->model_name = "UserGroup";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-system-groupmanager"];
        $this->post_privileges = ["page-all", "page-system-groupmanager"];
        $this->patch_privileges = ["page-all", "page-system-groupmanager"];
        $this->delete_privileges = ["page-all", "page-system-groupmanager"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
