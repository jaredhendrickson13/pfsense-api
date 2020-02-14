<?php
require_once("apiresp.inc");
header("Content-Type: application/json", true);
// Pull our error library as JSON and simply print it
$err_json = export_err_lib();
echo $err_json;