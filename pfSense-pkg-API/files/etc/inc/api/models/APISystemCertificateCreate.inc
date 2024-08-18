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


class APISystemCertificateCreate extends APIModel {
    public $cert_method;
    public $cert_attr;
    public $dn;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-certmanager"];
        $this->change_note = "Added certificate via API";
        $this->cert_method = null;
        $this->cert_attr = [];
        $this->dn = [];
    }

    public function action() {
        # Add the new certificate
        $this->id = $this->get_next_id("cert");
        $this->set_config("cert/{$this->id}",  $this->validated_data);

        # If user requests this cert to be active, configure it as the wc cert
        if ($this->initial_data["active"] === true and $this->validated_data["type"] === "server") {
            $this->set_config("system/webgui/ssl-certref", $this->validated_data["refid"]);
        }

        # Write changes to configuration
        $this->write_config();

        # Restart our webgui if user set this cert as active
        if ($this->initial_data["active"] === true) {
            APITools\restart_webconfigurator();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Validate shared fields
        $this->validated_data["refid"] = uniqid();
        $this->__validate_method();
        $this->__validate_descr();

        # Validate 'existing' method fields
        if ($this->cert_method === "existing") {
            $this->__validate_crt();
            $this->__validate_prv();
        }

        # Validate 'internal' method fields
        if ($this->cert_method === "internal") {
            $this->__validate_caref();
            $this->__validate_keytype();
            $this->__validate_keylen();
            $this->__validate_ecname();
            $this->__validate_digest_alg();
            $this->__validate_lifetime();
            $this->__validate_dn_commonname();
            $this->__validate_dn_country();
            $this->__validate_dn_state();
            $this->__validate_dn_city();
            $this->__validate_dn_organization();
            $this->__validate_dn_organizationalunit();
            $this->__validate_type();
            $this->__validate_altnames();
        }

        # Create the certificate
        $this->create_certificate();
    }

    private function __validate_method() {
        # Local variables
        $methods = ["import", "existing", "internal"];

        if (isset($this->initial_data['method'])) {
            # Ensure the method is a supported one
            if (in_array($this->initial_data["method"], $methods)) {
                $this->cert_method = $this->initial_data["method"];

                # Rewrite the import method to existing to be consistent with pfSense's UI
                if ($this->cert_method === "import") {
                    $this->cert_method = "existing";
                }
            } else {
                $this->errors[] = APIResponse\get(1032);
            }
        } else {
            $this->errors[] = APIResponse\get(1031);
        }
    }

    private function __validate_descr() {
        # Validate the required 'descr' field
        if (isset($this->initial_data['descr'])) {
            # Ensure description does not contain invalid characters
            if (preg_match("/[\?\>\<\&\/\\\"\']/", $this->initial_data['descr'])) {
                $this->errors[] = APIResponse\get(1037);
            } else {
                $this->validated_data["descr"] = $this->initial_data['descr'];
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
                $this->validated_data["crt"] = $crt;
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
                $this->validated_data["prv"] = $key;
            }
        } else {
            $this->errors[] = APIResponse\get(1004);
        }
    }

    private function __validate_caref() {
        # Validate the required 'caref' field
        if (isset($this->initial_data['caref'])) {
            # Ensure CA ref ID exists
            if (in_array($this->initial_data['caref'], $this->get_all_ca())) {
                $this->validated_data["caref"] = $this->initial_data["caref"];
            } else {
                $this->errors[] = APIResponse\get(1048);
            }
        } else {
            $this->errors[] = APIResponse\get(1047);
        }
    }

    private function __validate_keytype() {
        # Local variables
        $key_types = ["RSA", "ECDSA"];

        # Validate the required 'keytype' field
        if (isset($this->initial_data['keytype'])) {
            # Ensure keytype is known
            if (in_array(strtoupper($this->initial_data["keytype"]), $key_types)) {
                $this->cert_attr["keytype"] = strtoupper($this->initial_data["keytype"]);
            } else {
                $this->errors[] = APIResponse\get(1039);
            }
        } else {
            $this->errors[] = APIResponse\get(1038);
        }
    }

    private function __validate_keylen() {
        # Local variables
        $key_lengths = [1024, 2048, 3072, 4096, 6144, 7680, 8192, 15360, 16384];

        # Validate the conditional 'keylen' field when the keytype is set to RSA
        if ($this->cert_attr["keytype"] === "RSA") {
            if (isset($this->initial_data['keylen'])) {
                # Ensure key length is supported
                if (in_array(intval($this->initial_data["keylen"]), $key_lengths)) {
                    $this->cert_attr["keylen"] = intval($this->initial_data["keylen"]);
                } else {
                    $this->errors[] = APIResponse\get(1041);
                }
            } else {
                $this->errors[] = APIResponse\get(1040);
            }
        }
    }

    private function __validate_ecname() {
        # Validate the conditional 'ecname' field when the keytype is set to ECDSA
        if ($this->cert_attr["keytype"] === "ECDSA") {
            if (isset($this->initial_data['ecname'])) {
                # Ensure curve is supported
                if (in_array($this->initial_data["ecname"], array_keys(cert_build_curve_list()))) {
                    $this->cert_attr["ecname"] = $this->initial_data["ecname"];
                } else {
                    $this->errors[] = APIResponse\get(1043);
                }
            } else {
                $this->errors[] = APIResponse\get(1042);
            }
        }
    }

    private function __validate_digest_alg() {
        global $openssl_digest_algs;

        # Validate the required 'digest_alg' field
        if (isset($this->initial_data['digest_alg'])) {
            # Ensure digest is supported
            if (in_array($this->initial_data["digest_alg"], $openssl_digest_algs)) {
                $this->cert_attr["digest_alg"] = $this->initial_data["digest_alg"];
            } else {
                $this->errors[] = APIResponse\get(1045);
            }
        } else {
            $this->errors[] = APIResponse\get(1044);
        }
    }

    private function __validate_lifetime() {
        # Validate the optional 'lifetime' field
        if (isset($this->initial_data['lifetime'])) {
            # Ensure lifetime is within range
            if (is_numeric($this->initial_data["lifetime"]) and intval($this->initial_data["lifetime"]) < cert_get_max_lifetime()) {
                $this->cert_attr["lifetime"] = intval($this->initial_data["lifetime"]);
            } else {
                $this->errors[] = APIResponse\get(1046);
            }
        } else {
            $this->cert_attr["lifetime"] = 3650;
        }
    }

    private function __validate_dn_commonname() {
        # Validate the optional 'dn_commonname' field
        if (isset($this->initial_data['dn_commonname'])) {
            $this->dn["commonName"] = strval($this->initial_data['dn_commonname']);
        } else {
            $this->errors[] = APIResponse\get(1052);
        }
    }

    private function __validate_dn_country() {
        # Validate the optional 'dn_country' field
        if (isset($this->initial_data['dn_country'])) {
            # Ensure country is known
            if (in_array($this->initial_data["dn_country"], array_keys(get_cert_country_codes()))) {
                $this->dn["countryName"] = strval($this->initial_data['dn_country']);
            } else {
                $this->errors[] = APIResponse\get(1051);
            }
        }
    }

    private function __validate_dn_state() {
        # Validate the optional 'dn_state' field
        if (isset($this->initial_data['dn_state'])) {
            $this->dn["stateOrProvinceName"] = strval($this->initial_data['dn_state']);
        }
    }

    private function __validate_dn_city() {
        # Validate the optional 'dn_city' field
        if (isset($this->initial_data['dn_city'])) {
            $this->dn["localityName"] = strval($this->initial_data['dn_city']);
        }
    }

    private function __validate_dn_organization() {
        # Validate the optional 'dn_organization' field
        if (isset($this->initial_data['dn_organization'])) {
            $this->dn["organizationName"] = strval($this->initial_data['dn_organization']);
        }
    }

    private function __validate_dn_organizationalunit() {
        # Validate the optional 'dn_organizationalunit' field
        if (isset($this->initial_data['dn_organizationalunit'])) {
            $this->dn["organizationalUnitName"] = strval($this->initial_data['dn_organizationalunit']);
        }
    }

    private function __validate_type() {
        # Local variables
        $types = ["server", "user"];

        # Validate the required 'type' field
        if (isset($this->initial_data['type'])) {
            # Check if the certificate type is valid
            if (in_array($this->initial_data["type"], $types)) {
                $this->validated_data["type"] = $this->initial_data["type"];
            } else {
                $this->errors[] = APIResponse\get(1054);
            }
        } else {
            $this->errors[] = APIResponse\get(1053);
        }
    }

    private function __validate_altnames() {
        # Local variables
        $this->dn["subjectAltName"] = [];

        # Validate the optional 'altnames' field
        if (isset($this->initial_data['altnames'])) {
            # Ensure this valud is an array
            if (is_array($this->initial_data["altnames"])) {
                # Loop through each requested alternative name and ensure it is valid
                foreach ($this->initial_data["altnames"] as $altname_items) {
                    foreach ($altname_items as $altname_type=>$altname) {
                        # Validate FQDN or hostname alternative names
                        if (strtolower($altname_type) === "dns") {
                            # Require dns altnames to be valid FQDN/hostname and NOT IPs
                            if (is_hostname($altname, true) and !is_ipaddr($altname)) {
                                $this->dn["subjectAltName"][] = "DNS:".$altname;
                            } else {
                                $this->errors[] = APIResponse\get(1057);
                            }
                        }
                        # Validate IP alternative names
                        elseif (strtolower($altname_type) === "ip") {
                            # Require ip altnames to be valid IP address
                            if (is_ipaddr($altname)) {
                                $this->dn["subjectAltName"][] = "IP:".$altname;
                            } else {
                                $this->errors[] = APIResponse\get(1058);
                            }
                        }
                        # Validate URI alternative names
                        elseif (strtolower($altname_type) === "uri") {
                            # Require URI altnames to be valid URI
                            if (!is_URL($altname)) {
                                $this->errors[] = APIResponse\get(1059);
                            } else {
                                $this->dn["subjectAltName"][] = "URI:".$altname;
                            }
                        }
                        # Validate email alternative names
                        elseif (strtolower($altname_type) === "email") {
                            # Require email altnames to be valid email
                            if (empty($altname) or preg_match("/[\!\#\$\%\^\(\)\~\?\>\<\&\/\\\,\"\']/", $altname)) {
                                $this->errors[] = APIResponse\get(1059);
                            } else {
                                $this->dn["subjectAltName"][] = "email:".$altname;
                            }
                        }
                        # Return alternative name type unknown error
                        else {
                            $this->errors[] = APIResponse\get(1056);
                        }
                    }
                }
                # Piece together the subjectAltName x509 data string
                $this->dn["subjectAltName"] = implode(",", $this->dn["subjectAltName"]);
            } else {
                $this->errors[] = APIResponse\get(1055);
            }
        }
    }

    public function get_all_ca() {
        # Local variables
        $ca_enties = [];

        # Loop through each CA and add all CAs that have a valid key
        foreach ($this->get_config('ca', []) as $ca) {
            if ($ca['prv']) {
                $ca_enties[] = $ca['refid'];
            }
        }

        return $ca_enties;
    }

    public function create_certificate() {
        # Only import a new certificate if no errors were found and the method was existing
        if (empty($this->errors) and $this->cert_method === "existing") {
            cert_import($this->validated_data, $this->validated_data['crt'], $this->validated_data['prv']);
        }

        # Only generate a new certificate if no errors were found and the method was internal
        if (empty($this->errors) and $this->cert_method === "internal") {
            $openssl_create = cert_create(
                $this->validated_data,
                $this->validated_data["caref"],
                $this->cert_attr['keylen'],
                $this->cert_attr['lifetime'],
                $this->dn,
                $this->validated_data["type"],
                $this->cert_attr['digest_alg'],
                $this->cert_attr['keytype'],
                $this->cert_attr['ecname']
            );

            # Check if OpenSSL failed to generate the certificate
            if (!$openssl_create) {
                $this->errors[] = APIResponse\get(1050);
            }
        }
    }
}
