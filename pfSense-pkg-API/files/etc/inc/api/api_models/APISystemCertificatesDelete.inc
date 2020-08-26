<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemCertificatesDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system-certmanager"];
        $this->change_note = "Deleted certificate via API";
    }

    public function action() {
        $cert_del = $this->config["cert"][$this->validated_data["id"]];    // Save our cert we are deleting
        if (!empty($cert_del)) {
            // Check if our certificate is currently in use
            if ($this->config["system"]["webgui"]["ssl-certref"] !== $cert_del["refid"]) {
                unset($this->config["cert"][$this->validated_data["id"]]);    // Delete our cert
                $this->write_config();    // Apply our configuration change
            } else {
                return APIResponse\get(1005);
            }
        } else {
            return APIResponse\get(1009);
        }

        return APIResponse\get(0, $cert_del);
    }

    public function validate_payload() {
        if (isset($this->initial_data['refid'])) {
            $refid = $this->initial_data['refid'];
            // Loop through our certificates and find a match
            foreach ($this->config["cert"] as $i => $c) {
                if ($c["refid"] === $refid) {
                    $this->validated_data["id"] = $i;
                    break;
                }
            }
        } elseif (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
            // Loop through our certificates and find a match
            foreach ($this->config["cert"] as $i => $c) {
                if ($c["descr"] === $descr) {
                    $this->validated_data["id"] = $i;
                    break;
                }
            }
        } elseif (isset($this->initial_data['id'])) {
            $this->validated_data["id"] = $this->initial_data['id'];
        }
    }
}
