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

class APIServicesIPsecPhase1 extends APIEndpoint {
    public function __construct() {
        parent::__construct();
        $this->url = "/api/v1/services/ipsec/phase1";
        $this->method_options = ["GET", "POST", "PUT", "DELETE"];
        $this->get_privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->post_privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->put_privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->delete_privileges = ["page-all", "page-vpn-ipsec-editphase1"];
    }

    protected function get() {
        return (new APIServicesIPsecPhase1Read())->call();
    }

    protected function post() {
        return (new APIServicesIPsecPhase1Create())->call();
    }

    protected function put() {
        return (new APIServicesIPsecPhase1Update())->call();
    }

    protected function delete() {
        return (new APIServicesIPsecPhase1Delete())->call();
    }
}
