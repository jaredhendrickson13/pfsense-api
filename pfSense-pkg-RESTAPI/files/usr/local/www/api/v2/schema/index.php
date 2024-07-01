<?php
# Simply read and print the OpenAPI schema.json file
header('Content-Type: application/json');
echo file_get_contents('/usr/local/pkg/RESTAPI/.resources/schema.json');
