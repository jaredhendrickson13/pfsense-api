<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APISystemHostnameModify extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system"];
        $this->change_note = "Modified system hostname via API";
    }

    public function action() {
        // Write our new hostname
        $this->write_config();    // Apply our configuration change
        // Update a slew of backend services
        if (isset($hostname) or isset($domain)) {
            system_hostname_configure();
            system_hosts_generate();
            system_resolvconf_generate();
            if (isset($this->config['dnsmasq']['enable'])) {
                services_dnsmasq_configure();
            } elseif (isset($this->config['unbound']['enable'])) {
                services_unbound_configure();
            }
            filter_configure();
        }
        $data = ["hostname" => $this->config["system"]["hostname"], "domain" => $this->config["system"]["domain"]];
        return APIResponse\get(0, $data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['hostname'])) {
            $hostname = $this->initial_data['hostname'];
            $hostname = trim($hostname);
            // Check if our hostname is valid
            if (!is_hostname($hostname) or !is_unqualified_hostname($hostname)) {
                $this->errors[] = APIResponse\get(1000);
            } else {
                $this->config["system"]["hostname"] = $hostname;
            }
        }
        if (isset($this->initial_data['domain'])) {
            $domain = $this->initial_data['domain'];
            $domain = trim($domain);
            // Check if our hostname is valid
            if (!is_domain($domain)) {
                $this->errors[] = APIResponse\get(1001);
            } else {
                $this->config["system"]["domain"] = $domain;
            }
        }
    }
}
