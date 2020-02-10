<?php
# Copyright 2020 - Jared Hendrickson
# IMPORTS
require_once("apicalls.inc");

# RUN API CALL
$resp = api_users_authservers_radius_delete();
http_response_code($resp["code"]);
echo json_encode($resp) . PHP_EOL;
exit();
