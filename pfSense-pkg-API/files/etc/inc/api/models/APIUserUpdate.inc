<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUserUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager"];
        $this->change_note = "Modified user via API";
    }

    public function action() {
        local_user_set($this->validated_data);
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['username'])) {
            $this->validated_data =& getUserEntry($this->initial_data['username']);
            // Check that our user already exists
            if (!array_key_exists("uid", $this->validated_data)) {
                $this->errors[] = APIResponse\get(5001);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
        if (isset($this->initial_data['password'])) {
            $password = $this->initial_data['password'];
            $password = trim($password);
            local_user_set_password($this->validated_data, $password);  // Set our new user's password
        }
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = "";    // Update our user's disabled value if not false
        } elseif ($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);    // Unset our disabled value if not requested

        }
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
            $descr = trim($descr);
            $this->validated_data["descr"] = $descr;    // Update our user's full name
        }
        if (isset($this->initial_data['expires'])) {
            $expires = $this->initial_data['expires'];
            $expires = trim($expires);
            $this->validated_data["expires"] = $expires;    // Update our user's expiration date
        }
        if (isset($this->initial_data['authorizedkeys'])) {
            $authorizedkeys = $this->initial_data['authorizedkeys'];
            $authorizedkeys = trim($authorizedkeys);
            $this->validated_data["authorizedkeys"] = $authorizedkeys;    // Update our user's authorized keys
        }
        if (isset($this->initial_data['ipsecpsk'])) {
            $ipsecpsk = $this->initial_data['ipsecpsk'];
            $ipsecpsk = trim($ipsecpsk);
            $this->validated_data["ipsecpsk"] = $ipsecpsk;    // Update our user's IPsec pre-shared key
        }
        $this->validated_data["scope"] = "user";    // Set our new user's system scope
    }
}