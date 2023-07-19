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

class APIUserUpdate extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Modified user via API";
    }

    public function action() {
        # Update our new user in the config and set the user on the backend
        $this->set_config("system/user/{$this->id}", $this->validated_data);
        $this->write_config();
        local_user_set($this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Run each validation method
        $this->__validate_username();
        $this->__validate_password();
        $this->__validate_priv();
        $this->__validate_descr();
        $this->__validate_disabled();
        $this->__validate_expires();
        $this->__validate_cert();
        $this->__validate_authorizedkeys();
        $this->__validate_ipsecpsk();
    }

    private function __validate_username() {
        # Check for our required `username` payload value
        if (isset($this->initial_data['username'])) {
            # Loop through each configured user and check if this user exists
            foreach ($this->get_config("system/user", []) as $id=>$user) {
                if ($this->initial_data["username"] === $user["name"]) {
                    $this->validated_data = $user;
                    $this->id = intval($id);
                }
            }
            # Set an error if no user was found
            if (!isset($this->validated_data["uid"])) {
                $this->errors[] = APIResponse\get(5001);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
    }

    private function __validate_password() {
        # Check for our optional `password` payload value
        if (isset($this->initial_data['password'])) {
            # Generate the password hash and add it to our validated data
            local_user_set_password($this->validated_data, $this->initial_data['password']);
        }
    }

    private function __validate_priv() {
        global $priv_list;

        # Check for our optional `priv` payload value
        if (isset($this->initial_data["priv"])) {
            # Revert priv array to default
            $this->validated_data["priv"] = [];

            # Ensure value is an array
            if (!is_array($this->initial_data["priv"])) {
                $this->initial_data["priv"] = array($this->initial_data["priv"]);
            }

            # Loop through each requested privilege and ensure it exists
            foreach ($this->initial_data["priv"] as $priv) {
                if (array_key_exists($priv, $priv_list)) {
                    $this->validated_data["priv"][] = $priv;
                    $this->validated_data["priv"] = array_unique($this->validated_data["priv"]);
                } else {
                    $this->errors[] = APIResponse\get(5006);
                    break;
                }
            }
        }
    }

    private function __validate_disabled() {
        # Check for our optional `disabled` payload value
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = "";
        } elseif($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    private function __validate_descr() {
        # Check for our optional `descr` payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
    }

    private function __validate_expires() {
        # Check for our optional `expires` payload value
        if (isset($this->initial_data['expires'])) {
            # Attempt to format the expiration date if the value is not empty
            if (!empty($this->initial_data["expires"])) {
                # Try to format the date string, return an error if the format is invalid
                try {
                    $this->validated_data["expires"] = (new DateTime($this->initial_data['expires']))->format("m/d/Y");
                } catch (Exception $e) {
                    $this->errors[] = APIResponse\get(5040);
                }
            }
            # Otherwise, if the value was blank, unset the expiration date
            else {
                unset($this->validated_data["expires"]);
            }
        }
    }

    private function __validate_cert() {
        # Check for our optional `cert` payload value
        if (isset($this->initial_data["cert"])) {
            # Default validated user certificates
            $this->validated_data["cert"] = [];

            # Convert single values to array
            if (!is_array($this->initial_data["cert"])) {
                $this->initial_data["cert"] = [$this->initial_data["cert"]];
            }

            # Loop through requested user certificates and validate accordingly
            foreach ($this->initial_data["cert"] as $cert) {
                # Check if this cert is a user certificate
                if ($this->is_user_certificate($cert)) {
                    $this->validated_data["cert"][] = $cert;
                } else {
                    $this->errors[] = APIResponse\get(5041);
                    break;
                }
            }
        }
    }

    private function __validate_authorizedkeys() {
        # Check for our optional `authorizedkeys` payload value
        if (isset($this->initial_data['authorizedkeys'])) {
            $this->validated_data["authorizedkeys"] = base64_encode($this->initial_data['authorizedkeys']);
        }
    }

    private function __validate_ipsecpsk() {
        # Check for our optional `ipsecpsk` payload value
        if (isset($this->initial_data['ipsecpsk'])) {
            # Ensure the PSK does not contain invalid characters
            if (preg_match('/^[[:ascii:]]*$/', $_POST['ipsecpsk'])) {
                $this->validated_data["ipsecpsk"] = $this->initial_data['ipsecpsk'];
            } else {
                $this->errors[] = APIResponse\get(5039);
            }
        }
    }

    public function is_user_certificate($cert_ref_id) {
        # Loop through all certificates and check for matches
        foreach ($this->get_config("cert", []) as $cert) {
            # Check if this certificate matches the specified ref id
            if ($cert["refid"] === $cert_ref_id) {
                return true;
            }
        }

        return false;

    }
}
