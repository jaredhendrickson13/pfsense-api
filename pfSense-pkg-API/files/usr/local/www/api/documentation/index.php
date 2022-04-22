<?php
require_once("api/framework/APITools.inc");
include_once("util.inc");
include_once("guiconfig.inc");
session_start();

# Redirect user if they do not have privilege to access this page
$user_privs = get_user_privileges(getUserEntry($_SESSION["Username"]));
if (!in_array("page-system-api", $user_privs) and !in_array("page-all", $user_privs)) {
    header("Location: /");
    exit();
}
?>
<!-- HTML for static distribution bundle build -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>pfSense REST API Documentation</title>
    <link rel="stylesheet" type="text/css" href="./swagger-ui.css" />
    <link rel="stylesheet" type="text/css" href="index.css" />
    <link rel="icon" type="image/png" href="./favicon-32x32.png" sizes="32x32" />
    <link rel="icon" type="image/png" href="./favicon-16x16.png" sizes="16x16" />
</head>

<body>
<div id="swagger-ui"></div>
<script src="./swagger-ui-bundle.js" charset="UTF-8"> </script>
<script src="./swagger-ui-standalone-preset.js" charset="UTF-8"> </script>
<script src="./swagger-initializer.js" charset="UTF-8"> </script>
</body>
</html>
