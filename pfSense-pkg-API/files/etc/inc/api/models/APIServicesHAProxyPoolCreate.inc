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
require_once("haproxy/haproxy.inc");
require_once("haproxy/haproxy_utils.inc");

class APIServicesHAProxyPoolCreate extends APIModel {

    private $backend_defaults = [
        "ha_servers" => [
           "item" => [] 
        ], 
        "a_acl" => "", 
        "a_actionitems" => "", 
        "errorfiles" => "", 
        "advanced" => "", 
        "advanced_backend" => "aHR0cC1yZXNwb25zZSBzZXQtaGVhZGVyIFgtU2VydmVkLUJ5ICVzIHVubGVzcyB7IHJlcy5oZHIoWC1TZXJ2ZWQtQnkpIC1tIGZvdW5kIH0=", 
        "name" => "", 
        "balance" => "", 
        "balance_urilen" => "", 
        "balance_uridepth" => "", 
        "balance_uriwhole" => "", 
        "transparent_clientip" => "", 
        "transparent_interface" => "wan", 
        "check_type" => "HTTP", 
        "checkinter" => "5000", 
        "log-health-checks" => "", 
        "httpcheck_method" => "GET", 
        "monitor_uri" => "/ hdr Host healthcheck.lw", 
        "monitor_httpversion" => "", 
        "monitor_username" => "", 
        "monitor_domain" => "", 
        "monitor_agentport" => "template-https", 
        "agent_check" => "", 
        "agent_port" => "", 
        "agent_inter" => "", 
        "connection_timeout" => "300000", 
        "server_timeout" => "300000", 
        "retries" => "", 
        "stats_enabled" => "", 
        "stats_username" => "", 
        "stats_password" => "", 
        "stats_uri" => "", 
        "stats_scope" => "", 
        "stats_realm" => "", 
        "stats_admin" => "", 
        "stats_node" => "", 
        "stats_desc" => "", 
        "stats_refresh" => "", 
        "persist_stick_expire" => "60m", 
        "persist_stick_tablesize" => "256m", 
        "persist_stick_length" => "32", 
        "persist_stick_cookiename" => "X-HAProxy-Session", 
        "persist_sticky_type" => "none", 
        "persist_cookie_enabled" => "", 
        "persist_cookie_name" => "X-HAProxy-Session", 
        "persist_cookie_mode" => "insert-only", 
        "persist_cookie_cachable" => "", 
        "persist_cookie_postonly" => "", 
        "persist_cookie_httponly" => "yes", 
        "persist_cookie_secure" => "yes", 
        "haproxy_cookie_maxidle" => "", 
        "haproxy_cookie_maxlife" => "", 
        "haproxy_cookie_domains" => "", 
        "haproxy_cookie_dynamic_cookie_key" => "", 
        "strict_transport_security" => "", 
        "cookie_attribute_secure" => "", 
        "email_level" => "", 
        "email_to" => ""
    ];
    private $allowed_server_ssl = ['yes', 'no'];
    private $allowed_algorithms = ['roundrobin', 'leastconn'];
    private $allowed_persistence = ['stick_sourceipv4', 'stick_cookie_value', 'none'];    
    private $next_ids;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->requires_auth = false;
        $this->allowed_algorithms = $allowed_algorithms;
        $this->allowed_persistence = $allowed_persistence;
        $this->backend_defaults = $backend_defaults;
        $this->server_backend_ids = $next_ids;
        $this->current_backends = $this->get_config("installedpackaged/haproxy/ha_pools/item", []);
        
    }

    public function validate_payload() {
        $this->__validate_backend();
        $this->__validate_servers($this->initial_data['servers']);
        $this->__set_server_backend_ids();
        $this->__populate_defaults();
    }

    private function __validate_backend() {

        if ($this->__validate_backend_name()) {
            $this->validated_data['name'] = $this->initial_data['name'];
        }

        if (!in_array($this->initial_data['persist_sticky_type'], $this->allowed_persistence)) {
            $this->errors[] = APIResponse\get(2264, $this->initial_data['persist_sticky_type']);
        } else {
            $this->validated_data['persist_sticky_type'] = $this->initial_data['persist_sticky_type'];
        }

        if (!in_array($this->initial_data['balance'], $this->allowed_algorithms)) {
            $this->errors[] = APIResponse\get(2265, $this->initial_data['balance']);
        } else {
            $this->validated_data['balance'] = $this->initial_data['balance'];
        }

        if (!in_array($this->initial_data['check_type'], $this->allowed_checks)) {
            $this->errors[] = APIResponse\get(2266, $this->initial_data['check_type']);
        } else {
            $this->validated_data['check_type'] = $this->initial_data['check_type'];
        }

    }

    private function __validate_servers($servers) {

        foreach ($servers as $server) {
            $server_bad_name = false;
            $server_bad_address = false;
            $server_bad_port = false;
            $server_bad_ssl = false;

            if (preg_match("/[^a-zA-Z0-9\.\-_]/", $server['name'])) {
                $this->errors[] = APIResponse\get(2267, $server['name']);
                $server_bad_name = true;
            }
            if (!preg_match("/.{2,}/", $server['name'])) {
                $this->errors[] = APIResponse\get(2268, $server['name']);
                $server_bad_name = true;
            }
            if (!is_ipaddr($server['address'])) {
                $this->errors[] = APIResponse\get(2269, [$server['name'] => $server['address']]);
                $server_bad_address = true;
            }
            if (!is_numeric($server['port'])) {
                $this->errors[] = APIResponse\get(2270, [$server['name'] => $server['port']]);
                $server_bad_address = true;
            } 
            if (!array_key_exists('ssl', $server) || !in_array($server['ssl'], $this->allowed_server_ssl)) {
                $this->errors[] = APIResponse\get(2271, [$server['name'] => $server['ssl']]);
                $server_bad_ssl = true;
            } elseif ($server['ssl'] === 'no') {
                unset($server['ssl']);
            }
            if(!$server_bad_name && !$server_bad_address && !$server_bad_port && !$server_bad_ssl) {
                $server['cookie'] = $server['name'];
                $this->validated_data['ha_servers']['item'][] = $server;
            } 
        }
    }

    private function __validate_backend_name() {
        $bad_pool_name = false;
        /* Ensure only valid characters are in use*/
        if (preg_match("/[^a-zA-Z0-9_\.\-]/", $this->initial_data['name']) || str_contains($this->initial_data['name'], ' ')) {
            $this->errors[] = APIResponse\get(2262, $this->initial_data['name']);
            $name_bad_chars = true;
        }

        /* Ensure that our pool names are unique */
        $current_backends = get_haproxy_backends();
        foreach ($current_backends as $name => $settings) {
            if ($this->initial_data['name'] == $name) {
                $this->errors[] = APIResponse\get(2263, $this->initial_data['name']);
                $name_already_used = true;
            }
        } 

        if ($name_already_used || $name_bad_chars) {
            return false;
        } else {
            return true;
        }

    }

    private function __populate_defaults() {

        foreach ($this->backend_defaults as $setting => $value) {
            if (!array_key_exists($setting, $this->validated_data)) {
                $this->validated_data[$setting] = $this->backend_defaults[$setting];
            }
        }

    }

    private function __set_server_backend_ids() {
        $id = 100; 
        $new_backends = $this->validated_data['ha_servers']['item'];
        foreach($new_backends as &$backend) {
            if (!is_numeric($backend['id'])) {
                while(find_server_by_id($id) || find_backend_by_id($id)) {
                    // find unused id..
                    $id++;
                }    
                $backend['id'] = $id; 
            }    
            $servers = &getarraybyref($backend,'ha_servers','item');
            foreach($servers as &$server) {
                if (!is_numeric($server['id'])) {
                    while(find_server_by_id($id) || find_backend_by_id($id)) {
                        // find unused id..
                        $id++;
                    }    
                    $server['id'] = $id; 
                }    
            }    
        } 
    }

    # Tell our API model what to do after successfully validating the client's request
    public function action(){

        $this->id = $this->get_next_id("installedpackages/haproxy/ha_pools/item");
        $this->set_config("installedpackages/haproxy/ha_pools/item/{$this->id}", $this->validated_data);
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config(); 

        $result = haproxy_check_and_run('Applying backend changes via API.', true);
        if (!$result) {
            // If it fails, reset config back to original and apply.
            $this->set_config("installedpackages/haproxy/ha_pools/item", $this->current_backends);
            $this->write_config();
            $result = haproxy_check_and_run('Reverting back to original configuration', true);
            $this->initial_data['haproxy_result'] = $result;
            return APIResponse\get(2272, $this->initial_data);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, 'Backend successfully created!');
        }
        
    }
}
