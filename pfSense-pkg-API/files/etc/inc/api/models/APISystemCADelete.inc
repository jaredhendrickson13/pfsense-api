<?php
//   Copyright 2023 Jared Hendrickson
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


class APISystemCADelete extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Deleted CA certificate via API";
    }

    public function action() {
        # Remove the CA from configuration and reapply the truststore
        $this->del_config("ca/{$this->id}");
        $this->write_config();
        ca_setup_trust_store();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_refid();
        $this->__validate_descr();

        # Return an error if there was no matching CA
        if (!isset($this->id)) {
            $this->errors[] = APIResponse\get(1009);
        }
        # Return an error if the CA is in use
        if (cert_in_use($this->validated_data["refid"])) {
            $this->errors[] = APIResponse\get(1034);
        }

    }

    private function __validate_refid() {
        # Check for our optional 'refid' value
        if (isset($this->initial_data['refid'])) {
            # Loop through our certificates and find a match
            foreach ($this->get_config("ca", []) as $i => $c) {
                if ($c["refid"] === $this->initial_data["refid"]) {
                    $this->id = $i;
                    $this->validated_data = $this->get_config("ca/{$this->id}");
                    break;
                }
            }
        }
    }

    private function __validate_descr() {
        # Check for our optional 'descr' value if a refid was not specified
        if (isset($this->initial_data['descr']) and !isset($this->initial_data["refid"])) {
            # Loop through our certificates and find a match
            foreach ($this->get_config("ca", []) as $i => $c) {
                if ($c["descr"] === $this->initial_data["descr"]) {
                    $this->id = $i;
                    $this->validated_data = $this->get_config("ca/{$this->id}");
                    break;
                }
            }
        }
    }
}
