<?php

namespace RESTAPI\Forms;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Auth;
use RESTAPI\Core\Form;
use RESTAPI\Models\RESTAPIJWT;

class SystemRESTAPIKeys extends Form {
    public string $url = "/system_restapi_key.php";
    public string $many_sub_url = "/system_restapi_key_edit.php";
    public bool $many = true;
    public bool $editable = false;
    public string $model_name = "\RESTAPI\Models\RESTAPIKey";
    public array $title_path = ["System", "API", "Keys"];
    public array $table_fields = ["username", "hash_algo", "length_bytes", "descr"];
    public array $tabs = [
        ["Settings", false, "/system_restapi_settings.php"],
        ["Keys", true, "/system_restapi_key.php"],
        ["Updates", false, "/system_restapi_updates.php"],
        ["Documentation", false, "/openapi/"]
    ];
}
