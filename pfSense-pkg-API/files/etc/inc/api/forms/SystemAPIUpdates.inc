<?php

namespace api\forms;

require_once("api/auto_loader.inc");

use API\Core\Form;

class SystemAPIUpdates extends Form {
    public string $url = "/system_api_updates.php";
    public string $model_name = "\API\Models\APIVersion";
    public array $title_path = ["System", "API", "Updates"];
    public string $custom_css = "/css/api/api.css";
    public array $tabs = [
        ["Settings", false, "/system_api_settings.php"],
        ["Keys", false, "/system_api_key.php"],
        ["Updates", true, "/system_api_updates.php"],
        ["Documentation", false, "/api/documentation/"]
    ];
    public array $sections = [
        "Version" => [
            "fields" => ["current_version", "latest_version", "latest_version_release_date"]
        ],
        "Install a different version" => [
            "fields" => ["install_version"]
        ]
    ];
}
