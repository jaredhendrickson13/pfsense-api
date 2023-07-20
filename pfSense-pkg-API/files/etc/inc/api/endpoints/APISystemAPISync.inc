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

class APISystemAPISync extends APIEndpoint {
    public function __construct() {
        parent::__construct();
        $this->url = "/api/v1/system/api/sync";
        $this->method_options = ["GET"];
        $this->put_privileges = ["page-all", "page-system-api"];
        $this->set_auth_mode = "local";
        $this->retain_read_mode = false;
        $this->ignore_enabled = true;
        $this->ignore_ifs = true;
    }

    protected function put() : array {
        return (new APISystemAPISyncUpdate($this->request_data, $this->client))->call();
    }
}
