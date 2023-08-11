<?php
const API_LIBRARIES = [
    "/etc/inc/api/core/",
    "/etc/inc/api/validators/",
    "/etc/inc/api/fields/",
    "/etc/inc/api/models/",
    "/etc/inc/api/views/",
    "/etc/inc/api/tests/",
];

foreach (API_LIBRARIES as $library) {
    foreach(glob("$library/*.inc") as $inc_file) {
        require_once($inc_file);
    }
}

