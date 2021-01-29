<?php
//   Copyright 2021 Jared Hendrickson
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


class APIFirewallStatesRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-statessummary"];
    }

    public function action() {
        return APIResponse\get(0, $this->get_state_table());
    }

    public static function get_state_table() {
        $raw_table = pfSense_get_pf_states();
        return array_map(function($table) {
            return array(
                'interface' => $table['if'],
                'protocol' => $table['proto'],
                'source' => $table['src'],
                'destination' => $table['dst'],
                'status' => $table["state"],
                'age' => $table["age"],
                "expires_in" => $table["expires in"],
                "packets_total" => $table["packets total"],
                "packets_in" => $table["packets in"],
                "packets_out" => $table["packets out"],
                "bytes_total" => $table["bytes total"],
                "bytes_in" => $table["bytes in"],
                "bytes_out" => $table["bytes out"],
            );
        }, $raw_table);
    }
}