<?php
# Copyright 2020 - Jared Hendrickson
# IMPORTS
require_once("apicalls.inc");

# RUN API CALL
$resp = api_firewall_aliases_modify();
http_response_code($resp["code"]);
echo $resp;
exit();