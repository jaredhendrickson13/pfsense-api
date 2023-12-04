<?php

require_once("firebase/php-jwt/src/JWT.php");
require_once("firebase/php-jwt/src/JWK.php");
require_once("firebase/php-jwt/src/Key.php");
require_once("firebase/php-jwt/src/CachedKeySet.php");
require_once("firebase/php-jwt/src/JWTExceptionWithPayloadInterface.php");
require_once("firebase/php-jwt/src/ExpiredException.php");
require_once("firebase/php-jwt/src/SignatureInvalidException.php");
require_once("firebase/php-jwt/src/BeforeValidException.php");
require_once("firebase/php-jwt/src/JWTExceptionWithPayloadInterface.php");
require_once("config.inc");
require_once('diag_arp.inc');
require_once("util.inc");
require_once("interfaces.inc");
require_once("interfaces_fast.inc");
require_once("service-utils.inc");
require_once("filter.inc");
require_once("shaper.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("openvpn.inc");
require_once("certs.inc");
require_once("pkg-utils.inc");
require_once("firewall_virtual_ip.inc");

const API_LIBRARIES = [
    "/etc/inc/api/core/",
    "/etc/inc/api/dispatchers/",
    "/etc/inc/api/responses/",
    "/etc/inc/api/validators/",
    "/etc/inc/api/fields/",
    "/etc/inc/api/models/",
    "/etc/inc/api/endpoints/",
    "/etc/inc/api/forms/",
    "/etc/inc/api/tests/",
];

foreach (API_LIBRARIES as $library) {
    foreach(glob("$library/*.inc") as $inc_file) {
        require_once($inc_file);
    }
}

# Priv includes must be imported after api/core/Endpoint.inc
require_once("priv.defs.inc");
require_once("priv.inc");

