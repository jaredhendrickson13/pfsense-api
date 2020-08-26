<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemCertificatesAdd extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system-certmanager"];
        $this->change_note = "Added certificate via API";
    }

    public function action() {
        $this->config["cert"][] = $this->validated_data;    // Add our configuration
        $this->write_config();    // Apply our configuration change
        // Restart our webgui if user set this cert as active
        if ($this->initial_data["active"] === true) {
            APITools\restart_webconfigurator();
        }
        // Print our JSON response
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
        } else {
            $this->errors[] = APIResponse\get(1002);
        }
        if (isset($this->initial_data['method'])) {
            // Check what method was picked
            if ($this->initial_data['method'] === "import") {
                $cert = base64_decode($this->initial_data["cert"]);
                $key = base64_decode($this->initial_data["key"]);
                // Check if our certificate and key are valid
                if (!strstr($cert, "BEGIN CERTIFICATE") || !strstr($cert, "END CERTIFICATE")) {
                    $this->errors[] = APIResponse\get(1003);
                } elseif (cert_get_publickey($cert, false) != cert_get_publickey($key, false, 'prv')) {
                    $this->errors[] = APIResponse\get(1004);
                }
                // Populate our configuration array
                $this->validated_data["refid"] = uniqid();
                $this->validated_data["type"] = "server";
                $this->validated_data["descr"] = $descr;
                cert_import($this->validated_data, $cert, $key);
                // If user requests this cert to be active, configure it as the wc cert
                if ($this->initial_data["active"] === true) {
                    $this->config["system"]["webgui"]["ssl-certref"] = $this->validated_data["refid"];
                }
            }
        }
    }
}
