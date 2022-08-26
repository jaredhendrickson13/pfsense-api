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

class APIServicesIPsecPhase1Update extends APIModel {
    public $old_remote_gateway;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->change_note = "Modified IPsec phase 1 via API";
    }

    public function action() {
        # Write this entry to configuration
        $this->__init_config();
        $this->config["ipsec"]["phase1"][$this->id] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty('ipsec');

        # Remove the previous route if the remote gateway changed
        if ($this->old_remote_gateway) {
            route_del($this->old_remote_gateway);
        }

        # Apply these changes if requested
        if ($this->initial_data["apply"] === true) {
            APIServicesIPsecApplyCreate::apply();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Get the IKE ID and determine fields that need to be revalidated
        $this->__validate_ikeid();
        $this->__set_fields_to_revalidate();

        # Validate individual fields. Do not change the order at which these are validated!
        $this->__validate_descr();
        $this->__validate_disabled();
        $this->__validate_iketype();
        $this->__validate_protocol();
        $this->__validate_interface();
        $this->__validate_remote_gateway();
        $this->__validate_authentication_method();
        $this->__validate_myid_type();
        $this->__validate_myid_data();
        $this->__validate_peerid_type();
        $this->__validate_peerid_data();
        $this->__validate_pre_shared_key();
        $this->__validate_certref();
        $this->__validate_caref();
        $this->__validate_mode();
        $this->__validate_rekey_time();
        $this->__validate_reauth_time();
        $this->__validate_rand_time();
        $this->__validate_lifetime();
        $this->__validate_startaction();
        $this->__validate_closeaction();
        $this->__validate_nat_traversal();
        $this->__validate_mobike();
        $this->__validate_splitconn();
        $this->__validate_prfselect_enable();
        $this->__validate_gw_duplicates();
        $this->__validate_ikeport();
        $this->__validate_nattport();
        $this->__validate_dpd_enable();
        $this->__validate_dpd_delay();
        $this->__validate_dpd_maxfail();
        $this->__validate_encryption();
    }

    private function __set_fields_to_revalidate() {
        # For fields that must be revalidated, default them to their current state if they are not updated
        # Revalidate the remote-gateway if it was not set, in case protocol was changed
        if (!isset($this->initial_data["remote-gateway"])) {
            $this->initial_data["remote-gateway"] = $this->validated_data["remote-gateway"];
        }

        # Revalidate the myid_data if it was not set, in case myid_type was changed.
        if (!isset($this->initial_data["myid_data"])) {
            $this->initial_data["myid_data"] = $this->validated_data["myid_data"];
        }

        # Revalidate the peerid_data if it was not set, in case peerid_type was changed
        if (!isset($this->initial_data["peerid_data"])) {
            $this->initial_data["peerid_data"] = $this->validated_data["peerid_data"];
        }

        # Revalidate the pre-shared-key field if it was not set, in case authentication_method was changed
        if (!isset($this->initial_data["pre-shared-key"])) {
            $this->initial_data["pre-shared-key"] = $this->validated_data["pre-shared-key"];
        }

        # Revalidate the certref field if it was not set, in case authentication_method was changed
        if (!isset($this->initial_data["certref"])) {
            $this->initial_data["certref"] = $this->validated_data["certref"];
        }

        # Revalidate the caref field if it was not set, in case authentication_method was changed
        if (!isset($this->initial_data["caref"])) {
            $this->initial_data["caref"] = $this->validated_data["caref"];
        }

        # Revalidate the mode field if it was not set, in case the iketype was changed
        if (!isset($this->initial_data["mode"])) {
            $this->initial_data["mode"] = $this->validated_data["mode"];
        }

        # Revalidate the lifetime field if it was not set, in case the rekey_time or reauth_time was changed
        if (!isset($this->initial_data["lifetime"])) {
            $this->initial_data["lifetime"] = $this->validated_data["lifetime"];
        }

        # Revalidate the startaction field if it was not set, in case the remote-address field was changed
        if (!isset($this->initial_data["startaction"])) {
            $this->initial_data["startaction"] = $this->validated_data["startaction"];
        }

        # Revalidate the nattport field if it was not set, in case the ikeport field was changed
        if (!isset($this->initial_data["nattport"])) {
            $this->initial_data["nattport"] = $this->validated_data["nattport"];
        }

        # Revalidate the dpd_delay field if it was not set, in case the dpd_enable field was changed
        if (!isset($this->initial_data["dpd_delay"])) {
            $this->initial_data["dpd_delay"] = $this->validated_data["dpd_delay"];
        }

        # Revalidate the dpd_maxfail field if it was not set, in case the dpd_enable field was changed
        if (!isset($this->initial_data["dpd_maxfail"])) {
            $this->initial_data["dpd_maxfail"] = $this->validated_data["dpd_maxfail"];
        }
    }

    private function __validate_ikeid() {
        # Validate the required 'ikeid' field
        if (isset($this->initial_data["ikeid"])) {
            # Attempt to get the config ID from this IKE ID
            $this->id = APITools\get_config_id_from_ikeid($this->initial_data["ikeid"]);
            $this->validated_data = $this->config["ipsec"]["phase1"][$this->id];

            # Return an error if this IKE ID was not found in any existing IPsec phase1 entry
            if ($this->id === false) {
                $this->errors[] = APIResponse\get(2208);
            }
        } else {
            $this->errors[] = APIResponse\get(2207);
        }
    }

    private function __validate_descr() {
        # Check for the optional 'descr' field
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = (string)$this->initial_data["descr"];
        }
    }

    private function __validate_disabled() {
        # Check for the optional 'disabled' field
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = true;
        }
        # Unset this field if set to false
        elseif ($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    private function __validate_iketype() {
        # Check for the optional 'iketype' field
        if (isset($this->initial_data["iketype"])) {
            # Ensure IKE type is known
            if (in_array($this->initial_data["iketype"], ["ikev1", "ikev2", "auto"])) {
                $this->validated_data["iketype"] = $this->initial_data["iketype"];
            }
            else {
                $this->errors[] = APIResponse\get(2160);
            }
        }
    }

    private function __validate_protocol() {
        # Check for the optional 'protocol' field
        if (isset($this->initial_data["protocol"])) {
            # Ensure protocol is known
            if (in_array($this->initial_data["protocol"], ["inet", "inet6", "both"])) {
                $this->validated_data["protocol"] = $this->initial_data["protocol"];
            }
            else {
                $this->errors[] = APIResponse\get(2162);
            }
        }
    }

    private function __validate_interface() {
        # Check for the optional 'interface' field
        if (isset($this->initial_data["interface"])) {
            # Check if this is an existing interface
            if (APITools\get_pfsense_if_id($this->initial_data["interface"])) {
                $this->validated_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            }
            # Check if this is an existing virtual IP
            elseif (APITools\is_vip($this->initial_data["interface"])) {
                $this->validated_data["interface"] = APITools\is_vip($this->initial_data["interface"]);
            }
            else {
                $this->errors[] = APIResponse\get(2164);
            }
        }
    }

    private function __validate_remote_gateway() {
        # Check for the optional 'remote-gateway' field
        if (isset($this->initial_data["remote-gateway"])) {
            # Ensure remote gateway is a valid IP address or hostname
            if (!is_ipaddr($this->initial_data["remote-gateway"]) and !is_fqdn($this->initial_data["remote-gateway"])) {
                $this->errors[] = APIResponse\get(2166);
            }
            # For IPv6 remote gateways, ensure the protocol is not 'inet'.
            elseif (is_ipaddrv6($this->initial_data["remote-gateway"]) and $this->validated_data["protocol"] === "inet") {
                $this->errors[] = APIResponse\get(2167);
            }
            # For IPv4 remote gateways, ensure the protocol is not 'inet6'.
            elseif (is_ipaddrv4($this->initial_data["remote-gateway"]) and $this->validated_data["protocol"] === "inet6") {
                $this->errors[] = APIResponse\get(2168);
            }
            # For domain name remote gateways, ensure the protocol is 'both'
            elseif (is_fqdn($this->initial_data["remote-gateway"]) and $this->validated_data["protocol"] !== "both") {
                $this->errors[] = APIResponse\get(2169);
            }
            # Ensure remote gateway is not already in use
            elseif ($this->is_ipsec_remote_gateway_in_use($this->initial_data["remote-gateway"])) {
                $this->errors[] = APIResponse\get(2170);
            }
            # Otherwise, the input is valid
            else {
                # Caputre the old remote gateway if the interface is not wan, so the old route can be removed
                if ($this->validated_data["interface"] !== "wan") {
                    if ($this->initial_data["remote-gateway"] !== $this->validated_data["remote-gateway"]) {
                        $this->old_remote_gateway = $this->validated_data["remote-gateway"];
                    }
                }

                $this->validated_data["remote-gateway"] = $this->initial_data["remote-gateway"];
            }
        }
    }

    private function __validate_authentication_method() {
        # Check for the required 'authentication_method' field
        if ($this->initial_data["authentication_method"]) {
            # Ensure value is a valid option
            if (in_array($this->initial_data["authentication_method"], ["pre_shared_key", "cert"])) {
                # Remove the cert settings if set to pre_shared_key
                if ($this->initial_data["authentication_method"] === "pre_shared_key") {
                    unset($this->validated_data["certref"]);
                    unset($this->validated_data["caref"]);
                }
                # Remove the pre-shared key settings if set to cert
                if ($this->initial_data["authentication_method"] === "cert") {
                    unset($this->validated_data["pre-shared-key"]);
                }

                # Set the new authentication method
                $this->validated_data["authentication_method"] = $this->initial_data["authentication_method"];
            }
            else {
                $this->errors[] = APIResponse\get(2172);
            }
        }
    }

    private function __validate_myid_type() {
        global $my_identifier_list;

        # Check for the required 'myid_type' field
        if (isset($this->initial_data["myid_type"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["myid_type"], $my_identifier_list)) {
                $this->validated_data["myid_type"] = $this->initial_data["myid_type"];
            }
            else {
                $this->errors[] = APIResponse\get(2174);
            }
        }
    }

    private function __validate_myid_data() {
        # Create a shortname variable for myid_type for readability
        $myid_type = $this->validated_data["myid_type"];

        # Check for the conditional 'myid_data' field. Validate this field further if myid_type is not myaddress
        if (isset($this->initial_data["myid_data"]) and $myid_type !== "myaddress") {
            # Create a shortname variable for my myid_data for readability
            $myid_data = $this->initial_data["myid_data"];

            # Return an error if myid_type is address, but the data is not an IP address.
            if ($myid_type === "address" and !is_ipaddr($myid_data)) {
                $this->errors[] = APIResponse\get(2176);
            }
            # Return an error if myid_type is fqdn, but the data is not an FQDN.
            elseif ($myid_type === "fqdn" and !is_fqdn($myid_data)) {
                $this->errors[] = APIResponse\get(2177);
            }
            # Return an error if myid_type is user_fqdn, but the data is not an email address.
            elseif ($myid_type === "user_fqdn" and !filter_var($myid_data, FILTER_VALIDATE_EMAIL)) {
                $this->errors[] = APIResponse\get(2178);
            }
            # Return an error if myid_type is dyn_dns, but the data is not an FQDN.
            elseif ($myid_type === "dyn_dns" and !is_fqdn($myid_data)) {
                $this->errors[] = APIResponse\get(2179);
            }
            # Otherwise, this input is valid.
            else {
                $this->validated_data["myid_data"] = $myid_data;
            }
        }
        # Force this value to an empty string if myid_type is myaddress
        elseif ($myid_type === "myaddress") {
            $this->validated_data["myid_data"] = "";
        }
    }

    private function __validate_peerid_type() {
        global $peer_identifier_list;

        # Check for the required 'peerid_type' field
        if ($this->initial_data["peerid_type"]) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["peerid_type"], $peer_identifier_list)) {
                $this->validated_data["peerid_type"] = $this->initial_data["peerid_type"];
            }
            else {
                $this->errors[] = APIResponse\get(2181);
            }
        }
    }

    private function __validate_peerid_data() {
        # Create a shortname variable for peerid_type for readability
        $peerid_type = $this->validated_data["peerid_type"];

        # Check for the conditional 'peerid_data' field. Validate this field further if myid_type is not myaddress
        if (isset($this->initial_data["peerid_data"]) and $peerid_type !== "myaddress") {
            # Create a shortname variable for my peerid_data for readability
            $peerid_data = $this->initial_data["peerid_data"];

            # Return an error if myid_type is address, but the data is not an IP address.
            if ($peerid_type === "address" and !is_ipaddr($peerid_data)) {
                $this->errors[] = APIResponse\get(2183);
            }
            # Return an error if myid_type is fqdn, but the data is not an FQDN.
            elseif ($peerid_type === "fqdn" and !is_fqdn($peerid_data)) {
                $this->errors[] = APIResponse\get(2184);
            }
            # Return an error if myid_type is user_fqdn, but the data is not an email address.
            elseif ($peerid_type === "user_fqdn" and !filter_var($peerid_data, FILTER_VALIDATE_EMAIL)) {
                $this->errors[] = APIResponse\get(2185);
            }
            # Return an error if myid_type is dyn_dns, but the data is not an FQDN.
            elseif ($peerid_type === "dyn_dns" and !is_fqdn($peerid_data)) {
                $this->errors[] = APIResponse\get(2186);
            }
            # Otherwise, this input is valid.
            else {
                $this->validated_data["peerid_data"] = $peerid_data;
            }
        }
        # Force this value to an empty string if myid_type is peeraddress
        elseif ($peerid_type === "peeraddress") {
            $this->validated_data["peerid_data"] = "";
        }
    }

    private function __validate_pre_shared_key() {
        # This field is only available if authentication method is set to pre-shared key
        if ($this->validated_data["authentication_method"] === "pre_shared_key") {
            # Check for the required 'pre-shared-key' field
            if (isset($this->initial_data["pre-shared-key"])) {
                $this->validated_data["pre-shared-key"] = (string)$this->initial_data["pre-shared-key"];
            }
            else {
                $this->errors[] = APIResponse\get(2187);
            }
        }
    }

    private function __validate_certref() {
        # This field is only available if the authentication method is set to cert
        if ($this->validated_data["authentication_method"] === "cert") {
            # Check for the required 'certref' field
            if (isset($this->initial_data["certref"])) {
                # Ensure the value is an existing certificate's refid
                if (APITools\is_certificate($this->initial_data["certref"])) {
                    $this->validated_data["certref"] = $this->initial_data["certref"];
                }
                else {
                    $this->errors[] = APIResponse\get(2189);
                }
            }
            else {
                $this->errors[] = APIResponse\get(2188);
            }
        }
    }

    private function __validate_caref() {
        # This field is only available if the authentication method is set to cert
        if ($this->validated_data["authentication_method"] === "cert") {
            # Check for the required 'caref' field
            if (isset($this->initial_data["caref"])) {
                # Ensure the value is an existing CA's refid
                if (APITools\is_ca($this->initial_data["caref"])) {
                    $this->validated_data["caref"] = $this->initial_data["caref"];
                }
                else {
                    $this->errors[] = APIResponse\get(2191);
                }
            }
            else {
                $this->errors[] = APIResponse\get(2190);
            }
        }
    }

    private function __validate_mode() {
        # This field is only available if the iketype is set to 'ikev1' or 'auto'
        if ($this->validated_data["iketype"] === "ikev1" or $this->validated_data["iketype"] === "auto") {
            # Check for the optional 'mode' field
            if (isset($this->initial_data["mode"])) {
                # Ensure the mode is valid option
                if (in_array($this->initial_data["mode"], ["main", "aggressive"])) {
                    $this->validated_data["mode"] = $this->initial_data["mode"];
                }
                else {
                    $this->errors[] = APIResponse\get(2192);
                }
            }
            # Defaults to 'main' mode
            else {
                $this->validated_data["mode"] = "main";
            }
        }
    }

    private function __validate_rekey_time() {
        # Check for the optional 'rekey_time' field
        if (isset($this->initial_data["rekey_time"])) {
            # Ensure value is a numeric greater than or equal to 0
            if (is_numeric($this->initial_data["rekey_time"]) and (int)$this->initial_data["rekey_time"] >= 0) {
                $this->validated_data["rekey_time"] = (int)$this->initial_data["rekey_time"];
            }
            else {
                $this->errors[] = APIResponse\get(2193);
            }
        }
    }

    private function __validate_reauth_time() {
        # Check for the optional 'reauth_time' field
        if (isset($this->initial_data["reauth_time"])) {
            # Ensure value is a numeric greater than or equal to 0
            if (is_numeric($this->initial_data["reauth_time"]) and (int)$this->initial_data["reauth_time"] >= 0) {
                $this->validated_data["reauth_time"] = (int)$this->initial_data["reauth_time"];
            }
            else {
                $this->errors[] = APIResponse\get(2194);
            }
        }
    }

    private function __validate_rand_time() {
        # Check for the optional 'rand_time' field
        if (isset($this->initial_data["rand_time"])) {
            # Ensure value is a numeric greater than or equal to 0
            if (is_numeric($this->initial_data["rand_time"]) and (int)$this->initial_data["rand_time"] >= 0) {
                $this->validated_data["rand_time"] = (int)$this->initial_data["rand_time"];
            }
            else {
                $this->errors[] = APIResponse\get(2195);
            }
        }
    }

    private function __validate_lifetime() {
        # Use shorthand variables for rekey_time, reauth_time for readability
        $rekey_time = (int)$this->validated_data["rekey_time"];
        $reauth_time = (int)$this->validated_data["reauth_time"];

        # Ensure value is a numeric greater than or equal to 0
        if (!is_numeric($this->initial_data["lifetime"]) or (int)$this->initial_data["lifetime"] < 0) {
            $this->errors[] = APIResponse\get(2196);
        }
        # If default 'rekey_time' was not assumed, ensure re-key time is less than lifetime.
        elseif($rekey_time >= (int)$this->initial_data["lifetime"]) {
            $this->errors[] = APIResponse\get(2197);
        }
        # If default 'reauth_time' was not assumed, ensure re-key time is less than lifetime.
        elseif($reauth_time >= (int)$this->initial_data["lifetime"]) {
            $this->errors[] = APIResponse\get(2197);
        }
        # Otherwise, input is valid.
        else {
            $this->validated_data["lifetime"] = (int)$this->initial_data["lifetime"];
        }
    }

    private function __validate_startaction() {
        global $ipsec_startactions;
        $remotegw = $this->validated_data["remote-gateway"];

        # Ensure this value is a known IPsec start action
        if (!array_key_exists($this->initial_data["startaction"], $ipsec_startactions)) {
            $this->errors[] = APIResponse\get(2198);
        }
        # Ensure the remote gateway is not 'any' unless the start action is none
        elseif (in_array($remotegw, ["0.0.0.0", "::"]) and $this->initial_data["startaction"] != "none") {
            $this->errors[] = APIResponse\get(2199);
        }
        # Otherwise, input is valid
        else {
            $this->validated_data["startaction"] = $this->initial_data["startaction"];
        }
    }

    private function __validate_closeaction() {
        global $ipsec_closeactions;

        # Check for the optional 'closeaction' field.
        if (isset($this->initial_data["closeaction"])) {
            # Check if this value is a valid option
            if (array_key_exists($this->initial_data["closeaction"], $ipsec_closeactions)) {
                $this->validated_data["closeaction"] = $this->initial_data["closeaction"];
            }
            else {
                $this->errors[] = APIResponse\get(2200);
            }
        }
    }

    private function __validate_nat_traversal() {
        # Check for the optional 'nat_traversal' option
        if (isset($this->initial_data["nat_traversal"])) {
            # Ensure value is valid option
            if (in_array($this->initial_data["nat_traversal"], ["on", "force"])) {
                $this->validated_data["nat_traversal"] = $this->initial_data["nat_traversal"];
            }
            else {
                $this->errors[] = APIResponse\get(2201);
            }
        }
    }

    private function __validate_gw_duplicates() {
        # Check for the optional 'gw_duplicates' field
        if ($this->initial_data["gw_duplicates"] === true) {
            $this->validated_data["gw_duplicates"] = true;
        }
        # Allow clients to unset this field
        elseif ($this->initial_data["gw_duplicates"] === false) {
            unset($this->validated_data["gw_duplicates"]);
        }
    }

    private function __validate_mobike() {
        # Check for the optional 'mobike' field
        if ($this->initial_data["mobike"] === true) {
            $this->validated_data["mobike"] = "on";
        }
        elseif ($this->initial_data["mobike"] === false) {
            $this->validated_data["mobike"] = "off";
        }
    }

    private function __validate_splitconn() {
        # Check for the optional 'splitconn' field
        if ($this->initial_data["splitconn"] === true) {
            $this->validated_data["splitconn"] = true;
        }
        # Allow clients to unset this field
        elseif ($this->initial_data["splitconn"] === false) {
            unset($this->validated_data["splitconn"]);
        }
    }

    private function __validate_prfselect_enable() {
        # Check for the optional 'prfselect_enable' field
        if ($this->initial_data["prfselect_enable"] === true) {
            $this->validated_data["prfselect_enable"] = "yes";
        }
        # Allow clients to unset this field
        elseif ($this->initial_data["prfselect_enable"] === false) {
            unset($this->validated_data["prfselect_enable"]);
        }
    }

    private function __validate_ikeport() {
        # Check for the optional 'ikeport' field
        if (isset($this->initial_data["ikeport"])) {
            # Ensure value is numeric and a valid TCP/UDP port
            if (is_numeric($this->initial_data["ikeport"]) and is_port((int)$this->initial_data["ikeport"])) {
                $this->validated_data["ikeport"] = (int)$this->initial_data["ikeport"];
            }
            else {
                $this->errors[] = APIResponse\get(2202);
            }
        }
    }

    private function __validate_nattport() {
        # Check for the optional 'nattport' field
        if (isset($this->initial_data["nattport"])) {
            # Ensure value is numeric and a valid TCP/UDP port
            if (!is_numeric($this->initial_data["nattport"]) or !is_port((int)$this->initial_data["nattport"])) {
                $this->errors[] = APIResponse\get(2203);
            }
            # Ensure value is not the same as the IKE port
            elseif ($this->validated_data["ikeport"] === (int)$this->initial_data["nattport"]) {
                $this->errors[] = APIResponse\get(2204);
            }
            else {
                $this->validated_data["nattport"] = (int)$this->initial_data["nattport"];
            }
        }
    }

    private function __validate_dpd_enable() {
        # Check for the optional 'dpd_enable' field
        if ($this->initial_data["dpd_enable"] === true) {
            $this->validated_data["dpd_enable"] = true;
        }
        # Allow clients to unset this field
        elseif ($this->initial_data["dpd_enable"] === false) {
            unset($this->validated_data["dpd_enable"]);
            unset($this->validated_data["dpd_delay"]);
            unset($this->validated_data["dpd_maxfail"]);
        }
    }

    private function __validate_dpd_delay() {
        # This field is only available when `dpd_enable` is set
        if ($this->validated_data["dpd_enable"]) {
            # Check for the optional 'dpd_delay' field
            if (isset($this->initial_data["dpd_delay"])) {
                # Ensure value is a numeric value
                if (is_numeric($this->initial_data["dpd_delay"]) and (int)$this->initial_data["dpd_delay"] >= 0) {
                    $this->validated_data["dpd_delay"] = $this->initial_data["dpd_delay"];
                }
                else {
                    $this->errors[] = APIResponse\get(2205);
                }
            }
            else {
                # Default value
                $this->initial_data["dpd_delay"] = 10;
            }
        }
    }

    private function __validate_dpd_maxfail() {
        # This field is only available when `dpd_enable` is set
        if ($this->validated_data["dpd_enable"]) {
            # Check for the optional 'dpd_maxfail' field
            if (isset($this->initial_data["dpd_maxfail"])) {
                # Ensure value is a numeric value
                if (is_numeric($this->initial_data["dpd_maxfail"]) and (int)$this->initial_data["dpd_maxfail"] >= 0) {
                    $this->validated_data["dpd_maxfail"] = $this->initial_data["dpd_maxfail"];
                }
                else {
                    $this->errors[] = APIResponse\get(2206);
                }
            }
            else {
                # Default value
                $this->initial_data["dpd_maxfail"] = 5;
            }
        }
    }

    public function __validate_encryption() {
        # Use the value nested in the 'item' field if it is present
        # This is needed because pfSense stores encryption items within a redundant 'item' array.
        if (isset($this->initial_data["encryption"]["item"])) {
            $this->initial_data["encryption"] = $this->initial_data["encryption"]["item"];
        }

        # Check for the required 'encryption' field
        if (isset($this->initial_data["encryption"])) {
            # Ensure value is an array and contains at least 1 item
            if (is_array($this->initial_data["encryption"]) and count($this->initial_data["encryption"]) >= 1) {
                # Initialize the validated data array that will hold valid configuration
                $this->validated_data["encryption"] = ["item"=>array()];

                # Loop through each encryption item and ensure it is valid
                foreach ($this->initial_data["encryption"] as $enc_conf) {
                    # Call the APIServicesIPsecPhase1EncryptionCreate model to validate entries
                    $enc_create = new APIServicesIPsecPhase1EncryptionCreate();
                    $enc_create->initial_data = $enc_conf;
                    $enc_create->validate_payload(true, $this->validated_data["prfselect_enable"]);
                    $this->validated_data["encryption"]["item"][] = $enc_create->validated_data;

                    # Add any encountered errors to this model's error array
                    $this->errors = array_merge($this->errors,  $enc_create->errors);
                }
            }
            else {
                $this->errors[] = APIResponse\get(2218);
            }
        }
    }

    public function is_ipsec_remote_gateway_in_use($value) {
        # Loop through each IPsec phase 1 and see if the value is in use
        foreach ($this->config['ipsec']['phase1'] as $index=>$p1) {
            # Check if this phase 1 entries remote gateway matches the value
            if ($p1["remote-gateway"] === $value) {
                # Allow this value if it is all IPv4
                if ($value === "0.0.0.0") {
                    continue;
                }
                # Allow this value if it is all IPv6
                elseif ($value === "::") {
                    continue;
                }
                # Allow this value if the matched entry is disabled
                elseif (isset($p1["disabled"])) {
                    continue;
                }
                # Allow this entry if both phase 1 entries allow duplicate gateways
                elseif (isset($this->validated_data["gw_duplicates"]) and isset($p1["gw_duplicates"]) and is_ipaddr($value)) {
                    continue;
                }
                # Allow this entry if it is the same entry we are updating
                elseif ($index === $this->id) {
                    continue;
                }
                # Otherwise, this remote gateway is already in use
                else {
                    return true;
                }
            }
        }

        return false;
    }

    private function __init_config() {
        # Initialize the config with the ipsec phase1 and phase2 sections
        init_config_arr(array('ipsec', 'phase1'));
        init_config_arr(array('ipsec', 'phase2'));
    }
}
