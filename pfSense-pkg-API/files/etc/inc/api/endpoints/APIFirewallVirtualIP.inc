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

require_once("api/framework/APIEndpoint.inc");

class APIFirewallVirtualIP extends APIEndpoint {
    public function __construct() {
        parent::__construct();
        $this->url = "/api/v1/firewall/virtual_ip";
        $this->method_options = ["GET", "POST", "PUT", "DELETE"];
        $this->get_privileges = ["page-all", "page-firewall-virtualipaddresses"];
        $this->post_privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->put_privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->delete_privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
    }

    protected function get() : array {
        return (new APIFirewallVirtualIPRead($this->request_data, $this->client))->call();
    }

    protected function post() : array {
        return (new APIFirewallVirtualIPCreate($this->request_data, $this->client))->call();
    }

    protected function put() : array {
        return (new APIFirewallVirtualIPUpdate($this->request_data, $this->client))->call();
    }

    protected function delete() : array {
        return (new APIFirewallVirtualIPDelete($this->request_data, $this->client))->call();
    }

}
