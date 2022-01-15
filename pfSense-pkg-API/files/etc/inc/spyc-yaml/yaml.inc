<?php
require_once("spyc-yaml/spyc.inc");

# Wrapper for the sypc_load() function to closer match json_decode() behavior
function yaml_decode($string) {
    try {
        return spyc_load($string);
    } catch (Exception $exception) {
        return [];
    }
}

# Wrapper for the sypc_dump() function to closer match json_encode() behavior
function yaml_encode($array) {
    try {
        return spyc_dump($array);
    } catch (Exception $exception) {
        return [];
    }
}
