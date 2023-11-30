<?php

namespace API\Forms;

require_once("api/auto_loader.inc");

use API\Core\Auth;
use API\Core\Form;
use API\Models\APIJWT;

class SystemAPIKeys extends Form {
    public string $url = "/system_api_key.php";
    public string $many_sub_url = "/system_api_key_edit.php";
    public bool $many = true;
    public bool $editable = false;
    public string $model_name = "\API\Models\APIKey";
    public array $title_path = ["System", "API", "Keys"];
    public string $custom_css = "/css/api/api.css";
    public array $table_fields = ["username", "hash_algo", "length_bytes", "descr"];
    public array $tabs = [
        ["Settings", false, "/system_api_settings.php"],
        ["Keys", true, "/system_api_key.php"],
        ["Updates", false, "/system_api_updates.php"],
        ["Documentation", false, "/api/documentation/"]
    ];
}
