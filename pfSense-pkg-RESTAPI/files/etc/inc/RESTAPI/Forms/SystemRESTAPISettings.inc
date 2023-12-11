<?php

namespace RESTAPI\Forms;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Auth;
use RESTAPI\Core\Form;
use RESTAPI\Models\RESTAPIJWT;

class SystemRESTAPISettings extends Form {
    public string $url = "/system_restapi_settings.php";
    public string $model_name = "\RESTAPI\Models\RESTAPISettings";
    public array $title_path = ["System", "API", "Settings"];
    public string $custom_css = "/css/RESTAPI/RESTAPI.css";
    public array $tabs = [
        ["Settings", true, "/system_restapi_settings.php"],
        ["Keys", false, "/system_restapi_key.php"],
        ["Updates", false, "/system_restapi_updates.php"],
        ["Documentation", false, "/RESTAPI/documentation/"]
    ];
    public array $sections = [
        "General" => [
            "Fields" => ["enabled", "keep_backup", "represent_interfaces_as"]
        ],
        "Security" => [
            "Fields" => [
                "allowed_interfaces",
                "read_only",
                "auth_mode",
                "jwt_exp",
                "login_protection",
                "log_successful_auth",
            ]
        ],
        "Advanced Settings" => [
            "Fields" => [
                "allow_pre_releases",
                "ha_sync",
                "ha_sync_validate_certs",
                "ha_sync_hosts",
                "ha_sync_username",
                "ha_sync_password"
            ]
        ]
    ];
    public array $buttons = [
        "rotate_server_key" => [
            "title" => "Rotate Server Key",
            "icon" => "fa-repeat",
            "classes" => ["btn-success", "btn-sm"],
            "on_click" => "return confirm(\"Rotating the server key will void any existing JWTs. Proceed?\");",
            "callable" => "rotate_server_key"
        ],
        "report_an_issue" => [
            "title" => "Report an Issue",
            "icon" => "fa-question-circle",
            "link" => "https://github.com/jaredhendrickson13/pfsense-api/issues/new/choose",
            "classes" => ["btn-info"]
        ]
    ];

    /**
     * A button callable method that rotates the JWT server key when the `rotate_server_key` button is pressed.
     * @throws \Exception When the JWT server key fails to be generated.
     */
    public function rotate_server_key() {
        RESTAPIJWT::init_server_key(rotate: true);
        $this->print_success_banner(message: "The JWT server key has been rotated.");
    }
}
