<?php
//   Copyright 2022 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemCertificateDelete extends APIModel {
    public $cert_ref_id;
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-certmanager"];
        $this->change_note = "Deleted certificate via API";
        $this->cert_ref_id = "";
    }

    public function action() {
        # Remove the certificate from configuration
        $this->del_config("cert/{$this->id}");
        $this->write_config();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_refid();
        $this->__validate_descr();
        $this->__validate_id();
    }

    private function __validate_refid() {
        # Check for our conditional 'refid' field
        if (isset($this->initial_data['refid'])) {
            # Loop through our certificates and find a match
            foreach ($this->get_config("cert", []) as $i => $c) {
                if ($c["refid"] === $this->initial_data['refid']) {
                    $this->id = $i;
                    $this->cert_ref_id = $this->initial_data['refid'];
                    break;
                }
            }
        }
    }

    private function __validate_descr() {
        # Check for our conditional 'descr' field. Only validate this field if the ID was not found by ref ID previously
        if (isset($this->initial_data['descr']) and !isset($this->id)) {
            # Loop through our certificates and find a match
            foreach ($this->get_config("cert", []) as $i => $c) {
                if ($c["descr"] === $this->initial_data['descr']) {
                    $this->id = $i;
                    $this->cert_ref_id = $c["refid"];
                    break;
                }
            }
        }
    }

    private function __validate_id() {
        # Check our conditional 'id' field.
        if (isset($this->initial_data['id'])) {
            # Check if this ID exists
            if ($this->get_config("cert/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data['id'];
                $this->cert_ref_id = $this->get_config("cert/{$this->id}/refid");
            } else {
                $this->errors[] = APIResponse\get(1009);
            }
        }

        # Ensure that the certs index ID was identified by one of the locators and is not in use.
        if (isset($this->id) and isset($this->cert_ref_id)) {
            # Ensure certificate is not in use
            if (!cert_in_use($this->cert_ref_id)) {
                $this->validated_data = $this->get_config("cert/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(1005);
            }
        } else {
            $this->errors[] = APIResponse\get(1009);
        }
    }
}
