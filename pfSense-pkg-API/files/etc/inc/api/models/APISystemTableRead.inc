<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemTableRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-tables"];
    }

    public function action() {
        $tables_all = [];
        exec("/sbin/pfctl -sT", $tables);

        # Loop through each table and grab it's entries
        foreach ($tables as $tablename) {
            $entries = [];
            exec("/sbin/pfctl -t " . escapeshellarg($tablename) . " -T show", $entries);
            foreach ($entries as $key=>$ent) {$entries[$key] = trim($ent);}
            $tables_all[] = [
                "name" => $tablename,
                "entries" => $entries
            ];
        }

        return APIResponse\get(0, $tables_all);
    }
}