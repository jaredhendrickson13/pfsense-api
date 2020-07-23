<?php
# IMPORTS
require_once("apicalls.inc");

# RUN API CALL
$resp = api_system_tables();
http_response_code($resp["code"]);
echo json_encode($resp) . PHP_EOL;
exit();