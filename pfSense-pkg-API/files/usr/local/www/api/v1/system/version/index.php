<?php
# Copyright 2020 - Jared Hendrickson
require_once("api/api_models/APISystemVersion.inc");
(new APISystemVersion())->listen();