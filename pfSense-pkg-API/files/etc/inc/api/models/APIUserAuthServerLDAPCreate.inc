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

class APIUserAuthServerLDAPCreate extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added LDAP authentication server via API";
    }

    public function action() {
        $this->id = $this->get_next_id("system/authserver");
        $this->set_config("system/authserver/{$this->id}", $this->validated_data);
        // Check if clients wants to set this as default auth server
        if ($this->initial_data["active"] === true) {
            $this->set_config('system/webgui/authmode', $this->validated_data["name"]);    // Add default auth server
        }
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $ldap_count = count($this->get_config("system/authserver", []));    // Save our total number of auth servers
        $this->validated_data = ["refid" => uniqid(), "type" => "ldap"];    // Init our new LDAP config
        if (isset($this->initial_data['name'])) {
            $this->validated_data["name"] = APITools\sanitize_str($this->initial_data['name']);
            if (APITools\is_authentication_server($this->validated_data["name"])) {
                $this->errors[] = APIResponse\get(5026);
            }
        } else {
            while (true) {
                $this->validated_data["name"] = "LDAP_AUTHSERVER_" . strval($ldap_count);
                // Check if auto name is already used
                if (APITools\is_authentication_server($this->validated_data["name"])) {
                    $ldap_count++;
                } else {
                    break;
                }
            }
        }
        if (isset($this->initial_data['host'])) {
            $this->validated_data["host"] = $this->initial_data['host'];
            // Check if our hostname is valid
            if (!is_hostname($this->validated_data["host"]) and !is_ipaddrv4($this->validated_data["host"])) {
                $this->errors[] = APIResponse\get(5012);
            }
        } else {
            $this->errors[] = APIResponse\get(5011);
        }
        if (($this->initial_data['ldap_port'])) {
            $this->validated_data["ldap_port"] = strval($this->initial_data['ldap_port']);
            // Check if our hostname is valid
            if (!is_port($this->validated_data["ldap_port"])) {
                $this->errors[] = APIResponse\get(5014);
            }
        } else {
            $this->errors[] = APIResponse\get(5013);

        }
        if (isset($this->initial_data['ldap_urltype'])) {
            // Assign default config values
            $allowed_url_types = array(
                "standard" => "Standard TCP",
                "starttls" => "STARTTLS Encrypted",
                "encrypted" => "SSL/TLS Encrypted"
            );

            // Check if our URL type is allowed
            if (!array_key_exists($this->initial_data["ldap_urltype"], $allowed_url_types)) {
                $this->errors[] = APIResponse\get(5016);
            }
            $this->validated_data["ldap_urltype"] = $allowed_url_types[$this->initial_data['ldap_urltype']];
        } else {
            $this->errors[] = APIResponse\get(5015);
        }
        if (isset($this->initial_data['ldap_protver'])) {
            $this->validated_data["ldap_protver"] = intval($this->initial_data['ldap_protver']);
            // Check if our LDAP version is valid
            $allowed_ldap_ver = array(2, 3);    // Array of allowed LDAP protocol versions
            if (!in_array($this->validated_data["ldap_protver"], $allowed_ldap_ver)) {
                $this->errors[] = APIResponse\get(5017);
            }
        } else {
            $this->validated_data["ldap_protver"] = 3;
        }
        if (isset($this->initial_data['ldap_timeout'])) {
            $this->validated_data["ldap_timeout"] = intval($this->initial_data['ldap_timeout']);
        } else {
            $this->validated_data["ldap_timeout"] = 25;    // Assign default if not specified
        }
        if (isset($this->initial_data['ldap_scope'])) {
            $this->validated_data["ldap_scope"] = $this->initial_data['ldap_scope'];
            // Check if our LDAP scope is valid
            $allowed_ldap_scopes = array("one", "subtree");    // Array of allowed LDAP scopes
            if (!in_array($this->validated_data["ldap_scope"], $allowed_ldap_scopes)) {
                $this->errors[] = APIResponse\get(5019);
            }
        } else {
            $this->errors[] = APIResponse\get(5018);
        }
        if (isset($this->initial_data['ldap_basedn'])) {
            $this->validated_data["ldap_basedn"] = strval($this->initial_data['ldap_basedn']);
        } else {
            $this->validated_data["ldap_basedn"] = "";
        }
        if (isset($this->initial_data['ldap_authcn'])) {
            $this->validated_data["ldap_authcn"] = strval($this->initial_data['ldap_authcn']);
        } else {
            $this->validated_data["ldap_authcn"] = "";
        }
        if (isset($this->initial_data['ldap_extended_query'])) {
            $this->validated_data["ldap_extended_enabled"] = "yes";
            $this->validated_data["ldap_extended_query"] = strval($this->initial_data['ldap_extended_query']);
        }
        if (!isset($this->initial_data['ldap_binddn']) and !isset($this->initial_data['ldap_bindpw'])) {
            $bind_anon = true;
        }
        if (!$bind_anon) {
            if (isset($this->initial_data['ldap_binddn'])) {
                $this->validated_data["ldap_binddn"] = strval($this->initial_data['ldap_binddn']);
            } else {
                $this->errors[] = APIResponse\get(5020);
            }
            if (isset($this->initial_data['ldap_bindpw'])) {
                $this->validated_data["ldap_bindpw"] = strval($this->initial_data['ldap_bindpw']);
            } else {
                $this->errors[] = APIResponse\get(5021);
            }
        }
        if (isset($this->initial_data['ldap_attr_user'])) {
            $this->validated_data["ldap_attr_user"] = strval($this->initial_data['ldap_attr_user']);
        } else {
            $this->errors[] = APIResponse\get(5022);
        }
        if (isset($this->initial_data['ldap_attr_group'])) {
            $this->validated_data["ldap_attr_group"] = strval($this->initial_data['ldap_attr_group']);
        } else {
            $this->errors[] = APIResponse\get(5023);
        }
        if (isset($this->initial_data['ldap_attr_member'])) {
            $this->validated_data["ldap_attr_member"] = strval($this->initial_data['ldap_attr_member']);
        } else {
            $this->errors[] = APIResponse\get(5024);
        }
        if (isset($this->initial_data['ldap_attr_groupobj'])) {
            $this->validated_data["ldap_rfc2307"] = "";    // Enable RFC2307 mode
            $this->validated_data["ldap_attr_groupobj"] = strval($this->initial_data['ldap_attr_groupobj']);
        }
        if ($this->initial_data['ldap_utf8'] === true) {
            $this->validated_data["ldap_utf8"] = "";    // Enable UTF8 LDAP parameters
        }
        if ($this->initial_data['ldap_nostrip_at'] === true) {
            $this->validated_data["ldap_nostrip_at"] = "";    // Disable LDAP username alterations (@)
        }
        if ($this->initial_data['active'] === true) {
            $this->initial_data["active"] = true;
        }
    }
}
