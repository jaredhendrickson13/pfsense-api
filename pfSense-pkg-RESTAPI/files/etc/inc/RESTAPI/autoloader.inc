<?php

# Include third-party libraries and built-in pfSense libraries required for this package
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

# An array of paths to each library within this package. The order in which they are defined in this array is the order
# they will be included by this autoloader.
const RESTAPI_LIBRARIES = [
    "/etc/inc/RESTAPI/Core/",
    "/etc/inc/RESTAPI/Dispatchers/",
    "/etc/inc/RESTAPI/Responses/",
    "/etc/inc/RESTAPI/Validators/",
    "/etc/inc/RESTAPI/Fields/",
    "/etc/inc/RESTAPI/Models/",
    "/etc/inc/RESTAPI/Endpoints/",
    "/etc/inc/RESTAPI/Forms/",
    "/etc/inc/RESTAPI/Tests/",
];

# Include each of the REST API libraries
foreach (RESTAPI_LIBRARIES as $library) {
    foreach(glob("$library/*.inc") as $inc_file) {
        require_once($inc_file);
    }
}

# Priv includes must be imported after RESTAPI/Core/Endpoint.inc because /etc/inc/priv/restapi.priv.inc requires
# the Endpoint classes to auto-generate Endpoint privileges
require_once("priv.defs.inc");
require_once("priv.inc");

