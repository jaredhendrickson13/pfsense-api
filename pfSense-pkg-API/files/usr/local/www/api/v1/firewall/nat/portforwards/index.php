<?php
# Copyright 2020 - Jared Hendrickson
# IMPORTS
require_once("apicalls.inc");

# RUN API CALL
$resp = api_firewall_nat_portforwards();
http_response_code($resp["code"]);
echo json_encode($resp) . PHP_EOL;
exit();
