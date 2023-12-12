<?php
include_once("util.inc");
include_once("guiconfig.inc");
session_start();
?>
<!-- HTML for static distribution bundle build -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>pfSense REST API Documentation</title>
    <link rel="stylesheet" type="text/css" href="./swagger-ui.css" />
    <link rel="stylesheet" type="text/css" href="./index.css" />
    <link rel="apple-touch-icon-precomposed" href="/apple-touch/apple-touch-icon-iphone-60x60-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="60x60" href="/apple-touch/apple-touch-icon-ipad-76x76-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/apple-touch/apple-touch-icon-iphone-retina-120x120-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/apple-touch/apple-touch-icon-ipad-retina-152x152-precomposed.png">
    <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
    <link rel="mask-icon" href="/safari-pinned-tab.svg" color="#5bbad5">
</head>

<body>
<div id="swagger-ui"></div>
<script src="./swagger-ui-bundle.js" charset="UTF-8"> </script>
<script src="./swagger-ui-standalone-preset.js" charset="UTF-8"> </script>
<script src="./swagger-initializer.js" charset="UTF-8"> </script>
</body>
</html>
