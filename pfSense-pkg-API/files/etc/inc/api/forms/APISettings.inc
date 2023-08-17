<?php

namespace API\Forms;

require_once("api/auto_loader.inc");

use API\Core\ModelForm;

class APISettings extends ModelForm
{
    public string $url = "/api/settings/";
    public string $model_name = "\API\Models\APISettings";
    public array $title_path = ["System", "API", "Settings"];
    public array $tabs = [["Settings", true, "/api/settings/"], ["Documentation", false, "/api/documentation/"], ["Updates", false, "/api/update/"]];
    public array $sections = [
        "General" => [
            "fields" => ["enabled", "keep_backup", "represent_interfaces_as"]
        ],
        "Security" => [
            "fields" => [
                "allowed_interfaces",
                "read_only",
                "auth_mode",
                "jwt_exp",
                "key_hash_algo",
                "key_length_bytes",
                "login_protection",
                "log_successful_auth",
            ]
        ],
        "Advanced Settings" => [
            "fields" => [
                "allow_pre_releases",
                "ha_sync",
                "ha_sync_validate_certs",
                "ha_sync_hosts",
                "ha_sync_username",
                "ha_sync_password"
            ]
        ]
    ];
}
