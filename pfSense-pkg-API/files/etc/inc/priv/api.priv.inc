<?php
global $priv_list;

$priv_list['page-system-api'] = array();
$priv_list['page-system-api']['name'] = "WebCfg - System: API package";
$priv_list['page-system-api']['descr'] = "Allow access to API package UI";
$priv_list['page-system-api']['match'] = array();
$priv_list['page-system-api']['match'][] = "api*";