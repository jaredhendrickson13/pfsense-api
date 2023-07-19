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

class APIUserCreate extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added user via API";
    }

    public function action() {
        # Increase the system's next UID by one and add our user to the configuration
        $this->set_config("system/nextuid", intval($this->validated_data["uid"]) + 1);
        $this->id = $this->get_next_id("system/user");
        $this->set_config("system/user/{$this->id}", $this->validated_data);

        # Write the user to configuration and set the user on the backend. Return response with created user object.
        $this->write_config();
        local_user_set($this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_username() {
        # Check for our required `username` payload value
        if (isset($this->initial_data['username'])) {
            # Ensure a user with this username does not already exist
            if (!array_key_exists($this->initial_data['username'], index_users())) {
                # Ensure the username does not contain invalid characters
                if (!preg_match("/[^a-zA-Z0-9\.\-_]/", $this->initial_data['username'])) {
                    # Ensure username is not reserved by the system
                    if (!$this->is_username_reserved($this->initial_data["username"])) {
                        # Ensure username is not longer that 32 characters
                        if (strlen($this->initial_data["username"]) <= 32) {
                            $this->validated_data["name"] = $this->initial_data['username'];
                        } else {
                            $this->errors[] = APIResponse\get(5038);
                        }
                    } else {
                        $this->errors[] = APIResponse\get(5037);
                    }
                } else {
                    $this->errors[] = APIResponse\get(5036);
                }
            } else {
                $this->errors[] = APIResponse\get(5002);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
    }

    private function __validate_password() {
        # Check for our required `password` payload value
        if (isset($this->initial_data['password'])) {
            # Generate the password hash and add it to our validated data
            local_user_set_password($this->validated_data, $this->initial_data['password']);
        } else {
            $this->errors[] = APIResponse\get(5003);
        }
    }

    private function __validate_priv() {
        global $priv_list;
        $this->validated_data["priv"] = [];

        # Check for our optional `priv` payload value
        if ($this->initial_data["priv"]) {
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
            # Try to format the date string, return an error if the format is invalid
            try {
                $this->validated_data["expires"] = (new DateTime($this->initial_data['expires']))->format("m/d/Y");
            } catch (Exception $e) {
                $this->errors[] = APIResponse\get(5040);
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

    public function validate_payload() {
        # Set static object values
        $this->validated_data["uid"] = $this->get_config("system/nextuid");
        $this->validated_data["scope"] = "user";

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

    public function is_username_reserved($user) {
        # Open the /etc/passwd file to read all system users
        $sys_users = explode(PHP_EOL, file_get_contents("/etc/passwd"));

        # Loop through each system user and check if the username is reserved
        foreach ($sys_users as $sys_user_ent) {
            $sys_username = explode(":", $sys_user_ent)[0];
            if ($sys_username == $user) {
                return true;
            }
        }
        return false;
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
