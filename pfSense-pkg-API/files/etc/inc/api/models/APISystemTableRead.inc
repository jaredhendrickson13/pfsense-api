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


class APISystemTableRead extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    public function action() {
        return APIResponse\get(0, $this->get_table_data());
    }

    public function validate_payload() {
        $this->__validate_name();
    }

    private function __validate_name() {
        # Validate our required name
        if (isset($this->initial_data["name"])) {
            # Ensure it is an available table
            if (in_array($this->initial_data["name"], $this->get_available_tables())) {
                $this->validated_data["name"] = $this->initial_data["name"];
            } else {
                $this->errors[] = APIResponse\get(1083);
            }
        }
    }

    public function get_available_tables() {
        exec("/sbin/pfctl -sT", $tables);
        return $tables;
    }

    public function get_table_data() {
        # Local variables
        $table_data = [];

        # Loop through each table and grab it's entries
        foreach ($this->get_available_tables() as $table_name) {
            # Only include this table if it matches our 'name' parameter or no parameters were specified.
            if ($table_name === $this->validated_data["name"] or empty($this->validated_data["name"])) {
                # Local variables
                $entries = [];
                exec("/sbin/pfctl -t " . escapeshellarg($table_name) . " -T show", $entries);

                # Loop through each table entry and remove excess whitespace, then add it to our table data
                foreach ($entries as $key => $ent) {
                    $entries[$key] = trim($ent);
                }
                $table_data[] = ["name" => $table_name, "entries" => $entries];
            }
        }

        return $table_data;
    }
}
