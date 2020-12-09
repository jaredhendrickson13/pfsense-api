<?php
//   Copyright 2020 Jared Hendrickson
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


class APIStatusGatewayRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-gateways"];
    }

    public function action() {
        return APIResponse\get(0, $this->__get_metrics());
    }

    private function __get_metrics() {
        $gw_metrics_raw = return_gateways_status(true);
        $gw_metrics = [];

        # Loop through each gateway's metrics, format time data as floats, and update statuses
        foreach ($gw_metrics_raw as $gw) {
            $gw["delay"] = floatval(str_replace(["%", "ms"], "", $gw["delay"]));
            $gw["stddev"] = floatval(str_replace(["%", "ms"], "", $gw["stddev"]));
            $gw["loss"] = floatval(str_replace(["%", "ms"], "", $gw["loss"]));
            $gw["status"] = ($gw["status"] === "none") ? "up" : $gw["status"];
            $gw_metrics[] = $gw;
        }
        return $gw_metrics;
    }
}