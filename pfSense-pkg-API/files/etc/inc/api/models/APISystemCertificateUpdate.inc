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

class APISystemCertificateUpdate extends APIModel {

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-certmanager"];
        $this->change_note = "Modified system certificate address via API";
    }

    public function action() {
        $this->config["cert"][$this->id] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty("cert");

        # Only reload the firewall filter if a false value was not passed in
        # TODO: This condition applies the changes by default to stay backwards compatible with v1.3.0
        # TODO: this should be refactored in a future release to not apply by default
        #if ($this->initial_data["apply"] !== false) {
        #    APIFirewallApplyCreate::apply();
        #}
        return APIResponse\get(0, $this->config["cert"][$this->id]);
    }

    private function __validate_id() {
        # Validate optional 'refid' field containing the refid of an existing certificate. If refid is not passed, find the cert through 'descr' field
        if (isset($this->initial_data["refid"])) {
            # Loop through each cert and check for a match
            foreach ($this->config["cert"] as $id=>$cert) {
                # Check if the field 'refid' matches the certificate's refid
                if ($this->initial_data["refid"] === $cert["refid"]) {
                    $this->id = $id;
                    $this->validated_data = $cert;
                    break;
                }
            }
            # If we did not find an ID in the loop, return a not found error
            if (is_null($this->id)) {
                $this->errors[] = APIResponse\get(1009);
            }
        } 
    }

    private function __validate_descr() {
        # Validate the required 'descr' field
        if (isset($this->initial_data['descr'])) {
            # Ensure description does not contain invalid characters
            if (preg_match("/[\?\>\<\&\/\\\"\']/", $this->initial_data['descr'])) {
                $this->errors[] = APIResponse\get(1037);
            } else {
                # Match certificate by 'descr' field only when refid is not set
                if (isset($this->initial_data["refid"])) {
                    $this->validated_data["descr"] = $this->initial_data['descr'];
                } else {
                    # Loop through each cert and check for a match
                    foreach ($this->config["cert"] as $id=>$cert) {
                        # Check if the field 'descr' matches the certificate's description
                        if ($this->initial_data["descr"] === $cert["descr"]) {
                            $this->id = $id;
                            $this->validated_data["descr"] = $this->initial_data['descr'];
                            break;
                        }
                    }
                    # If we did not find an ID in the loop, return a not found error
                    if (is_null($this->id)) {
                        $this->errors[] = APIResponse\get(1009);
                    }
                }
            }
        } else {
            $this->errors[] = APIResponse\get(1002);
        }
    }

    private function __validate_crt() {
        # Check for our required 'crt' field
        if (isset($this->initial_data["crt"]) or isset($this->initial_data["cert"])) {
            # Convert 'cert' to 'crt' to remain backwards compatible
            if (isset($this->initial_data["cert"])) {
                $this->initial_data["crt"] = $this->initial_data["cert"];
            }

            # Decode certificate from base64 format
            $crt = base64_decode($this->initial_data["crt"]);

            # Check if our certificate is valid
            if (!strstr($crt, "BEGIN CERTIFICATE") || !strstr($crt, "END CERTIFICATE")) {
                $this->errors[] = APIResponse\get(1003);
            }
            else {
                # Certificate must be stored base64-encoded
                $this->validated_data["crt"] = base64_encode($crt);
            }
        } else {
            $this->errors[] = APIResponse\get(1003);
        }
    }

    private function __validate_prv() {
        # Check for our optional 'prv' field
        if (isset($this->initial_data["prv"]) or isset($this->initial_data["key"])) {
            # Convert 'key' to 'prv' to remain backwards compatible
            if (isset($this->initial_data["key"])) {
                $this->initial_data["prv"] = $this->initial_data["key"];
            }

            # Decode certificate from base64 format
            $crt = base64_decode($this->initial_data["crt"]);
            $key = base64_decode($this->initial_data["prv"]);

            # Check if this private key is encrypted
            if (strstr($key, "ENCRYPTED")) {
                $this->errors[] = APIResponse\get(1036);
            }
            # Check if our certificate and key matches
            elseif (cert_get_publickey($crt, false) != cert_get_publickey($key, false, 'prv')) {
                $this->errors[] = APIResponse\get(1049);
            }
            else {
                # key must be stored base64-encoded
                $this->validated_data["prv"] = base64_encode($key);
            }
        } else {

        }
    }

    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_descr();
        $this->__validate_crt();
        $this->__validate_prv();
    }

}
